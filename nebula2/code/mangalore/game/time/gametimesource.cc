//------------------------------------------------------------------------------
//  gametimesource.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "game/time/gametimesource.h"

namespace Game
{
ImplementRtti(Game::GameTimeSource, Game::TimeSource);
ImplementFactory(Game::GameTimeSource);

GameTimeSource* GameTimeSource::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
GameTimeSource::GameTimeSource()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
GameTimeSource::~GameTimeSource()
{
    n_assert(Singleton);
    Singleton = 0;
}


//--------------------------------------------------------------------
/**
    Called by TimeManager when time is updated. This will update our
    own time and distribute the time to all lowlevel subsystems.
*/
void
GameTimeSource::UpdateTime(nTime fTime)
{
    // update own time
    TimeSource::UpdateTime(fTime);
}

} // namespace Game
