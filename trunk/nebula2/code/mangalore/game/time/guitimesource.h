#ifndef GAME_GUITIMESOURCE_H
#define GAME_GUITIMESOURCE_H
//------------------------------------------------------------------------------
/**
    @class Game::GuiTimeSource

    The time source for the user interface subsystem(s). Overrides some
    method so that it can never be paused.

    (C) 2006 Radon Labs GmbH
*/
#include "game/time/timesource.h"

//------------------------------------------------------------------------------
namespace Game
{
class GuiTimeSource : public TimeSource
{
    DeclareRtti;
    DeclareFactory(GuiTimeSource);
public:
    /// constructor
    GuiTimeSource();
    /// destructor
    virtual ~GuiTimeSource();
    /// get instance pointer
    static GuiTimeSource* Instance();
    /// update current time (called by time manager)
    virtual void UpdateTime(nTime frameTime);
    /// pause the time source (increments pause counter)
    virtual void Pause();
    /// unpause the time source (decrements pause counter)
    virtual void Continue();
private:
    static GuiTimeSource* Singleton;
};

//------------------------------------------------------------------------------
/**
*/
inline
GuiTimeSource*
GuiTimeSource::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

};
//------------------------------------------------------------------------------
#endif