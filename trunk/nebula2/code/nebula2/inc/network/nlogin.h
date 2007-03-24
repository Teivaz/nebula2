#ifndef N_LOGIN_H
#define N_LOGIN_H
//------------------------------------------------------------------------------
/**
    @class nLogin
    @ingroup Network

    login command for buddyclient

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "network/ncommand.h"
#include "kernel/nguid.h"


//------------------------------------------------------------------------------
class nLogin : public nCommand
{
public:
    /// constructor
    nLogin();
    /// destructor
   ~nLogin();

   bool Execute();
   void EvaluateResult(nStream& result);

   void SetPassword(const nString& p);
   void SetUsername(const nString& u);

   const nString& GetUsername();

   void SetGameGuid(nGuid id);

private:

   nString username;
   nString password;

   nGuid gameGuid;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
nLogin::SetUsername(const nString& u)
{
    this->username = u;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nLogin::GetUsername()
{
    return this->username;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nLogin::SetPassword(const nString& p)
{
    this->password = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLogin::SetGameGuid(nGuid id)
{
    this->gameGuid = id;
}



#endif