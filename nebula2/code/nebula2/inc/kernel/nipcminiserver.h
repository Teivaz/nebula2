#ifndef N_IPCMINISERVER_H
#define N_IPCMINISERVER_H
//------------------------------------------------------------------------------
/**
    @class nIpcMiniServer
    @ingroup Ipc
    @brief The per-connection state for the network server.

    A nIpcServer creates one nIpcMiniServer for each connecting client.

    (C) 2002 RadonLabs GmbH
*/
#include "util/nnode.h"

//------------------------------------------------------------------------------
class nIpcServer;
class nThread;
class nIpcMiniServer : public nNode 
{
public:
    /// constructor
    nIpcMiniServer(nIpcServer* server);
    /// destructor
    ~nIpcMiniServer();
    /// listen on server socket and wait for new client
    bool Listen();
    /// ignore the new client for any reason
    void Ignore();
    /// check for and pull incoming messages, call this frequently!
    bool Poll();
    /// send a message to the client
    bool Send(const nIpcBuffer& msg);
    /// get the client id
    int GetClientId() const;
    /// return connection status
    bool IsConnected() const;

private:
    friend class nIpcServer;

    /// close the internal receiver socket
    void CloseRcvrSocket();
   
    nIpcServer* ipcServer;    // the master server
    int clientId;                                      
    SOCKET rcvrSocket;
    nIpcBuffer msgBuffer;
    bool isConnected;         // valid connection established (including handshake)
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nIpcMiniServer::GetClientId() const
{
    return this->clientId;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nIpcMiniServer::IsConnected() const
{
    return this->isConnected;
}

//------------------------------------------------------------------------------
#endif
