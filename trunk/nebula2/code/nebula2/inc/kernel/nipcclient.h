#ifndef N_IPCCLIENT_H
#define N_IPCCLIENT_H
//-----------------------------------------------------------------------------=
/**
    @class nIpcClient

    A nIpcClient object can connect to exactly one nIpcServer
    object over a socket and send messages to it. The client
    generally expects a reply from the server befor a new
    message can be sent. 
    The server is identified through an [optional] tcp/ip
    address and a port name, which is magically converted
    into a port number.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nlist.h"
#include "util/nmsgnode.h"
#include "kernel/nsocketdefs.h"
#include "util/nstring.h"
#include "util/nhashtable.h"

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
