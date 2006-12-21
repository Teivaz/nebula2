#ifndef N_REMOTESERVER_H
#define N_REMOTESERVER_H
//------------------------------------------------------------------------------
/**
    @class nRemoteServer
    @ingroup Ipc
    @brief A network-accessible console server.

    The remote server opens a socket and listens for connecting
    clients. For each client, a remote session will be created,
    each with its own current working object.

    All strings coming from the remote client will be send to the
    script server for evaluation. All strings emitted by
    the Nebula kernel will be sent back to the client.

    Please check out the @ref NebulaToolsnremoteshell tool, a
    command line tool which can talk to an nRemoteServer.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nipcserver.h"
#include "kernel/nautoref.h"

//------------------------------------------------------------------------------
class nScriptServer;
class nRemoteServer : public nRoot
{
public:
    /// constructor
    nRemoteServer();
    /// destructor
    virtual ~nRemoteServer();
    /// return pointer to server instance
    static nRemoteServer* Instance();
    /// open a named communication port
    bool Open(const char* portName);
    /// close the communication port
    void Close();
    /// return true if open
    bool IsOpen() const;
    /// send a string to all connected clients
    void Broadcast(const char* str);
    /// process pending messages, call this method frequently
    bool Trigger();

private:
    class nClientContext : public nNode
    {
        friend class nRemoteServer;

        /// constructor
        nClientContext(int cid, nRemoteServer* owner);
        /// destructor
        ~nClientContext();
        /// get client id
        int GetClientId() const;
        /// set cwd object
        void SetCwd(nRoot* obj);
        /// get cwd object
        nRoot* GetCwd() const;

        int clientId;
        nRef<nRoot> refCwd;
    };

    /// get client context for client id
    nClientContext* GetClientContext(int clientId);
    /// get cwd for a client id, create new client context if not exists yet
    nRoot* GetClientCwd(int clientId);
    /// set cwd in client context
    void SetClientCwd(int clientId, nRoot* cwd);

    static nRemoteServer* Singleton;

    nAutoRef<nScriptServer> refScriptServer;
    bool isOpen;
    nIpcServer* ipcServer;
    nList clientContexts;
};

//------------------------------------------------------------------------------
/**
*/
inline
nRemoteServer*
nRemoteServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRemoteServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRemoteServer::nClientContext::nClientContext(int cid, nRemoteServer* /*owner*/) :
    clientId(cid)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nRemoteServer::nClientContext::~nClientContext()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRemoteServer::nClientContext::GetClientId() const
{
    return this->clientId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRemoteServer::nClientContext::SetCwd(nRoot* obj)
{
    n_assert(obj);
    this->refCwd = obj;
}

//------------------------------------------------------------------------------
/**
    Get the client specific cwd object. If the object no longer exists,
    return 0.
*/
inline
nRoot*
nRemoteServer::nClientContext::GetCwd() const
{
    return this->refCwd.isvalid() ? this->refCwd.get() : 0;
}

//------------------------------------------------------------------------------
#endif

