//------------------------------------------------------------------------------
//  nipcminiserver.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nipcserver.h"
#include "kernel/nipcminiserver.h"
#include "kernel/nthread.h"

//------------------------------------------------------------------------------
/**
*/
nIpcMiniServer::nIpcMiniServer(nIpcServer *server) :
    msgBuffer(4096),
    isConnected(false)
{
    n_assert(server);

    this->ipcServer  = server;
    this->clientId   = this->ipcServer->uniqueMiniServerId++;
    this->rcvrSocket = INVALID_SOCKET;

    this->ipcServer->miniServerList.Lock();
    this->ipcServer->miniServerList.AddTail(this);
    this->ipcServer->miniServerList.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
nIpcMiniServer::~nIpcMiniServer()
{
    n_printf("-> ~nIpcMiniServer\n");
    this->CloseRcvrSocket();
    n_printf("<- ~nIpcMiniServer\n");
}

//------------------------------------------------------------------------------
/**
    Close the receiver socket.
*/
void
nIpcMiniServer::CloseRcvrSocket()
{
    if (INVALID_SOCKET != this->rcvrSocket)
    {
        shutdown(this->rcvrSocket, 2);
        closesocket(this->rcvrSocket);
        this->rcvrSocket = INVALID_SOCKET;
    }
}

//------------------------------------------------------------------------------
/**
    Waits for a client to connect and accepts the connection.
*/
bool nIpcMiniServer::Listen()
{
    bool retval = false;

    // wait for a client...
    if (listen(this->ipcServer->sock, 5) != -1)
    {
        this->rcvrSocket = accept(this->ipcServer->sock, 0, 0);
        if (INVALID_SOCKET != this->rcvrSocket)
        {
            n_printf("client %d: connection accepted, socket %d.\n", this->clientId, this->rcvrSocket);

            // put the socket into nonblocking mode
            #if defined(__WIN32__)
                u_long trueAsUlong = 1;
                ioctlsocket(this->rcvrSocket, FIONBIO, &trueAsUlong);
            #elif defined(__LINUX__) || defined(__MACOSX__)
                int flags;
                flags = fcntl(this->rcvrSocket, F_GETFL);
                flags |= O_NONBLOCK;
                fcntl(this->rcvrSocket, F_SETFL, flags);
            #endif
            retval = true;

            // set the connection status to false, this will only be set to true
            // when the actual handshake with the client has happened
            //this->isConnected = false;
        }
        else
        {
            n_printf("nIpcMiniServer::Listen(): accept() failed!");
        }
    }
    else
    {
        n_printf("nIpcMiniServer::Listen(): listen() failed!");
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    This method should be called after Listen() if the connection should
    be ignored for any reason.
*/
void
nIpcMiniServer::Ignore()
{
    this->CloseRcvrSocket();
}

//------------------------------------------------------------------------------
/**
    Checks if an incoming message is available, if yes, reads the message
    and adds a new nMsgNode to the parent nIpcServer's message list.

    If no message is pending, just do nothing.

    Handles handshakes and close requests internally.

    @return     false if socket has been closed
*/
bool
nIpcMiniServer::Poll()
{
    if (INVALID_SOCKET != this->rcvrSocket)
    {
        // do a non-blocking recv
        int len = recv(this->rcvrSocket, this->msgBuffer.GetPointer(), this->msgBuffer.GetMaxSize(), 0);
        if (len == 0 || N_SOCKET_LAST_ERROR == N_ECONNRESET)
        {
            // the connection has been closed
            n_printf("nIpcMiniServer: connection closed!\n");
            this->isConnected = false;
            this->CloseRcvrSocket();
            return false;
        }
        else if (len < 0)
        {
            // the receive call would block (actually, this is not quite
            // clean, since WSAEWOULDBLOCK is just one possibility of
            // many)
            return true;
        }
        else
        {
            // normal case: a message was received!
            this->msgBuffer.SetSize(len);

            // split multi-string receives
            const char* curString = msgBuffer.GetFirstString();
            if (curString) do
            {
                bool systemMessage = false;
                nString tokenString = curString;
                const char* cmd = tokenString.GetFirstToken(" ");
                if (cmd)
                {
                    if (strcmp(cmd, "~handshake") == 0)
                    {
                        // handshake from client, one portname argument
                        const char* portName = tokenString.GetNextToken(" ");
                        //if (portName && (0 == strcmp(portName, this->ipcServer->selfAddr.GetPortName())))
                        {
                            this->isConnected = true;
                        }
                        /*
                        else
                        {
                            this->isConnected = false;
                        }*/
                        systemMessage = true;
                    }
                    else if (strcmp(cmd, "~close") == 0)
                    {
                        // client going to close connection
                        this->isConnected = false;
                        systemMessage = true;
                    }
                }

                // if not a system message, add to user message list
                if (!systemMessage)
                {
                    // a user message, add to msg list of thread
                    nMsgNode *msgNode = n_new(nMsgNode((void*) curString, strlen(curString) + 1));
                    msgNode->SetPtr((void*) this->clientId);
                    this->ipcServer->msgList.Lock();
                    this->ipcServer->msgList.AddTail(msgNode);
                    this->ipcServer->msgList.Unlock();
                    this->ipcServer->msgList.SignalEvent();
                }
            } while ((curString = msgBuffer.GetNextString()));
        }
        return this->isConnected;
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nIpcMiniServer::Send(const nIpcBuffer& msg)
{
    if ((INVALID_SOCKET != this->rcvrSocket) && (this->isConnected))
    {
        int res = send(this->rcvrSocket, msg.GetPointer(), msg.GetSize(), 0);
        if ((res == SOCKET_ERROR) || (res != msg.GetSize()))
        {
            n_printf("nIpcMiniServer::Send() failed!\n");
        }
        return (res != SOCKET_ERROR);
    }
    return true;
}
