//------------------------------------------------------------------------------
//  nipcserver.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nthread.h"
#include "kernel/nipcserver.h"
#include "util/nhashtable.h"
#include "kernel/nipcminiserver.h"

//------------------------------------------------------------------------------
/**
    The listener thread. Creates one nIpcMiniServer object
    for each client which connects.
*/
int
N_THREADPROC
n_listener_tfunc(nThread *t)
{
    // tell thread object that we have started
    t->ThreadStarted();

    // get pointer to ipc server object
    nIpcServer *is = (nIpcServer *) t->LockUserData();
    t->UnlockUserData();

    do
    {
        nIpcMiniServer *ims;
        n_printf("nIpcServer: listening on port %d...\n", ntohs(is->hostAddr.sin_port));
        ims = n_new nIpcMiniServer(is);
        if (ims)
        {
            if (ims->Listen())
            {
                // a connection was requested
                // requests can come from the Wakeupfunc.
                // query ThreadStopRequested()
                // to find out...
                if (t->ThreadStopRequested())
                {
                    ims->Ignore();
                    n_printf("nIpcServer: wakeupfunc client connected, shutting down.\n");
                }
                else
                {
                    n_printf("nIpcServer: a client has connected.\n");
                }
            }
        }
    } while (!t->ThreadStopRequested());

    n_printf("nIpcServer: shutting down listener thread.\n");
    t->ThreadHarakiri();
    return 0;
}

//------------------------------------------------------------------------------
/**
    Wake up the Listener Thread
*/
void
n_listener_wakeup(nThread *t)
{
    sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    
    nIpcServer *is = (nIpcServer *) t->LockUserData();
    t->UnlockUserData();

    my_addr.sin_family = AF_INET;
    my_addr.sin_port   = is->hostAddr.sin_port; 

#if defined(__WIN32__)
    my_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#elif defined(__LINUX__)
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif

    SOCKET my_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (my_sock != INVALID_SOCKET) 
    {
        int res;
        res = connect(my_sock, (struct sockaddr *) &my_addr, sizeof(my_addr));
        if (res != 0) 
        {
            n_printf("n_listener_wakeup, connection failed!\n");
        }
        shutdown(my_sock, 2);
        closesocket(my_sock);
    }                
}

//------------------------------------------------------------------------------
/**
     This computes the actual port number given a portname. Portnames are 
     converted to number by a simple hash. 

     @param portName pointer to the portname

     @return the port number
*/
short 
nIpcServer::GetPortNumFromName(const char* portName)
{
    short pnum = ((short)hash(portName, N_SOCKET_PORTRANGE)) + N_SOCKET_MIN_PORTNUM;
    return pnum;
} 

//------------------------------------------------------------------------------
/**
    This constructor starts up the server and sets it listening on the 
    portname given.

    @param portName pointer to the portname 
*/
nIpcServer::nIpcServer(const char *portName)
{
    n_assert(portName);

    this->numMiniServers = 0;
    this->pname = portName;
    memset(&(this->hostAddr), 0, sizeof(this->hostAddr));
            
    // create socket
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    n_assert(this->sock != INVALID_SOCKET);

    int true_int = 1;
    int res = setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&true_int, sizeof(true_int)); 
    n_assert(res != -1);
        
    // Bind the socket to an address
    this->hostAddr.sin_family = AF_INET;
    this->hostAddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(this->hostAddr.sin_zero), 0, sizeof(this->hostAddr.sin_zero));
    
    bool bound = false;
    int portNum = this->GetPortNumFromName(portName);
    while (!bound) 
    {
        this->hostAddr.sin_port = htons(portNum);
        res = bind(this->sock, (struct sockaddr *) &(this->hostAddr), sizeof(this->hostAddr));
        if (res != 0) 
        {
            if (portNum < N_SOCKET_MAX_PORTNUM) 
            {
                portNum++;
                this->hostAddr.sin_port = htons(portNum);
            } 
            else 
            {
                n_error("nIpcServer::nIpcServer(): max number of servers started!");
            }
        } 
        else 
        {
            bound = true;
        }
    }     
    
    // Start listener Thread...
    this->listenerThread = n_new nThread(n_listener_tfunc,
                                         nThread::Normal,
                                         0,                  // Default-Stacksize
                                         n_listener_wakeup,
                                         &(this->msgList),
                                         (void *) this);
    n_assert(this->listenerThread);
}

