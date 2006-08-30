#ifndef GAME_GAMETIMESOURCE_H
#define GAME_GAMETIMESOURCE_H
//------------------------------------------------------------------------------
/**
    @class Game::GameTimeSource

    Provides timing information for the game logic.

    (C) 2006 Radon Labs GmbH
*/
#include "game/time/timesource.h"

//------------------------------------------------------------------------------
namespace Game
{
class GameTimeSource : public TimeSource
{
    DeclareRtti;
    DeclareFactory(GameTimeSource);
public:
    /// constructor
    GameTimeSource();
    /// destructor
    virtual ~GameTimeSource();
    /// get instance pointer
    static GameTimeSource* Instance();
    /// update current time (called by time manager)
    virtual void UpdateTime(nTime frameTime);
private:
    static GameTimeSource* Singleton;
};

//------------------------------------------------------------------------------
/**
*/
inline
GameTimeSource*
GameTimeSource::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

};
//------------------------------------------------------------------------------
#endif

