#ifndef N_SESSIONCLIENTCONTEXT_H
#define N_SESSIONCLIENTCONTEXT_H
//------------------------------------------------------------------------------
/**
    @class nSessionClientContext
    @ingroup Network
    @brief Represents a session client on the server side.

    The session server object creates one nSessionClientContext object
    for each connected client. The session client context contains
    all information about the client for the server to know.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "kernel/nguid.h"
#include "variable/nvariableserver.h"
#include "network/nsessionattrpool.h"

//------------------------------------------------------------------------------
class nSessionClientContext : public nRoot
{
public:
    /// constructor
    nSessionClientContext();
    /// destructor
    virtual ~nSessionClientContext();
    /// set the nIpcServer's client id
    void SetIpcClientId(int id);
    /// get the nIpcServer's client id
    int GetIpcClientId() const;
    /// set the client's guid string
    void SetClientGuid(const char* guid);
    /// get the client's guid string
    const char* GetClientGuid() const;
    /// set keepalive timestamp
    void SetKeepAliveTime(double t);
    /// get keepalive timestamp
    double GetKeepAliveTime() const;
    /// get a client attribute by name
    const char* GetClientAttr(const char* name);
    /// get number of client attributes
    int GetNumClientAttrs() const;
    /// get a client attribute by index
    void GetClientAttrAt(int index, const char*& name, const char*& value);

private:
    friend class nSessionServer;

    /// set a client attribute by name
    void SetClientAttr(const char* name, const char* val);

    int ipcClientId;
    nGuid clientGuid;
    nSessionAttrPool clientAttrs;
    double keepAliveTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionClientContext::SetIpcClientId(int id)
{
    this->ipcClientId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSessionClientContext::GetIpcClientId() const
{
    return this->ipcClientId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionClientContext::SetClientGuid(const char* guid)
{
    n_assert(guid);
    this->clientGuid = guid;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSessionClientContext::GetClientGuid() const
{
    return this->clientGuid.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionClientContext::SetClientAttr(const char* name, const char* val)
{
    this->clientAttrs.SetAttr(name, val);
}

//------------------------------------------------------------------------------
/**
    Get a client attribute by name. Returns 0 if attribute doesn't exist.

    @param  name    an attribute name
    @return         value string, or 0
*/
inline
const char*
nSessionClientContext::GetClientAttr(const char* name)
{
    return this->clientAttrs.GetAttr(name);
}

//------------------------------------------------------------------------------
/**
    Get number of client attributes.

    @return     current number of client attributes
*/
inline
int
nSessionClientContext::GetNumClientAttrs() const
{
    return this->clientAttrs.GetNumAttrs();
}

//------------------------------------------------------------------------------
/**
    Get value of client attribute at index.

    @param  index   [in] a client attribute index
    @param  name    [out] attribute name
    @param  value   [out] attribute value
*/
inline
void
nSessionClientContext::GetClientAttrAt(int index, const char*& name, const char*& value)
{
    this->clientAttrs.GetAttrAt(index, name, value);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionClientContext::SetKeepAliveTime(double t)
{
    this->keepAliveTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
double
nSessionClientContext::GetKeepAliveTime() const
{
    return this->keepAliveTime;
}

//------------------------------------------------------------------------------
#endif