//------------------------------------------------------------------------------
/**
     The destructor deletes the list of current mini servers.
*/
nIpcServer::~nIpcServer()
{
    n_printf("-> ~nIpcServer()\n");
    
    // the listener thread must be killed before the mini servers
    // as it uses it's own mini server
    n_delete this->listenerThread;

    nIpcMiniServer *ims;
    this->miniServerList.Lock();
    while ((ims = (nIpcMiniServer *) this->miniServerList.RemHead())) 
    {
        n_delete ims;
    }
    this->miniServerList.Unlock();
    
    if (this->sock)
    {
        shutdown(this->sock, 2);
        closesocket(this->sock);
        this->sock = 0;
    }
    
    nMsgNode *nd;
    while ((nd = (nMsgNode *) this->msgList.RemHead())) 
    {
        n_delete nd;
    }
    
    n_printf("<- ~nIpcServer()\n");    
}

//------------------------------------------------------------------------------
/**
    Poll the mini servers for new messages. 
    
    @return true if there are any to process
*/
bool
nIpcServer::Poll()
{
    // poll miniservers...
    nIpcMiniServer *ims;
    this->miniServerList.Lock();
    for (ims = (nIpcMiniServer *) this->miniServerList.GetHead();
         ims;
         ims = (nIpcMiniServer *) ims->GetSucc())
    {
        ims->Poll();
    }
    this->miniServerList.Unlock();

    // check for new messages on the msg list...
    this->msgList.Lock();
    nMsgNode* first = (nMsgNode*) this->msgList.GetHead();
    this->msgList.Unlock();

    return (first != 0);
}

//------------------------------------------------------------------------------
/**
     Get next message. This checks for messages from any connected clients.

     @param outClientId [out] the client id, it will be -1 if there is no message.

     @return nMsgNode with message
*/
nMsgNode*
nIpcServer::GetMsg(int& outClientId)
{
    // check for messages
    this->msgList.Lock();
    nMsgNode* nd = (nMsgNode *) this->msgList.RemHead();
    this->msgList.Unlock();

    if (nd) 
    {
        outClientId = (int) nd->GetPtr();
    }
    else    
    {
        outClientId = -1;
    }
    return nd;
}

//------------------------------------------------------------------------------
/**
     Tells the listener thread that you are done with this nMsgNode.
*/
void 
nIpcServer::ReplyMsg(nMsgNode* nd)
{
    this->listenerThread->ReplyMsg(nd);
}
    
//------------------------------------------------------------------------------
/**
     @param buf pointer to message data
     @param size size of message data
     @param clientId the client to send the message to

     @return true on success
*/
bool 
nIpcServer::AnswerMsg(void* buf, int size, int clientId)
{
    n_assert(buf);
    n_assert(size > 0);
    n_assert(clientId >= 0);
    bool retval = false;
    
    // find client
    nIpcMiniServer *ims;
    this->miniServerList.Lock();
    for (ims = (nIpcMiniServer *) this->miniServerList.GetHead();
         ims;
         ims = (nIpcMiniServer *) ims->GetSucc())
    {
        if (clientId == ims->id) 
        {
            // send msg
            ims->Send(buf, size);
            retval = true;
            break;
        }
    }
    this->miniServerList.Unlock();

    return retval;
}

//------------------------------------------------------------------------------
/**
    Sends a message to all connected clients

    @param buf message data
    @param size size of message
*/
void
nIpcServer::BroadcastMsg(void* buf, int size)
{
    nIpcMiniServer *ims;
    this->miniServerList.Lock();
    for (ims = (nIpcMiniServer *) this->miniServerList.GetHead();
         ims;
         ims = (nIpcMiniServer *) ims->GetSucc())
    {
        ims->Send(buf, size);
    }
    this->miniServerList.Unlock();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
