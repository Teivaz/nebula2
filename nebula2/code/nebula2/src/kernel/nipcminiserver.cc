#define N_IMPLEMENTS nIpcMiniServer
#define N_KERNEL
//------------------------------------------------------------------------------
//  nipcminiserver.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#ifdef __LINUX__
#include "fcntl.h"
#endif

#include "kernel/nipcserver.h"
#include "kernel/nthread.h"
#include "kernel/nipcminiserver.h"

//------------------------------------------------------------------------------
/**
    Close the receiver socket.
*/
void
nIpcMiniServer::CloseRcvrSocket()
{
    if (this->rcvrSocket) 
    {
        shutdown(this->rcvrSocket, 2);
#if defined(__WIN32__)
        closesocket(this->rcvrSocket);
#elif defined(__LINUX__)
        close(this->rcvrSocket);
#endif
        this->rcvrSocket = 0;
    }
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
    bool connected = false;

    if (this->rcvrSocket)
    {
        connected = true;

        // do a non-blocking recv
        int len = recv(this->rcvrSocket, this->rcvrBuf, sizeof(this->rcvrBuf), 0);
        if (len == 0)
        {
            // the connection has been closed
            n_printf("nIpcMiniServer: connection closed!\n");
            connected = false;
            this->CloseRcvrSocket();
        }
        else if (len < 0)
        {
            // an error, filter out the would block error
            #ifdef __WIN32__
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                {
                    // no data pending, return immediately
                    return true;
                }
                else
                {
                    // a real error, close socket!
                    this->CloseRcvrSocket();
                    connected = false;
                }
            #elif defined(__LINUX__)
                if (errno == EWOULDBLOCK)
                {
                    // no data pending, return immediately
                    return true;
                }
                else
                {
                    // a real error, close socket!
                    this->CloseRcvrSocket();
                    connected = false;
                }
            #else
            #error IMPLEMENT ME
            #endif
        }
        else
        {
            // normal case: a message was received!

            // check for special case command
            if (this->rcvrBuf[0] == '~') 
            {
                char *result = 0;
                char *cmd = strtok(this->rcvrBuf, " \t~");
                if (strcmp(cmd, "handshake") == 0) 
                {
                    // client checks for right portname
                    char *pname = strtok(NULL," \t~");
                    if (pname) 
                    {
                        if (strcmp(pname, this->server->pname.Get()) == 0) 
                        {
                            result = "~true";
                        }
                        else 
                        {
                            // ooops, wrong port, shut down connection
                            result      = "~false";
                            connected = false;
                        }
                    } 
                    else 
                    {
                        n_error("nIpcMiniServer::n_miniserver_tfunc(): ~handshake command broken!");
                    }
                } 
                else if (strcmp(cmd, "close") == 0) 
                {
                    result    = "~ok";
                    connected = false;
                } 
                else
                {
                    // an unknown command, just ignore
                }

                // return answer to special case command
                if (result) 
                {
                    send(this->rcvrSocket, result, strlen(result) + 1, 0);
                }
            } 
            else 
            {
                // normal message, add to msg list and emit signal
                nMsgNode *nd = n_new nMsgNode(this->rcvrBuf, len);
                nd->SetPtr((void *)this->id);
                this->server->msgList.Lock();
                this->server->msgList.AddTail(nd);
                this->server->msgList.Unlock();
                this->server->msgList.SignalEvent();
            }
        }
    }
    return connected;
}

//------------------------------------------------------------------------------
/**
     - 28-Oct-98   floh    created
     - 28-May-99   floh    + memleak fixed if nobody connected to the
                             miniserver
*/
nIpcMiniServer::nIpcMiniServer(nIpcServer *_server)
{
    n_assert(_server);

    this->server = _server;
    this->id     = this->server->numMiniServers++;
    this->srvrSocket = _server->sock;
    this->rcvrSocket = 0;
    memset(&(this->clientAddr), 0, sizeof(this->clientAddr));

    this->server->miniServerList.Lock();
    this->server->miniServerList.AddTail(this);
    this->server->miniServerList.Unlock();
}

//------------------------------------------------------------------------------
/**
     - 28-Oct-98   floh    created
*/
nIpcMiniServer::~nIpcMiniServer()
{
    this->CloseRcvrSocket();
}

//------------------------------------------------------------------------------
/**
     - 28-Oct-98   floh    created
     - 31-Oct-98   floh    divided into Listen(), Accept() und Ignore()
*/
bool nIpcMiniServer::Listen()
{
#ifdef __GLIBC__
    unsigned int addrLen;
#else
    int addrLen;
#endif
    bool retval = false;
    
    // wait for a client...
    if (listen(this->srvrSocket, 5) != -1) 
    {
        addrLen = sizeof(this->clientAddr);

        this->rcvrSocket = accept(this->srvrSocket, (struct sockaddr *)&(this->clientAddr), &addrLen);

        if (this->rcvrSocket != INVALID_SOCKET) 
        {
            n_printf("client %d: connection accepted.\n", (int) this);

            // put the socket into nonblocking mode
#ifdef __WIN32__
            u_long argp = 1;
            ioctlsocket(this->rcvrSocket, FIONBIO, &argp);
#elif defined(__LINUX__)
            int flags;
            flags = fcntl(this->rcvrSocket, F_GETFL);
            flags |= O_NONBLOCK;
            fcntl(this->rcvrSocket, F_SETFL, flags);
#endif
 
            retval = true;
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
     - 31-Oct-98   floh    created
*/
void 
nIpcMiniServer::Ignore()
{
    this->CloseRcvrSocket();
}

//------------------------------------------------------------------------------
/**
     - 28-Oct-98   floh    created
*/
bool 
nIpcMiniServer::Send(void *buf, int size)
{
    n_assert(buf);
    n_assert(size > 0);
    
    if (this->rcvrSocket) 
    {
        int res = 0;
        res = send(this->rcvrSocket, (char *)buf, size, 0);
        if (-1 == res)
        {
            n_printf("nIpcMiniServer::Send(): send() failed!\n");
        }
    }
    return true;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
