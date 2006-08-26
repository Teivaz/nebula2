#ifndef N_SESSIONCLIENT_H
#define N_SESSIONCLIENT_H
//------------------------------------------------------------------------------
/**
    @class nSessionClient
    @ingroup Network

    @brief  A session client object can discover open sessions in a LAN,
    join an open session and get information about the session from
    the session server.

    When the session is started by the session server, the session client
    will configure and open the local game client object.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "kernel/nipcaddress.h"
#include "kernel/nipcpeer.h"
#include "kernel/nguid.h"
#include "network/nsessionattrpool.h"

class nSessionServerContext;

//------------------------------------------------------------------------------
class nSessionClient : public nRoot
{
public:
    /// constructor
    nSessionClient();
    /// destructor
    virtual ~nSessionClient();
    /// set application name (must match with session server)
    void SetAppName(const char* name);
    /// get application name
    const char* GetAppName() const;
    /// set application version string (must match with session server)
    void SetAppVersion(const char* version);
    /// get application version string
    const char* GetAppVersion() const;
    /// set the current time
    void SetTime(double time);
    /// get the current time
    double GetTime() const;
    /// open the session client, will start discovering sessions
    bool Open();
    /// close the client, will stop discovering sessions
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// trigger the client object, will discover servers and update the server list
    void Trigger();
    /// set a client attribute
    void SetClientAttr(const char* name, const char* val);
    /// get a client attribute by name
    const char* GetClientAttr(const char* name);
    /// get number of client attributes
    int GetNumClientAttrs() const;
    /// get client attribute at index
    void GetClientAttrAt(int index, const char*& name, const char*& value);
    /// get number of discovered servers
    int GetNumServers() const;
    /// get server context object which provides information about a server
    nSessionServerContext* GetServerAt(int index) const;
    /// return pointer to joined session server context, or 0
    nSessionServerContext* GetJoinedServer() const;
    /// join a session, will not go into joined state until answer arrives
    bool JoinSession(const char* sessionGuid);
    /// leave a session, will not go into joined state until answer arrives
    bool LeaveSession();
    /// return true if join has been accepted
    bool IsJoinAccepted() const;
    /// return true if join has been denied
    bool IsJoinDenied() const;
    /// return true if joined
    bool IsJoined() const;

private:
    /// discover new sessions on the broadcast channel
    void DiscoverSessions();
    /// register a new session
    bool RegisterSession(const char* guidStr, const char* hostName, const char* portName);
    /// find a server context object by guid
    nSessionServerContext* FindServerContextByGuid(const char* guid);
    /// cleanup dead session due to timeout
    void CleanupExpiredSessions();
    /// trigger the server contexts
    void TriggerServerContexts();
    /// send client attributes to server
    void SendClientAttrs();

    nString appName;
    nString appVersion;
    double time;
    bool isOpen;
    bool isJoined;
    bool clientAttrsDirty;
    nGuid clientGuid;
    nIpcAddress snifferIpcAddress;
    nIpcPeer* ipcSessionSniffer;
    nRef<nRoot> refServerContexts;
    nRef<nSessionServerContext> refJoinServer;
    nSessionAttrPool clientAttrs;
    int uniqueNumber;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionClient::SetAppName(const char* name)
{
    n_assert(name);
    this->appName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSessionClient::GetAppName() const
{
    return this->appName.IsEmpty() ? 0 : this->appName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionClient::SetAppVersion(const char* version)
{
    n_assert(version);
    this->appVersion = version;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSessionClient::GetAppVersion() const
{
    return this->appVersion.IsEmpty() ? 0 : this->appVersion.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionClient::SetTime(double t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
double
nSessionClient::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSessionClient::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
#endif

