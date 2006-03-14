#ifndef MANAGERS_TIMEMANAGER_H
#define MANAGERS_TIMEMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Managers::TimeManager

    Singleton object which manages the current game time.
    
    (C) 2005 Radon Labs GmbH 
*/
#include "game/manager.h"

//------------------------------------------------------------------------------
namespace Managers
{
class TimeManager : public Game::Manager
{
    DeclareRtti;
	DeclareFactory(TimeManager);

public:
    /// constructor
    TimeManager();
    /// destructor
    virtual ~TimeManager();
    /// get instance pointer
    static TimeManager* Instance();

    /// set the current time
    virtual void SetTime(nTime t);
    /// get the current time
    virtual nTime GetTime() const;
    /// set the current frame time
    virtual void SetFrameTime(nTime t);
    /// get the current frame time
    virtual nTime GetFrameTime() const;
    /// get a unique frame id
    virtual uint GetFrameId() const;

    /// called per-frame by game server
    virtual void OnFrame();

private:
    static TimeManager* Singleton;

protected:
    nTime time;
    nTime frameTime;
    uint frameId;
};

RegisterFactory(TimeManager);

//------------------------------------------------------------------------------
/**
*/
inline
TimeManager*
TimeManager::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

}; // namespace Managers
//------------------------------------------------------------------------------
#endif