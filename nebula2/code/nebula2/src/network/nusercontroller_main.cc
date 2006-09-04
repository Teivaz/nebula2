//------------------------------------------------------------------------------
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nsystem.h"
#include "kernel/nkernelserver.h"
#include "network/nusercontroller.h"
#include "util/nstring.h"
#include "network/nbuddyserver.h"

//nNebulaClass(nUserController, "nroot");

nUserController* nUserController::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nUserController::nUserController()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nUserController::~nUserController()
{
    n_assert(Singleton);
    Singleton = 0;
}




void nUserController::Trigger()
{
    nArray<int> lostConnections = nBuddyServer::Instance()->GetLostConnections();


    for (int i=0;i< lostConnections.Size();i++)
    {
        this->DeleteUser(lostConnections[i]);
    }


    nBuddyServer::Instance()->ClearLostConnections();
}


bool nUserController::AddUser(int ClientId,nString& name,nGuid& gameID)
{
    nUserContext* context = n_new(nUserContext);
    context->SetIpcClientId(ClientId);
    context->SetUserName(name);
    context->SetGameGuid(gameID);
    this->UserContexts.PushBack(context);
    return true;
}

void nUserController::DeleteUser(int ClientId)
{
    nArray<nUserContext*>::iterator it;

    for (it = this->UserContexts.Begin();it != this->UserContexts.End();it++)
    {
       if ((*it)->GetIpcClientId()==ClientId)
       {
          n_delete((*it));
          this->UserContexts.EraseQuick(it);
          break;
       }
    }
}

bool nUserController::DeleteUser(nString& user)
{
    nArray<nUserContext*>::iterator it;

    for (it = this->UserContexts.Begin();it != this->UserContexts.End();it++)
    {
       if ((*it)->GetUserName()==user)
       {
          n_delete((*it));
          this->UserContexts.EraseQuick(it);
          return true;
       }
    }

    return false;
}

nUserContext* nUserController::GetUserContext(int ClientId)
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


bool nUserController::GetClientID(nString& user,int& id)
{
    nArray<nUserContext*>::iterator it;

    for (it = this->UserContexts.Begin();it != this->UserContexts.End();it++)
    {
       if ((*it)->GetUserName()==user)
       {
           id = (*it)->GetIpcClientId();
           return true;
       }
    }

    return false;
}

