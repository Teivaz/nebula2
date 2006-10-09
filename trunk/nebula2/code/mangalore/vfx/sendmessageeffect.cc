//------------------------------------------------------------------------------
//  sendmessageeffect.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/sendmessageeffect.h"

namespace VFX
{
ImplementRtti(VFX::SendMessageEffect, VFX::Effect);
ImplementFactory(VFX::SendMessageEffect);

//------------------------------------------------------------------------------
/**
*/
SendMessageEffect::SendMessageEffect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SendMessageEffect::~SendMessageEffect()
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
SendMessageEffect::OnStart()
{
    Effect::OnStart();
    for (int i = 0; i < this->messages.Size(); i++)
    {
        this->GetEntity()->SendSync(this->messages[i]);
    }
    this->OnDeactivate();
}

} // namespace VFX
