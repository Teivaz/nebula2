#ifndef N_SESSIONSERVERCONTEXT_H
#define N_SESSIONSERVERCONTEXT_H
//------------------------------------------------------------------------------
/**
    @class nSessionServerContext
    @ingroup Network
    @brief Represents a session server on the client side.

    A session client object creates one nSessionServerContext for
    each discovered server in the LAN and contains information about the
    specific server in the form of server attributes.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nguid.h"
#include "kernel/nipcaddress.h"
#include "kernel/nipcclient.h"
#include "network/nsessionattrpool.h"

class nNetClient;

//------------------------------------------------------------------------------
class nSessionServerContext : public nRoot
{
public:
    /// constructor
    nSessionServerContext();
    /// destructor
    virtual ~nSessionServerContext();
    /// set the session guid
    void SetSessionGuid(const char* guid);
    /// get the session guid
    const char* GetSessionGuid() const;
    /// set the client guid
    void SetClientGuid(const nGuid& guid);
    /// get the client guid
    const nGuid& GetClientGuid() const;
    /// set server host name
    void SetHostName(const char* name);
    /// get server host name
    const char* GetHostName() const;
    /// set server port name
    void SetPortName(const char* name);
    /// get server port name
    const char* GetPortName() const;
    /// get a server attribute by name
    const char* GetServerAttr(const char* name);
    /// set keep alive time stamp
    void SetKeepAliveTime(double t);
    /// get keep alive time stamp
    double GetKeepAliveTime() const;
    /// get number of server attributes
    int GetNumServerAttrs() const;
    /// get a server attribute by index
    void GetServerAttrAt(int index, const char*& name, const char*& value);
    /// open connection to the server
    bool Open();
    /// close the connection to the server
    void Close();
    /// return true if open
    bool IsOpen() const;
    /// bool currently joined?
    bool IsJoined() const;
    /// return status of join accepted flag
    bool IsJoinAccepted() const;
    /// return status of join denied flag
    bool IsJoinDenied() const;
    /// per-frame-trigger
    bool Trigger();
    /// join the session on the server
    bool JoinSession();
    /// leave the session
    bool LeaveSession();
    /// send a text message to the server
    bool Send(const char* msg);

private:
    /// set a server attribute
    void SetServerAttr(const char* name, const char* val);
    /// handle a start message from the server
    void HandleStartMessage(const char* gameServerHostName, const char* gameServerPortName);

    nAutoRef<nNetClient> refNetClient;
    nSessionAttrPool serverAttrs;
    nGuid sessionGuid;
    nIpcAddress serverIpcAddress;
    nIpcClient* ipcClient;
    nGuid clientGuid;
    bool isOpen;
    bool isJoined;
    bool isJoinAccepted;
    bool isJoinDenied;
    double keepAliveTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionServerContext::SetSessionGuid(const char* guid)
{
    this->sessionGuid.Set(guid);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSessionServerContext::GetSessionGuid() const
{
    return this->sessionGuid.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionServerContext::SetClientGuid(const nGuid& guid)
{
    this->clientGuid = guid;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nGuid&
nSessionServerContext::GetClientGuid() const
{
    return this->clientGuid;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionServerContext::SetHostName(const char* name)
{
    this->serverIpcAddress.SetHostName(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSessionServerContext::GetHostName() const
{
    return this->serverIpcAddress.GetHostName();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionServerContext::SetPortName(const char* name)
{
    this->serverIpcAddress.SetPortName(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSessionServerContext::GetPortName() const
{
    return this->serverIpcAddress.GetPortName();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionServerContext::SetKeepAliveTime(double t)
{
    this->keepAliveTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
double
nSessionServerContext::GetKeepAliveTime() const
{
    return this->keepAliveTime;
}

//------------------------------------------------------------------------------
#endif

