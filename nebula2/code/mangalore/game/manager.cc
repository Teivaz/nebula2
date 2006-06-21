//------------------------------------------------------------------------------
//  game/manager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "game/manager.h"

namespace Game
{
ImplementRtti(Game::Manager, Message::Port);

//------------------------------------------------------------------------------
/**
*/
Manager::Manager() :
    isActive(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Manager::~Manager()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called when the manager is attached to the game server.
    The manager base class will register its message port with the
    message server.
*/
void
Manager::OnActivate()
{
    n_assert(!this->isActive);
    this->isActive = true;
    Message::Server::Instance()->RegisterPort(this);
}

//------------------------------------------------------------------------------
/**
    This method is called when the manager is removed from the game server.
    It will unregister its message port from the message server at this point.
*/
void
Manager::OnDeactivate()
{
    n_assert(this->isActive);
    Message::Server::Instance()->UnregisterPort(this);
    this->isActive = true;
}

//------------------------------------------------------------------------------
/**
    The manager base class will invoke the HandlePendingMessages() method
    to process any queued messages.
*/
void
Manager::OnFrame()
{
    this->HandlePendingMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
Manager::OnLoad()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Manager::OnSave()
{
    // empty
}

} // namespace Game
