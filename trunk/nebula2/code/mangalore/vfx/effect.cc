//------------------------------------------------------------------------------
//  vfx/effect.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "vfx/effect.h"
#include "vfx/server.h"
#include "attr/attributes.h"

namespace VFX
{
ImplementRtti(VFX::Effect, Foundation::RefCounted);
ImplementFactory(VFX::Effect);

//------------------------------------------------------------------------------
/**
*/
Effect::Effect() :
    startDelay(0.0),
    duration(0.0),
    hotspotTime(0.0),
    activationTime(0.0),
    startTime(0.0),
    isWaiting(false),
    isPlaying(false),
    isFinished(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Effect::~Effect()
{
    if (!this->IsFinished())
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
    Wrapper to get the time. If a time source is set, use this,
    otherwise use VFX::Server's time.
*/
nTime
Effect::GetTime() const
{
    if (this->timeSource.isvalid())
    {
        return this->timeSource->GetTime();
    }
    else
    {
        return VFX::Server::Instance()->GetTime();
    }
}

//------------------------------------------------------------------------------
/**
    This computes the current world space transform of the effect.
    If an entity is set, its current transform will be used and the
    effect transform will be relative to that. If no entity is set,
    the effect transform will be used directly as world space transform.
*/
matrix44
Effect::ComputeWorldSpaceTransform() const
{
    if (this->entity.isvalid())
    {
        return this->transform * this->entity->GetMatrix44(Attr::Transform);
    }
    else
    {
        return this->transform;
    }
}

//------------------------------------------------------------------------------
/**
    Activate the effect. If no start delay is defined (the default) it
    immediately starts playing, otherwise it starts waiting.
*/
void
Effect::OnActivate()
{
    n_assert(!this->IsWaiting() && !this->IsPlaying() && !this->IsFinished());
    this->activationTime = this->GetTime();
    if (this->startDelay > 0.0)
    {
        this->isWaiting = true;
    }
    else
    {
        this->OnStart();
    }
}

//------------------------------------------------------------------------------
/**
    Starts the effect. This is either called directly by OnActivate() if
    no start delay is defined, or after the start delay is over from
    OnFrame().
*/
void
Effect::OnStart()
{
    n_assert(!this->IsPlaying() && !this->IsFinished());
    this->isWaiting = false;
    this->isPlaying = true;
    this->startTime = this->GetTime();
}

//------------------------------------------------------------------------------
/**
    Deactivates the effect and sets its state to finished. This is usually
    called from OnFrame() when the effect has expired.
*/
void
Effect::OnDeactivate()
{
    this->isWaiting = false;
    this->isPlaying = false;
    this->isFinished = true;
}

//------------------------------------------------------------------------------
/**
    Called once per frame by the VFX server.
*/
void
Effect::OnFrame()
{
    n_assert(!this->IsFinished());
    nTime curTime = this->GetTime();

    // if waiting, check if start delay is over...
    if (this->IsWaiting())
    {
        nTime timeDiff = curTime - this->activationTime;
        if ((timeDiff < 0.0) || (timeDiff > (this->startDelay - this->hotspotTime)))
        {
            this->OnStart();
        }
    }

    // if we're playing, check if we're expired
    if (this->IsPlaying())
    {
        nTime timeDiff = curTime - this->startTime;
        if (timeDiff > this->duration && this->duration != 0.0f)
        {
            this->OnDeactivate();
        }
    }
}

} // namespace VFX
