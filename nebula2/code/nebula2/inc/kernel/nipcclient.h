#ifndef N_IPCCLIENT_H
#define N_IPCCLIENT_H
//-----------------------------------------------------------------------------=
/**
    A nIpcClient object can connect to exactly one nIpcServer
    object over a socket and send messages to it. The client
    generally expects a reply from the server befor a new
    message can be sent. 
    The server is identified through an [optional] tcp/ip
    address and a port name, which is magically converted
    into a port number.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_MSGNODE_H
#include "util/nmsgnode.h"
#endif

#ifndef N_SOCKETDEFS_H
#include "kernel/nsocketdefs.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#ifndef N_HASHTABLE_H
#include "util/nhashtable.h"
#endif

#if __XBxX__
#   include "xbox/nxbwrapper.h"
#elif defined(__WIN32__)
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

//------------------------------------------------------------------------------
class nIpcClient 
{
public:
    /// constructor
    nIpcClient();
    /// destructor
    ~nIpcClient();
    /// connect to an ipc server
    bool Connect(const char* portName);
    /// disconnect
    void Disconnect();
    /// send a message to the server, and return answer
    nMsgNode* SendMsg(void* buf, int size);
    /// release answer message node
    void FreeReplyMsgNode(nMsgNode* nd);
    /// get hostname part
    const char* GetHostName() const;
    /// get portname part 
    const char* GetPortName() const;

private:
    /// initialize the server address field
    bool FillServerAddr(const char* pname);
    /// get port number from port name
    short GetPortNumFromName(const char* portName);

    enum
    {
        RECEIVEBUFFERSIZE = 2048,
    };
    SOCKET sock;   
    sockaddr_in serverAddr;
    short serverPortNum;
    nString serverHostName;
    nString serverPortName;
    char receiveBuffer[RECEIVEBUFFERSIZE];
};

//------------------------------------------------------------------------------
#endif
