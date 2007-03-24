//------------------------------------------------------------------------------
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nsystem.h"
#include "kernel/nkernelserver.h"
#include "network/nbuddysessioncontroller.h"
#include "util/nstring.h"
#include "network/nbuddyserver.h"

nBuddySessionController* nBuddySessionController::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nBuddySessionController::nBuddySessionController()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nBuddySessionController::~nBuddySessionController()
{
    n_assert(Singleton);
    Singleton = 0;
}




void nBuddySessionController::Trigger()
{

}


void nBuddySessionController::AddSession(nBuddySession* session)
{
    this->sessions.PushBack(session);
}

void nBuddySessionController::DeleteSession(int ClientId)
{
    nArray<nBuddySession*>::iterator it;

    for (it = this->sessions.Begin();it != this->sessions.End();it++)
    {
       if ((*it)->GetIpcClientId()==ClientId)
       {
          n_delete((*it));
          this->sessions.EraseQuick(it);
          break;
       }
    }
}

/*
nUserContext* nBuddySessionController::GetUserContext(int ClientId)
{
    nArray<nUserContext*>::iterator it;

    for (it = this->UserContexts.Begin();it != this->UserContexts.End();it++)
    {
       if ((*it)->GetIpcClientId()==ClientId)
       {
           return (*it);
       }
    }

    return NULL;
}
*/

