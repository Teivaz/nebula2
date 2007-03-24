#ifndef N_CREATEUSER_H
#define N_CREATEUSER_H
//------------------------------------------------------------------------------
/**
    @class nCreateUser
    @ingroup Network

    createuser command for buddyclient

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "network/ncommand.h"


//------------------------------------------------------------------------------
class nCreateUser : public nCommand
{
public:
    /// constructor
    nCreateUser();
    /// destructor
   ~nCreateUser();

   bool Execute();

   void SetPassword(const nString& p);
   void SetUsername(const nString& u);

private:

   nString username;
   nString password;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
nCreateUser::SetUsername(const nString& u)
{
    this->username = u;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nCreateUser::SetPassword(const nString& p)
{
    this->password = p;
}



#endif