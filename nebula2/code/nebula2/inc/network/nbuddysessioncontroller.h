#ifndef N_SESSIONCONTROLLER_H
#define N_SESSIONCONTROLLER_H
//------------------------------------------------------------------------------
/**
    @class nBuddySessionController
    @ingroup Network

    @brief Central Session Controller


    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nref.h"
#include "util/nstring.h"
#include "network/nbuddysession.h"

class nBuddySessionController
{
public:
    /// constructor
    nBuddySessionController();
    /// destructor
    virtual ~nBuddySessionController();
    /// get instance pointer
    static nBuddySessionController* Instance();

    void AddSession(nBuddySession* session);
    void DeleteSession(int ClientId);
   // nUserContext* GetUserContext(int ClientId);
   // bool GetClientID(nString& user,int& id);

    void Trigger();

private:

    static nBuddySessionController* Singleton;

    nArray<nBuddySession*> sessions;
};

//------------------------------------------------------------------------------
/**
*/
inline
nBuddySessionController*
nBuddySessionController::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

#endif
