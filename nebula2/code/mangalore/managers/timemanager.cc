//------------------------------------------------------------------------------
//  managers/timemanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "managers/timemanager.h"

namespace Managers
{
ImplementRtti(Managers::TimeManager, Game::Manager);
ImplementFactory(Managers::TimeManager);

TimeManager* TimeManager::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
TimeManager::TimeManager() :
    time(0.0),
    frameTime(0.0),
    frameId(0)
{    
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
TimeManager::~TimeManager()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Sets the current time. This is usually called by Application::GameStateHandler.
*/
void
TimeManager::SetTime(nTime t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
    Returns the current time.
*/
nTime
TimeManager::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
    Set the current frame time. This is usually called by 
    Application::GameStateHandler.
*/
void
TimeManager::SetFrameTime(nTime t)
{
    this->frameTime = t;
}

//------------------------------------------------------------------------------
/**
    Get the current frame time.
*/
nTime
TimeManager::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
/**
    Get the current frame id. This is an id which uniquely identifies
    the current frame.
*/
uint
TimeManager::GetFrameId() const
{
    return this->frameId;
}

//------------------------------------------------------------------------------
/**
    The per-frame method is called by the game server. It currently
    just updates the frame id.
*/
void
TimeManager::OnFrame()
{
    this->frameId++;
}

} // namespace Game
