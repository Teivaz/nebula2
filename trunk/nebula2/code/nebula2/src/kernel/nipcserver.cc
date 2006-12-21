//------------------------------------------------------------------------------
//  nipcserver.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nthread.h"
#include "kernel/nipcserver.h"
#include "kernel/nipcminiserver.h"

//------------------------------------------------------------------------------
/**
    The listener thread. Simply creates one nIpcMiniServer object
    for each client which connects.
*/
static
int
N_THREADPROC
ListenerThreadFunc(nThread *thread)
{
    // tell thread object that we have started
    thread->ThreadStarted();

    // get pointer to ipc server object
    nIpcServer *ipcServer = (nIpcServer *) thread->LockUserData();
    thread->UnlockUserData();

    do
    {
        // this loop waits for a new client to connect, for each
        // new client, an nIpcMiniServer object is created
        n_printf("nIpcServer: listening on port %d...\n", ipcServer->selfAddr.GetPortNum());
        nIpcMiniServer* ipcMiniServer = n_new(nIpcMiniServer(ipcServer));
        if (ipcMiniServer->Listen())
        {
            // Some sort of connection has been established. This
            // could also come from the Wakeup function of
            // nIpcMiniServer if it wants to shutdown the thread.
            // If this is the case, the ThreadStopRequested flag
            // should be set in the thread object.
            if (thread->ThreadStopRequested())
            {
                ipcMiniServer->Ignore();
                n_printf("nIpcServer: woke up for shutting down.\n");
            }
            else
            {
                n_printf("nIpcServer: a client has connected.\n");
            }
        }
        n_sleep(0); // call the sheduler to be multitask friendly
    } while (!thread->ThreadStopRequested());

    n_printf("nIpcServer: shutting down listener thread.\n");
    thread->ThreadHarakiri();
    return 0;
}

//------------------------------------------------------------------------------
/**
    The wakeup func for the thread object.
    This creates a dummy connection to the current ipcMiniServer object
    in the listener thread in order to wakeup the thread because it is
    going to shut down.
*/
static
void
ListenerWakeupFunc(nThread* t)
{
    sockaddr_in myAddr;
    memset(&myAddr, 0, sizeof(myAddr));
    nIpcServer* ipcServer = (nIpcServer*) t->LockUserData();
    t->UnlockUserData();
    myAddr.sin_family = AF_INET;
    myAddr.sin_port   = ipcServer->selfAddr.GetAddrStruct().sin_port;
    myAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    n_assert(INVALID_SOCKET != sock);
    connect(sock, (struct sockaddr *) &myAddr, sizeof(myAddr));
    shutdown(sock, 2);
    closesocket(sock);
}

//------------------------------------------------------------------------------
/**
    NOTE: the host name of the ipc address object MUST be set to "any",
    the port name must be initialized with a valid portname.
*/
nIpcServer::nIpcServer(nIpcAddress& addr) :
    uniqueMiniServerId(0),
    selfAddr(addr)
{
    // create a socket
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    n_assert(INVALID_SOCKET != this->sock);

    // configure the socket
    int trueAsInt = 1;
    int res = setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&trueAsInt, sizeof(trueAsInt));
    n_assert(res != -1);

    // bind address to socket
    res = bind(this->sock, (const sockaddr*) &(addr.GetAddrStruct()), sizeof(addr.GetAddrStruct()));
    n_assert(SOCKET_ERROR != res);

    // start the listener thread
    this->listenerThread = n_new(nThread(ListenerThreadFunc,
                                         nThread::Normal,
                                         0,
                                         ListenerWakeupFunc,
                                         &(this->msgList),
                                         (void*) this));
}

//------------------------------------------------------------------------------
/**
     The destructor deletes the list of current mini servers.
*/
nIpcServer::~nIpcServer()
{
    // delete the thread before the mini servers, because inside
    // the thread there is a living nIpcMiniServer object waiting
    // for connections
    n_delete(this->listenerThread);
    this->listenerThread = 0;

    // kill existing mini servers
    nIpcMiniServer* ipcMiniServer;
    this->miniServerList.Lock();
    while ((ipcMiniServer = (nIpcMiniServer*) this->miniServerList.RemHead()))
    {
        n_delete(ipcMiniServer);
    }
    this->miniServerList.Unlock();

    // shutdown the server socket
    if (INVALID_SOCKET != this->sock)
    {
        shutdown(this->sock, 0);
        closesocket(this->sock);
        this->sock = 0;
    }

    // delete pending messages
    nMsgNode* msgNode;
    this->msgList.Lock();
    while ((msgNode = (nMsgNode*)this->msgList.RemHead()))
    {
        n_delete(msgNode);
    }
    this->msgList.Unlock();
}

//------------------------------------------------------------------------------
/**
    Poll the mini servers for new messages.

    @return true if there are any to process
*/
bool
nIpcServer::Poll()
{
    // poll all our miniservers...
    this->miniServerList.Lock();
    nIpcMiniServer* cur = (nIpcMiniServer*) this->miniServerList.GetHead();
    nIpcMiniServer* next = 0;
    if (cur) do
    {
        next = (nIpcMiniServer*) cur->GetSucc();
        if (!cur->Poll())
        {
            // this ipc mini server has a closed connection, delete it
            cur->Remove();
            this->ClientsReseted.Append(cur->GetClientId());
            n_delete(cur);
            cur = 0;
        }
    }
    while ((cur = next));
    this->miniServerList.Unlock();

    // check for new messages on the msg list...
    this->msgList.Lock();
    nMsgNode* first = (nMsgNode*)this->msgList.GetHead();
    this->msgList.Unlock();

    return (first != 0);
}

//------------------------------------------------------------------------------
/**
    Remove the next message from the message list and copy its content into
    the provided nIpcBuffer object. The fromClientId arg will be filled
    with the id of the client which has sent the message (may be needed
    to send an answer back to the client later).
*/
bool
nIpcServer::GetMsg(nIpcBuffer& msg, int& fromClientId)
{
    // check if any messages came in...
    nMsgNode* msgNode = (nMsgNode*)this->listenerThread->GetMsg();
    if (msgNode)
    {
        // copy contents of message to the nIpcBuffer object
        msg.Set((const char*) msgNode->GetMsgPtr(), msgNode->GetMsgSize());
        fromClientId = (int) msgNode->GetPtr();
        this->listenerThread->ReplyMsg(msgNode);
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Send a message to a specific client.
    @return true on success
*/
bool
nIpcServer::Send(int toClientId, const nIpcBuffer& msg)
{
    bool retval = false;

    // find the right mini server
    nIpcMiniServer* ipcMiniServer;
    this->miniServerList.Lock();
    for (ipcMiniServer = (nIpcMiniServer*) this->miniServerList.GetHead();
         ipcMiniServer;
         ipcMiniServer = (nIpcMiniServer*) ipcMiniServer->GetSucc())
    {
        if (toClientId == ipcMiniServer->GetClientId())
        {
            ipcMiniServer->Send(msg);
            retval = true;
            break;
        }
    }
    this->miniServerList.Unlock();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Send a message to all clients.
*/
bool
nIpcServer::SendAll(const nIpcBuffer& msg)
{
    nIpcMiniServer* ipcMiniServer;
    this->miniServerList.Lock();
    for (ipcMiniServer = (nIpcMiniServer*) this->miniServerList.GetHead();
         ipcMiniServer;
         ipcMiniServer = (nIpcMiniServer*) ipcMiniServer->GetSucc())
    {
       ipcMiniServer->Send(msg);
    }
    this->miniServerList.Unlock();
    return true;
}
