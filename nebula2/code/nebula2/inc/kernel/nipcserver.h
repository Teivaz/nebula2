#ifndef N_IPCSERVER_H
#define N_IPCSERVER_H
//------------------------------------------------------------------------------
/**
    Sockets based server object for simple ipc 

    An nIpcServer object opens a named public message port, and
    waits for connection requests from nIpcClient objects. One
    nIpcServer can handle any number of nIpcClients.
  
    The current implementation uses AF_INET sockets for any type
    of connection (even if the client is on the local host). 
    The main reason is that Win32 sockets only implement AF_INET.
    
    An nIpcServer address has 2 parts, an optional TCP/IP address
    and the port name:
  
        [hostaddress:]portname
  
    For instance:
  
        192.168.0.90:nebula
        nomad:testport
        localhost:nebula
  
    The hostaddress field can be empty if the port is on the localhost.
  
    The portname will be converted into an ordinary port number 
    by hashing the port name string.
  
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_THREADSAFELIST_H
#include "kernel/nthreadsafelist.h"
#endif

#ifndef N_MSGNODE_H
#include "util/nmsgnode.h"
#endif

#ifndef N_MUTEX_H
#include "kernel/nmutex.h"
#endif

#ifndef N_SOCKETDEFS_H
#include "kernel/nsocketdefs.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#if __XBxX__
#   include "xbox/nxbwrapper.h"
#elif __WIN32__
#   ifndef _INC_WINDOWS
#   include <windows.h> 
#   endif
#   ifndef _WINSOCKAPI_
#   include <winsock.h>
#   endif
#else
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <unistd.h>
#   include <netdb.h>
#endif

#undef N_DEFINES
#define N_DEFINES nIpcServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nThread;
class nIpcServer 
{
public:
    /// constructor
    nIpcServer(const char* portName);
    /// destructor
    ~nIpcServer();
    /// poll connections for new messages, call this frequently!
    bool Poll();
    /// get next message
    nMsgNode* GetMsg(int& outClientId);
    /// release message
    void ReplyMsg(nMsgNode*);
    /// send message back to a client
    bool AnswerMsg(void* buf, int size, int clientId);
    /// broadcast message to all clients
    void BroadcastMsg(void* buf, int size);
    /// compute a port number from a name
    short GetPortNumFromName(const char* portName);

    nThread *listenerThread;
    int numMiniServers;
    nThreadSafeList miniServerList;
    nThreadSafeList msgList;
    nString pname;
    struct sockaddr_in hostAddr;
    SOCKET sock;
};
//--------------------------------------------------------------------
#endif
