#ifndef MANAGERS_TIMEMANAGER_H
#define MANAGERS_TIMEMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Managers::TimeManager

    Singleton object which manages the current game time. These are
    the standard time source objects provided by Mangalore:

    SystemTimeSource    - timing for low level Mangalore subsystems
    GameTimeSource      - timing for the game logic
    CameraTimeSource    - extra time source for camera handling
    GuiTimeSource       - time source for user interface stuff
    (C) 2005 Radon Labs GmbH
*/
#include "game/manager.h"
#include "game/time/timesource.h"

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

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();
    /// called after loading game state
    virtual void OnLoad();
    /// called before saving game state
    virtual void OnSave();
    /// update the time manager (called by game state handler early in the frame)
    void Update();
    /// reset all time sources
    void ResetAll();
    /// pause all time sources
    void PauseAll();
    /// continue all time sources
    void ContinueAll();

    /// attach a time source
    void AttachTimeSource(Game::TimeSource* timeSource);
    /// remove a time source
    void RemoveTimeSource(Game::TimeSource* timeSource);
    /// get number of time source
    int GetNumTimeSources() const;
    /// get pointer to time source by index
    Game::TimeSource* GetTimeSourceByIndex(int index) const;
    /// get pointer to time source by class name
    Game::TimeSource* GetTimeSourceByClassName(const nString& n) const;

private:
    static TimeManager* Singleton;

protected:
    nTime time;
    nArray<Ptr<Game::TimeSource> > timeSourceArray;
};

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

} // namespace Managers
//------------------------------------------------------------------------------
#endif
