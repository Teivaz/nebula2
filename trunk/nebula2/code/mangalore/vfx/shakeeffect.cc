//------------------------------------------------------------------------------
//  vfx/shakeeffect.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/shakeeffect.h"
#include "vfx/server.h"
#include "message/server.h"

namespace VFX
{
ImplementRtti(VFX::ShakeEffect, Foundation::RefCounted);
ImplementFactory(VFX::ShakeEffect);

//------------------------------------------------------------------------------
/**
*/
ShakeEffect::ShakeEffect() :
    range(1.0f),
    intensity(1.0f),
    isPlaying(false),
    duration(0.01),
    startTime(0.0),
    curIntensity(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShakeEffect::~ShakeEffect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ShakeEffect::Play()
{
    n_assert(!this->isPlaying);
    this->isPlaying = true;
    this->startTime = VFX::Server::Instance()->GetTime();
    this->curIntensity = this->intensity;
}

//------------------------------------------------------------------------------
/**
*/
void
ShakeEffect::Stop()
{
    if (this->IsPlaying())
    {
        this->isPlaying = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ShakeEffect::Trigger()
{
    if (this->isPlaying)
    {
        nTime age = VFX::Server::Instance()->GetTime() - this->startTime;
        if (age <= this->duration)
        {
            // drop off shake intensity by time
            this->curIntensity = this->intensity * (1.0f - n_saturate(float(age / this->duration)));
        }
        else
        {
            // shake effect has expired, stop playback. VFX::Server will
            // care about our garbage collection
            this->Stop();
        }
    }
}

}; // namespace VFX