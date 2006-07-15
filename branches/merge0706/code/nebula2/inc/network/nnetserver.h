#ifndef N_NETSERVER_H
#define N_NETSERVER_H
//------------------------------------------------------------------------------
/**
    @class nNetServer
    @ingroup Network
    @brief A server in a multiplayer session. Should be subclasses for specific
    applications or protocols.

    After the session server has collected information about the network clients
    and the player has hit the start button, the session server should
    configure the net server object with the clients which are expected to 
    join (with "localclient" being a special shortcut connection to the
    local nnetclient object). After the configuration the game server should
    be opened, after which the server will wait for the clients to connect.
    Once all clients are connected, the IsStarted() flag will turn true, and
    communication can begin.

    The net server is a simple message distributor. It either sends messages
    to all clients, or replicates messages from one client to all other clients.
    Specific communication protocols and behaviour should be implemented in 
    subclasses.

    This is the net client/server protocol:

@verbatim
    ---
    client: "~joinsession clientGuid"
    server: "~joinaccepted" OR "~joindenied"
@endverbatim

    A client requestes to join the server. A client must send a guid
    to the server for authentification. This must be one of the 
    guids set by nNetServer::SetClientGuid(). This guid is usually
    the result of the session setup process. The server answers with
    a <tt>~joinaccepted</tt> or <tt>~joindenied</tt>.

@verbatim
    ---
    server: "~start"
    client: ---
@endverbatim

    This message is sent by the server once to all clients, when all
    clients have connected.

@verbatim
    ---
    server: "~closesession"
    client: ---
@endverbatim

    This is sent by the server to all clients when the session is
    ended by the host. Only the host may close the session. No
    reply is expected from the clients.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nguid.h"
#include "kernel/nipcserver.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nNetServer : public nRoot
{
public:
    /// client status enum
    enum ClientStatus
    {
        Invalid,
        Waiting,
        Connected,
        Timeout,
    };

    /// constructor
    nNetServer();
    /// destructor
    virtual ~nNetServer();
    /// set the communication port name
    void SetPortName(const char* name);
    /// get the communication port name
    const char* GetPortName() const;
    /// begin configuring client info
    void BeginClients(int numClients);
    /// set client guid
    void SetClientGuid(int clientIndex, const char* clientGuid);
    /// set client player name
    void SetClientPlayerName(int clientIndex, const char* name);
    /// finish configuring client info
    void EndClients();
    /// get number of clients
    int GetNumClients() const;
    /// get guid of client at index
    const char* GetClientGuidAt(int index) const;
    /// get client player status at index
    const char* GetClientPlayerNameAt(int index) const;
    /// get client connection status
    ClientStatus GetClientStatusAt(int index) const;
    /// open the network session
    bool Open();
    /// close the network session
    void Close();
    /// per-frame-trigger
    void Trigger();
    /// return true if server is open
    bool IsOpen() const;
    /// return true if session is started (all clients have connected)
    bool IsStarted() const;

protected:
    /// handle a custom message (handled by subclass)
    virtual bool HandleMessage(int fromClientId, const char* msg);
    /// perform protocol specific OnStart() action
    virtual void OnStart();
    /// handle a join session request from a client
    void HandleJoinSessionRequest(int clientId, const char* clientGuid);

    class ClientContext
    {
    public:
        /// constructor
        ClientContext();
        /// constructor with guid
        ClientContext(const nGuid& guid);
        /// destructor
        ~ClientContext();
        /// set guid
        void SetClientGuid(const nGuid& guid);
        /// get guid
        const nGuid& GetClientGuid() const;
        /// set ipc client id
        void SetIpcClientId(int id);
        /// get ipc client id
        int GetIpcClientId() const;
        /// set client status
        void SetClientStatus(ClientStatus status);
        /// get client status
        ClientStatus GetClientStatus() const;
        /// set player name
        void SetPlayerName(const char* name);
        /// get player name
        const char* GetPlayerName() const;

    private:
        nGuid clientGuid;
        int ipcClientId;
        ClientStatus clientStatus;
        nString playerName;
    };

    nString portName;
    nArray<ClientContext> clientArray;
    bool isOpen;
    bool isStarted;
    nIpcServer* ipcServer;
};

//------------------------------------------------------------------------------
/**
*/
inline
nNetServer::ClientContext::ClientContext() :
    ipcClientId(-1),
    clientStatus(Invalid)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nNetServer::ClientContext::ClientContext(const nGuid& guid) :
    clientGuid(guid),
    ipcClientId(-1),
    clientStatus(Invalid)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nNetServer::ClientContext::~ClientContext()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetServer::ClientContext::SetClientGuid(const nGuid& guid)
{
    this->clientGuid = guid;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nGuid&
nNetServer::ClientContext::GetClientGuid() const
{
    return this->clientGuid;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetServer::ClientContext::SetPlayerName(const char* name)
{
    n_assert(name);
    this->playerName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nNetServer::ClientContext::GetPlayerName() const
{
    return this->playerName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetServer::ClientContext::SetIpcClientId(int id)
{
    this->ipcClientId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nNetServer::ClientContext::GetIpcClientId() const
{
    return this->ipcClientId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetServer::ClientContext::SetClientStatus(ClientStatus status)
{
    this->clientStatus = status;
}

//------------------------------------------------------------------------------
/**
*/
inline
nNetServer::ClientStatus
nNetServer::ClientContext::GetClientStatus() const
{
    return this->clientStatus;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetServer::SetPortName(const char* name)
{
    n_assert(name);
    this->portName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nNetServer::GetPortName() const
{
    return this->portName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetServer::BeginClients(int numClients)
{
    n_assert(!this->IsOpen());
    this->clientArray.SetFixedSize(numClients);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetServer::SetClientGuid(int clientIndex, const char* clientGuid)
{
    n_assert(clientGuid);
    nGuid newClientGuid(clientGuid);
    this->clientArray[clientIndex].SetClientGuid(newClientGuid);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetServer::SetClientPlayerName(int clientIndex, const char* name)
{
    n_assert(name);
    this->clientArray[clientIndex].SetPlayerName(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetServer::EndClients()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nNetServer::GetNumClients() const
{
    return this->clientArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nNetServer::GetClientGuidAt(int index) const
{
    return this->clientArray[index].GetClientGuid().Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char* 
nNetServer::GetClientPlayerNameAt(int index) const
{
    return this->clientArray[index].GetPlayerName();
}

//------------------------------------------------------------------------------
/**
*/
inline
nNetServer::ClientStatus
nNetServer::GetClientStatusAt(int index) const
{
    return this->clientArray[index].GetClientStatus();
}

//------------------------------------------------------------------------------
/**
    Return true if net server is open.
*/
inline
bool
nNetServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Return true if all clients have connected.
*/
inline
bool
nNetServer::IsStarted() const
{
    bool isStarted = true;
    int numClients = this->clientArray.Size();
    int clientIndex = 0;
    for (clientIndex = 0; clientIndex < numClients; clientIndex++)
    {
        isStarted &= (this->clientArray[clientIndex].GetClientStatus() == Connected);
    }
    return isStarted;
}

//------------------------------------------------------------------------------
#endif

