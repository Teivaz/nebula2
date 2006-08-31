#ifndef VFX_EFFECT_H
#define VFX_EFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::Effect

    Base class for effects. An effect is a small, autonomous object which
    "renders" something into the world for a limited time and which removes
    itself automatically afterwards.

    Every effect may optionally be glued to a target entity, if a
    target entity is set, the transform will be treated locally to the
    entity's transformation, otherwise transform is in global space

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "game/entity.h"
#include "game/time/timesource.h"

//------------------------------------------------------------------------------
namespace VFX
{
class Effect : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Effect);

public:
    /// constructor
    Effect();
    /// destructor
    virtual ~Effect();
    /// set time source to use for timing, if not set, use VFX server's time
    void SetTimeSource(Game::TimeSource* t);
    /// get time source for timing
    Game::TimeSource* GetTimeSource() const;
    /// wrapper to get time
    nTime GetTime() const;
    /// set effects abstract name
    void SetName(const nString& n);
    /// get effects abstract name
    const nString& GetName() const;
    /// set optional entity which delivers the position
    void SetEntity(Game::Entity* entity);
    /// get optional entity
    Game::Entity* GetEntity() const;
    /// return true if entity has been set
    bool HasEntity() const;
    /// set position and orientation in world, or relativ to entity
    void SetTransform(const matrix44& m);
    /// get position and orientation in world, or relative to entity
    const matrix44& GetTransform() const;
    /// returns the resulting transformation in world space
    matrix44 ComputeWorldSpaceTransform() const;
    /// set start delay
    void SetStartDelay(nTime d);
    /// get start delay
    nTime GetStartDelay() const;
    /// set effect duration
    void SetDuration(nTime t);
    /// get effect duration
    nTime GetDuration() const;
    /// set an optional hotspot time
    void SetHotspotTime(nTime t);
    /// get optional hotspot time
    nTime GetHotspotTime() const;
    /// return true if effect hasn't started yet
    bool IsWaiting() const;
    /// return true if currently active
    bool IsPlaying() const;
    /// return true if effect has finished
    bool IsFinished() const;
    /// activate the effect
    virtual void OnActivate();
    /// start the effect, called by OnFrame() after delay is over
    virtual void OnStart();
    /// deactivate the effect
    virtual void OnDeactivate();
    /// trigger the effect
    virtual void OnFrame();

protected:
    nString name;
    Ptr<Game::Entity> entity;
    Ptr<Game::TimeSource> timeSource;
    matrix44 transform;
    nTime startDelay;
    nTime duration;
    nTime activationTime;
    nTime startTime;
    nTime hotspotTime;
    bool isWaiting;
    bool isPlaying;
    bool isFinished;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetTimeSource(Game::TimeSource* t)
{
    this->timeSource = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
Game::TimeSource*
Effect::GetTimeSource() const
{
    this->timeSource.get_unsafe();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Effect::IsWaiting() const
{
    return this->isWaiting;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Effect::IsPlaying() const
{
    return this->isPlaying;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Effect::IsFinished() const
{
    return this->isFinished;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetStartDelay(nTime d)
{
    this->startDelay = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Effect::GetStartDelay() const
{
    return this->startDelay;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Effect::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetTransform(const matrix44& m)
{
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
Effect::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetDuration(nTime t)
{
    this->duration = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Effect::GetDuration() const
{
    return this->duration;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetEntity(Game::Entity* e)
{
    n_assert(0 != e);
    this->entity = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
Game::Entity*
Effect::GetEntity() const
{
    return this->entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Effect::HasEntity() const
{
    return this->entity.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetHotspotTime(nTime t)
{
    this->hotspotTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Effect::GetHotspotTime() const
{
    return this->hotspotTime;
}

};
//------------------------------------------------------------------------------
#endif
