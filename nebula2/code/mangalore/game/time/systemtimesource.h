#ifndef GAME_SYSTEMTIMESOURCE_H
#define GAME_SYSTEMTIMESOURCE_H
//------------------------------------------------------------------------------
/**
    @class Game::SystemTimeSource

    Provides the time for the various low level subsystems of Mangalore
    (Graphics, Physics, etc...).

    Access the SystemTimeSource object as Singleton:

    nTime sysTime = SystemTimeSource::Instance()->GetTime();

    (C) 2006 Radon Labs GmbH
*/
#include "game/time/timesource.h"

//------------------------------------------------------------------------------
namespace Game
{
class SystemTimeSource : public TimeSource
{
    DeclareRtti;
    DeclareFactory(SystemTimeSource);
public:
    /// constructor
    SystemTimeSource();
    /// destructor
    virtual ~SystemTimeSource();
    /// get instance pointer
    static SystemTimeSource* Instance();
    /// update current time (called by time manager)
    virtual void UpdateTime(nTime frameTime);
private:
    static SystemTimeSource* Singleton;
};

//------------------------------------------------------------------------------
/**
*/
inline
SystemTimeSource*
SystemTimeSource::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

};
//------------------------------------------------------------------------------
#endif