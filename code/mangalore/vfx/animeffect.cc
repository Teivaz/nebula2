//------------------------------------------------------------------------------
//  animeffect.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/animeffect.h"
#include "msg/gfxsetanimation.h"

namespace VFX
{
ImplementRtti(VFX::AnimEffect, VFX::Effect);
ImplementFactory(VFX::AnimEffect);

using namespace Message;

//------------------------------------------------------------------------------
/**
*/ 
AnimEffect::AnimEffect() :
    stopAnim(false),
    durationOverride(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AnimEffect::~AnimEffect()
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
AnimEffect::OnStart()
{
    Effect::OnStart();
    if (this->stopAnim)
    {
        Ptr<GfxSetAnimation> stopAnimMsg = GfxSetAnimation::Create();
        stopAnimMsg->SetOverlayAnimStop(true);
        this->GetEntity()->SendSync(stopAnimMsg);
    }
    if (this->animName.IsValid())
    {
        Ptr<GfxSetAnimation> setAnimMsg = GfxSetAnimation::Create();
        setAnimMsg->SetOverlayAnimation(this->animName);
        if (this->durationOverride > 0.0)
        {
            setAnimMsg->SetOverlayAnimDurationOverride(this->durationOverride);
        }
        this->GetEntity()->SendSync(setAnimMsg);
    }
    this->OnDeactivate();
}

}; // namespace VFX
