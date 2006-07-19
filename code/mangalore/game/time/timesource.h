#ifndef GAME_TIMESOURCE_H
#define GAME_TIMESOURCE_H
//------------------------------------------------------------------------------
/**
    @class Game::TimeSource
    
    A generic time source object which is attached to the TimeManager.
    Each time source tracks its own time independently from the other
    time sources, they can also be paused and unpaused independentlty 
    from each other, and they may also run faster or slower then
    realtime. 
    
    To create new time sources, derive a subclass from time source. This
    is necessary because time source objects are identified by their
    class id. A positive side effect of this is that time sources
    are created as singletons, so access is very simple, e.g.:

    nTime gameTime = GameTime::Instance()->GetTime();

    Time source classes are responsible for loading/saving their
    state into the globals attribute table.

    (C) 2006 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "attr/attributes.h"
#include "db/reader.h"
#include "db/writer.h"
#include "attr/attributes.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareString(TimeSourceId);
    DeclareFloat(TimeSourceTime);
    DeclareFloat(TimeSourceFactor);
};

namespace Managers
{
    class TimeManager;
};

//------------------------------------------------------------------------------
namespace Game
{
class TimeSource : public Foundation::RefCounted
{
    DeclareRtti;
    DeclareFactory(TimeSource);
public:
    /// constructor
    TimeSource();
    /// destructor
    virtual ~TimeSource();

    /// called when time source is attached to the time manager
    virtual void OnActivate();
    /// called when time source is removed from the time manager
    virtual void OnDeactivate();
    /// read state from database reader
    virtual void OnLoad(Db::Reader* dbReader);
    /// write state to database writer
    virtual void OnSave(Db::Writer* dbWriter);

    /// reset the time source to 0.0
    virtual void Reset();
    /// pause the time source (increments pause counter)
    virtual void Pause();
    /// unpause the time source (decrements pause counter)
    virtual void Continue();
    /// return true if currently paused
    bool IsPaused() const;

    /// set acceleration/deceleration factor
    void SetFactor(float f);
    /// get acceleration/deceleration factor
    float GetFactor() const;
    /// get the current time
    nTime GetTime() const;
    /// get the current frame time
    nTime GetFrameTime() const;
    /// get a unique frame id
    uint GetFrameId() const;

protected:
    friend class Managers::TimeManager;

    /// update current time (called by time manager)
    virtual void UpdateTime(nTime frameTime);

    nTime frameTime; 
    nTime time;
    int pauseCounter;
    float timeFactor;
    uint frameId;
};

//------------------------------------------------------------------------------
/**
*/
inline
uint
TimeSource::GetFrameId() const
{
    return this->frameId;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
TimeSource::GetFrameTime() const
{
    if (this->IsPaused())
    {
        return 0.0f;
    }
    else
    {
        return this->frameTime;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
TimeSource::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
TimeSource::SetFactor(float f)
{
    n_assert(f > 0.0f);
    this->timeFactor = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
TimeSource::GetFactor() const
{
    return this->timeFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
TimeSource::Reset()
{
    this->time = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
TimeSource::Pause()
{
    this->pauseCounter++;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
TimeSource::Continue()
{
    if (this->pauseCounter > 0)
    {
        this->pauseCounter--;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
TimeSource::IsPaused() const
{
    return (this->pauseCounter > 0);
}

}; // namespace Game
//------------------------------------------------------------------------------
#endif    