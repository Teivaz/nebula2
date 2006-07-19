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

}; // namespace Game
