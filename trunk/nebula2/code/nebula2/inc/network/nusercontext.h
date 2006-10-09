#ifndef N_USERCONTEXT_H
#define N_USERCONTEXT_H
//------------------------------------------------------------------------------
/**
    @class nUserContext
    @ingroup Network

    Represents an user on the server side.

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "kernel/nguid.h"
#include "variable/nvariableserver.h"

//------------------------------------------------------------------------------
class nUserContext
{
public:
    /// constructor
    nUserContext();
    /// destructor
    virtual ~nUserContext();
    /// set the nIpcServer's client id
    void SetIpcClientId(int id);
    /// get the nIpcServer's client id
    int GetIpcClientId() const;
    /// set the client's guid string
    void SetClientGuid(const char* guid);
    /// get the client's guid string
    const char* GetClientGuid() const;


    /// set the game guid string
    void SetGameGuid(nGuid& id);
    /// get the game guid string
    nGuid GetGameGuid() const;

    /// set the client's user name
    void SetUserName(nString& user);
    /// get the client's username
    nString GetUserName() const;


private:
    //friend class nBuddyServer;

    int ipcClientId;
    nString userName;
    nGuid clientGuid;

    nGuid gameGuid;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nUserContext::SetIpcClientId(int id)
{
    this->ipcClientId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nUserContext::GetIpcClientId() const
{
    return this->ipcClientId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nUserContext::SetClientGuid(const char* guid)
{
    n_assert(guid);
    this->clientGuid = guid;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nUserContext::GetClientGuid() const
{
    return this->clientGuid.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nUserContext::SetGameGuid(nGuid& id)
{
    this->gameGuid = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuid
nUserContext::GetGameGuid() const
{
    return this->gameGuid;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nUserContext::SetUserName(nString& user)
{
    this->userName = user;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
nUserContext::GetUserName() const
{
    return this->userName;
}


#endif