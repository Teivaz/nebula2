#ifndef N_SESSIONSERVER_H
#define N_SESSIONSERVER_H
//------------------------------------------------------------------------------
/**
    @class nSessionServer
    @ingroup Network

    A session server manages all necessary information about a network
    session, and eventually configures and opens the actual game network server.

    Session info broadcasting
    =========================
    An open session server periodically broadcasts its presence
    into the LAN approx once per second. The broadcast is sent on 
    the nIpcAddress "self:BroadcastAPPNAME", where APPNAME is the 
    application name set via nSessionServer::SetAppName(). The message 
    is a normal string of the following format:

    "~session Guid AppName AppVersion IpAddress IpPort"

    The guid is a unique session id understood by the Nebula nGuid
    class. Appname and Appversion are the respective application
    id strings set by nSessionServer::SetAppName() and 
    nSessionServer::SetAppVersion(). Potential clients which
    want to know more about the session should initiate a
    normal nIpcServer/nIpcClient connection to the session server
    using the IpAddress and IpPort fields of the session broadcast message.

    Protocol spec:
    ==============
    Once a potential client has found out about a session server by
    listening on the above broadcast message it may establish a 
    nIpcServer/nIpcClient connection to get a reliable communication 
    channel with the server and implement the following protocol:

    ---
    client: "~queryserverattrs"
    server: ---

    This message is sent by a new client to obtain a copy of the current
    server attributes. The server will reply with "~serverattr" messages
    to that client (see below).

    ---
    server: "~serverattr AttrName [AttrValue]"
    client: ---

    This message is sent by the server to all connected clients
    as soon as a refresh of one or more server attributes is necessary. 
    Server attributes are Name/Value string pairs which contain info such 
    as current number of players, max number of players, etc... It 
    is totally up to the application which server attributes it defines.
    The server does not expect an answer from the clients.

    ---
    server: "~closesession"
    client: ---

    This message is sent by the server to all connected clients when
    the session is going to be closed/cancelled. This means the session
    will no longer exists. If a game is started from the session, all
    connected non-joined clients will receive a "~closesession" message,
    while all joined clients will receive a "~start" message from the
    server.

    ---
    server: "~start IpAddress IpPort"
    client: ---

    This message is sent by the server to all connected clients when
    the game session is about to start. The provided IpcAddress is
    the address of the GameServer which the client should contact
    right after receiving the "~start" message from the session
    server. The session server itself will stop its duty after
    the "~start" message has been sent, as the GameServer will
    take over from here on.

    ---
    client: "~joinsession clientGuid"
    server: "~joinaccepted" OR
            "~joindenied"

    This message is sent by a client when it wants to join a session.
    The server responds either with a "~joinaccepted clientName"
    message, where clientName is a string which identifies the client
    for later communication, or a "~joindenied" message, the most
    common reason for "~joindenied" is that the session was full.

    ---
    client: "~clientattr clientGuid AttrName [AttrValue]"
    server: ---

    This message is sent by joined clients to the server when a client attribute
    needs a refresh, or right after a session is joined. Client attributes
    contain client specific data like PlayerName, PlayerColor. It is totally
    up to the application which data is defined as client attribute.

    The server will convert client attributes to server attributes, which then
    get automatically distributed to all clients. The server will prefix
    the client attribute's name with a ClientX_ prefix, where X is a
    number from 0 to (MaxClients - 1).
    
    ---
    server: "~kick"
    client: ---

    This message is sent by the server to a joined client which has
    been kicked from the session by the host player. The client
    should simply reply with a ~leavesession clientGuid" message.
    
    ---
    client: "~leavesession clientGuid"
    server: ---

    This message is sent by a joined client to the server when it
    wants to leave the session. The client will still receive
    "~serverattr" messages from the server, but no longer join-specific
    messages.

    ---
    NOT YET IMPLEMENTED

    server: "~ping clientGuid"
    client: "~pong clientGuid"

    This is a keepalive message which the server sends to every joined
    client every few seconds just to keep the connection alive. The
    client should respond with a "~pong clientId" message. If no messages
    arrive for at least 10 seconds on the either the server or the
    client side, the connection should be considered dead by both sides.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nipcserver.h"
#include "kernel/nipcpeer.h"
#include "kernel/nautoref.h"
#include "kernel/nguid.h"
#include "network/nsessionattrpool.h"

class nSessionClientContext;
class nNetServer;
class nNetClient;

//------------------------------------------------------------------------------
class nSessionServer : public nRoot
{
public:
    /// constructor
    nSessionServer();
    /// destructor
    virtual ~nSessionServer();
    /// set application name
    void SetAppName(const char* name);
    /// get application name
    const char* GetAppName() const;
    /// set application version string
    void SetAppVersion(const char* version);
    /// get application version string
    const char* GetAppVersion() const;
    /// set max number of clients in session
    void SetMaxNumClients(int num);
    /// get max number of clients in session
    int GetMaxNumClients() const;
    /// set a session attribute
    void SetServerAttr(const char* name, const char* val);
    /// get a session attribute by name
    const char* GetServerAttr(const char* name);
    /// get number of session attributes
    int GetNumServerAttrs() const;
    /// get session attribute by index
    void GetServerAttrAt(int index, const char*& name, const char*& value);
    /// open the session (makes session visible in network)
    bool Open();
    /// close the session (makes session invisible in network)
    void Close();
    /// return true if session open
    bool IsOpen() const;
    /// set current time
    void SetTime(double t);
    /// get current time
    double GetTime() const;
    /// process pending messages
    void Trigger();
    /// get number of joined clients
    int GetNumClients() const;
    /// get pointer to a client context which provides client information
    nSessionClientContext* GetClientAt(int index) const;
    /// kick a specific client (this will invalidate numClients and client indices!)
    bool KickClient(int clientIndex);
    /// send the start signal to all clients
    bool Start();

private:
    /// private enums
    enum
    {
        AllClients = -1,
    };

    /// broadcast a "I'm here" into the LAN
    void BroadcastIdentity();
    /// send server attributes to connected clients
    void SendServerAttrs(int clientId);
    /// fiend a client context by guid
    nSessionClientContext* FindClientContextByGuid(const char* guid);
    /// handle a join session request
    void HandleJoinSessionRequest(int ipcClientId, const char* clientGuid);
    /// handle a leave session request
    void HandleLeaveSessionRequest(int ipcClientId, const char* clientGuid);
    /// handle a client attribute message
    void HandleClientAttribute(int ipcClientId, const char* clientGuid, const char* attrName, const char* attrValue);
    /// update the NumPlayers attribute
    void UpdateNumPlayersAttr();
    /// convert client attributes into server attributes
    void UpdateServerClientAttrs();
    /// updates the MaxNumPlayers server attribute
    void UpdateMaxNumPlayersAttr();

    nAutoRef<nNetServer> refNetServer;
    nAutoRef<nNetClient> refNetClient;
    nRef<nRoot> refClientContexts;
    nGuid sessionGuid;
    nString appName;
    nString appVersion;
    nSessionAttrPool serverAttrs;
    int maxNumClients;
    bool isOpen;
    nIpcAddress sessionIpcAddress;
    nIpcAddress broadcastIpcAddress;
    nIpcPeer* ipcBroadcaster;
    nIpcServer* ipcServer;
    double time;
    double broadcastTimeStamp;
    bool serverAttrsDirty;
    int uniqueNumber;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionServer::SetAppName(const char* name)
{
    n_assert(name);
    this->appName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSessionServer::GetAppName() const
{
    return this->appName.IsEmpty() ? 0 : this->appName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionServer::SetAppVersion(const char* version)
{
    n_assert(version);
    this->appVersion = version;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSessionServer::GetAppVersion() const
{
    return this->appVersion.IsEmpty() ? 0 : this->appVersion.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionServer::SetMaxNumClients(int num)
{
    n_assert(num > 0);
    this->maxNumClients = num;

    // update the MaxNumPlayers server attribute
    this->UpdateMaxNumPlayersAttr();
    this->UpdateNumPlayersAttr();
    this->UpdateServerClientAttrs();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSessionServer::GetMaxNumClients() const
{
    return this->maxNumClients;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSessionServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionServer::SetTime(double t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
double
nSessionServer::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
#endif