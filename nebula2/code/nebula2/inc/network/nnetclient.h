#ifndef N_NETCLIENT_H
#define N_NETCLIENT_H
//------------------------------------------------------------------------------
/**
    @class nNetClient
    @ingroup Network
    @brief A client in a multiplayer session.  Works together with
    nNetServer, should be subclassed for a specific application or protocol.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nguid.h"
#include "kernel/nipcaddress.h"
#include "kernel/nipcclient.h"

//------------------------------------------------------------------------------
class nNetClient : public nRoot
{
public:
    /// client status enum
    enum ClientStatus
    {
        Invalid,
        Connecting,         // trying to establish server connection
        Connected,          // connected, waiting for authentification
        JoinAccepted,       // authentification accepted by server
        JoinDenied,         // authentification denied by server
        Started,            // session started by server
        Timeout,            // connection timed out
    };

    /// constructor
    nNetClient();
    /// destructor
    virtual ~nNetClient();
    /// set the client's guid
    void SetClientGuid(const char* guid);
    /// get the client's guid
    const char* GetClientGuid() const;
    /// set the server's host name
    void SetServerHostName(const char* name);
    /// get the server's host name
    const char* GetServerHostName() const;
    /// set the server's port name
    void SetServerPortName(const char* name);
    /// get the server's port name
    const char* GetServerPortName() const;
    /// set the current time
    void SetTime(double t);
    /// get the current time
    double GetTime() const;
    /// open the client
    bool Open();
    /// close the client
    void Close();
    /// return true if open
    bool IsOpen() const;
    /// per-frame-trigger
    void Trigger();
    /// get the client's connection status
    ClientStatus GetClientStatus() const;

protected:
    /// handle a custom message
    virtual void HandleMessage(const char* msg);
    /// set current client status
    void SetClientStatus(ClientStatus status);

    nGuid clientGuid;
    nIpcAddress ipcAddress;
    nIpcClient* ipcClient;
    bool isOpen;
    ClientStatus clientStatus;
    double time;
    double retryTime;
    int numRetries;
};
//------------------------------------------------------------------------------
/**
*/
inline
void
nNetClient::SetClientGuid(const char* guid)
{
    n_assert(guid);
    this->clientGuid.Set(guid);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nNetClient::GetClientGuid() const
{
    return this->clientGuid.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetClient::SetServerHostName(const char* name)
{
    this->ipcAddress.SetHostName(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nNetClient::GetServerHostName() const
{
    return this->ipcAddress.GetHostName();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetClient::SetServerPortName(const char* name)
{
    this->ipcAddress.SetPortName(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nNetClient::GetServerPortName() const
{
    return this->ipcAddress.GetPortName();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetClient::SetTime(double t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
double
nNetClient::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNetClient::SetClientStatus(ClientStatus status)
{
    this->clientStatus = status;
}

//------------------------------------------------------------------------------
/**
*/
inline
nNetClient::ClientStatus
nNetClient::GetClientStatus() const
{
    return this->clientStatus;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nNetClient::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
#endif

