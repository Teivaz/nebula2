#ifndef N_IPCSERVER_H
#define N_IPCSERVER_H
//------------------------------------------------------------------------------
/**
    @class nIpcServer

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
  
        - 192.168.0.90:nebula
        - nomad:testport
        - localhost:nebula
  
    The hostaddress field can be empty if the port is on the localhost.
  
    The portname will be converted into an ordinary port number 
    by hashing the port name string.
  
    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nthreadsafelist.h"
#include "util/nmsgnode.h"
#include "kernel/nmutex.h"
#include "kernel/nsocketdefs.h"
#include "util/nstring.h"

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
