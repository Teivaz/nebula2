#ifndef N_IPCSERVER_H
#define N_IPCSERVER_H
//------------------------------------------------------------------------------
/**
    @class nIpcServer
    @ingroup Ipc
    @brief Sockets based server object for simple inter-process communication.

    An nIpcServer object opens a named public message port, and
    waits for connection requests from nIpcClient objects. One
    nIpcServer can handle any number of nIpcClients.

    (C) 2002 RadonLabs GmbH
*/

#include "kernel/ntypes.h"
#include "kernel/nthreadsafelist.h"
#include "kernel/nsocketdefs.h"
#include "kernel/nipcaddress.h"
#include "kernel/nipcbuffer.h"

//------------------------------------------------------------------------------
class nThread;
class nIpcServer
{
public:
    /// constructor
    nIpcServer(nIpcAddress& selfAddr);
    /// destructor
    ~nIpcServer();
    /// poll the ipc server, call this frequently!
    bool Poll();
    /// receive a message from a client, will never block!
    bool GetMsg(nIpcBuffer& msg, int& fromClientId);
    /// send a message to a client
    bool Send(int toClientId, const nIpcBuffer& msg);
    /// send a message to all clients
    bool SendAll(const nIpcBuffer& msg);

    nIpcAddress selfAddr;
    nThread *listenerThread;
    int uniqueMiniServerId;
    nThreadSafeList miniServerList;
    nThreadSafeList msgList;
    nArray<int> ClientsReseted;
    SOCKET sock;
};
//--------------------------------------------------------------------
#endif
