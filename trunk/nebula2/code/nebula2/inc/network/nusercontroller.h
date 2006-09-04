#ifndef N_USERCONTROLLER_H
#define N_USERCONTROLLER_H
//------------------------------------------------------------------------------
/**
    @class nUserController
    @ingroup Network

    @brief Central User Controller


    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nref.h"
#include "util/nstring.h"
#include "network/nusercontext.h"


class nUserController
{
public:
    /// constructor
    nUserController();
    /// destructor
    virtual ~nUserController();
    /// get instance pointer
    static nUserController* Instance();

    bool AddUser(int ClientId,nString& name,nGuid& gameID);
    void DeleteUser(int ClientId);
    /// deletes a user by it's name - returns true if found
    bool DeleteUser(nString& user);
    nUserContext* GetUserContext(int ClientId);
    bool GetClientID(nString& user,int& id);

    void Trigger();

private:

    static nUserController* Singleton;

    nArray<nUserContext*> UserContexts;
};

//------------------------------------------------------------------------------
/**
*/
inline
nUserController*
nUserController::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

#endif
