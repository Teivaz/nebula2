//--------------------------------------------------------------------
//  systemtimesource.cc
//  (C) 2006 Radon Labs GmbH
//--------------------------------------------------------------------
#include "game/time/systemtimesource.h"
#include "input/server.h"
#include "physics/server.h"
#include "vfx/server.h"
#include "audio/server.h"
#include "graphics/server.h"
#include "particle/nparticleserver2.h"

namespace Game
{
ImplementRtti(Game::SystemTimeSource, Game::TimeSource);
ImplementFactory(Game::SystemTimeSource);

SystemTimeSource* SystemTimeSource::Singleton = 0;

//--------------------------------------------------------------------
/**
*/
SystemTimeSource::SystemTimeSource()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//--------------------------------------------------------------------
/**
*/
SystemTimeSource::~SystemTimeSource()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//--------------------------------------------------------------------
/**
    Called by TimeManager when time is updated. This will update our
    own time and distribute the time to all lowlevel subsystems.
*/
void
SystemTimeSource::UpdateTime(nTime fTime)
{
    // update own time
    TimeSource::UpdateTime(fTime);

    // update subsystem times...
    Input::Server::Instance()->SetTime(this->time);
    Physics::Server::Instance()->SetTime(this->time);
    VFX::Server::Instance()->SetTime(this->time);
    Audio::Server::Instance()->SetTime(this->time);
    Graphics::Server::Instance()->SetTime(this->time);
    Graphics::Server::Instance()->SetFrameTime(this->frameTime);
    nParticleServer2::Instance()->SetTime(this->time);
}

} // namespace Game