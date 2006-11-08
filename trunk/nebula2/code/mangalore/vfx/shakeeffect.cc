//------------------------------------------------------------------------------
//  vfx/shakeeffect.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/shakeeffect.h"

namespace VFX
{
ImplementRtti(VFX::ShakeEffect, VFX::Effect);
ImplementFactory(VFX::ShakeEffect);

//------------------------------------------------------------------------------
/**
*/
ShakeEffect::ShakeEffect() :
    range(1.0f),
    intensity(1.0f),
    curIntensity(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShakeEffect::~ShakeEffect()
{
    if (!this->IsFinished())
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ShakeEffect::OnStart()
{
    Effect::OnStart();
    this->curIntensity = this->intensity;
}

//------------------------------------------------------------------------------
/**
*/
void
ShakeEffect::OnFrame()
{
    Effect::OnFrame();
    if (this->IsPlaying())
    {
        // drop off shake intensity by time
        nTime age = this->GetTime() - this->startTime;
        if (age <= this->GetDuration())
        {
            this->curIntensity = this->intensity * (1.0f - n_saturate(float(age / this->duration)));
        }
    }
}

} // namespace VFX
