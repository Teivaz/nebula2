#ifndef N_BUDDYSESSION_H
#define N_BUDDYSESSION_H
//------------------------------------------------------------------------------
/**
    @class nBuddySession
    @ingroup Network

    Represents a buddysession on the server side.

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "kernel/nguid.h"
#include "variable/nvariableserver.h"
#include "network/nsessionattrpool.h"

//------------------------------------------------------------------------------
class nBuddySession
{
public:
    /// constructor
    nBuddySession();
    /// destructor
    virtual ~nBuddySession();
    /// set the nIpcServer's client id
    void SetIpcClientId(int id);
    /// get the nIpcServer's client id
    int GetIpcClientId() const;

    /// get a attribute by name
    const char* GetAttr(const char* name);
    /// get number of attributes
    int GetNumAttrs() const;
    /// get a attribute by index
    void GetAttrAt(int index, const char*& name, const char*& value);

private:

    friend class nBuddySessionController;

    /// set a client attribute by name
    void SetAttr(const char* name, const char* val);

    int ipcClientId;

    nSessionAttrPool sessionAttrs;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddySession::SetIpcClientId(int id)
{
    this->ipcClientId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBuddySession::GetIpcClientId() const
{
    return this->ipcClientId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddySession::SetAttr(const char* name, const char* val)
{
    this->sessionAttrs.SetAttr(name, val);
}

//------------------------------------------------------------------------------
/**
    Get a attribute by name. Returns 0 if attribute doesn't exist.

    @param  name    an attribute name
    @return         value string, or 0
*/
inline
const char*
nBuddySession::GetAttr(const char* name)
{
    return this->sessionAttrs.GetAttr(name);
}

//------------------------------------------------------------------------------
/**
    Get number of attributes.

    @return     current number of attributes
*/
inline
int
nBuddySession::GetNumAttrs() const
{
    return this->sessionAttrs.GetNumAttrs();
}


//------------------------------------------------------------------------------
/**
    Get value of attribute at index.

    @param  index   [in]  an attribute index
    @param  name    [out] attribute name
    @param  value   [out] attribute value
*/
inline
void
nBuddySession::GetAttrAt(int index, const char*& name, const char*& value)
{
    return this->sessionAttrs.GetAttrAt(index, name, value);
}


#endif