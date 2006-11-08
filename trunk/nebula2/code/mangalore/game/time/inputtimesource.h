#ifndef GAME_INPUTTIMESOURCE_H
#define GAME_INPUTTIMESOURCE_H
//------------------------------------------------------------------------------
/**
    @class Game::InputTimeSource

    Provides a time source for input handling. This allows the game world
    to be paused independently from input handling.

    (C) 2006 Radon Labs GmbH
*/
#include "game/time/timesource.h"

//------------------------------------------------------------------------------
namespace Game
{
class InputTimeSource : public TimeSource
{
    DeclareRtti;
    DeclareFactory(InputTimeSource);
public:
    /// constructor
    InputTimeSource();
    /// destructor
    virtual ~InputTimeSource();
    /// get instance pointer
    static InputTimeSource* Instance();
private:
    static InputTimeSource* Singleton;
};

//------------------------------------------------------------------------------
/**
*/
inline
InputTimeSource*
InputTimeSource::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

} // namespace Game
//------------------------------------------------------------------------------
#endif
