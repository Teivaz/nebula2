#ifndef N_IPCCLIENT_H
#define N_IPCCLIENT_H
//-----------------------------------------------------------------------------=
/**
    @class nIpcClient
    @ingroup Ipc
    @brief The client side of a network connection.

    A nIpcClient object can connect to exactly one nIpcServer
    object over a socket and send messages to it, or receive messages
    from it.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nsocketdefs.h"
#include "kernel/nipcaddress.h"
#include "kernel/nipcbuffer.h"

//------------------------------------------------------------------------------
class nIpcClient
{
public:
    /// connection flags
    enum
    {
        Blocking = (1<<0),
    };

    /// constructor
    nIpcClient();
    /// destructor
    ~nIpcClient();
    /// set the blocking behaviour
    void SetBlocking(bool b);
    /// get the blocking behaviour
    bool GetBlocking() const;
    /// connect to an ipc server
    bool Connect(nIpcAddress& addr);
    /// disconnect from ipc server
    void Disconnect();
    /// return true if currently connected
    bool IsConnected() const;
    /// send a message to the server
    bool Send(const nIpcBuffer& msg);
    /// receive a message from the server (optionally blocks)
    bool Receive(nIpcBuffer& msg);

private:
    /// destroy internal socket
    void DestroySocket();
    /// change blocking mode on socket internally
    void ApplyBlocking(bool b);

    SOCKET sock;
    nIpcAddress serverAddr;
    bool blocking;
    bool isConnected;
};

//------------------------------------------------------------------------------
#endif
