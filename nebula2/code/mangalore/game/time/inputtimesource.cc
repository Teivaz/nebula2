//------------------------------------------------------------------------------
//  inputtimesource.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "game/time/inputtimesource.h"

namespace Game
{
ImplementRtti(Game::InputTimeSource, Game::TimeSource);
ImplementFactory(Game::InputTimeSource);

InputTimeSource* InputTimeSource::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
InputTimeSource::InputTimeSource()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
InputTimeSource::~InputTimeSource()
{
    n_assert(Singleton);
    Singleton = 0;
}

} // namespace Game
