//------------------------------------------------------------------------------
//  soundeffect.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/soundeffect.h"
#include "audio/server.h"

namespace VFX
{
ImplementRtti(VFX::SoundEffect, VFX::Effect);
ImplementFactory(VFX::SoundEffect);

//------------------------------------------------------------------------------
/**
*/
SoundEffect::SoundEffect() :
    volume(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SoundEffect::~SoundEffect()
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
SoundEffect::OnStart()
{
    Effect::OnStart();
    vector3 pos, vel;
    if (this->HasEntity())
    {
        pos = this->GetEntity()->GetMatrix44(Attr::Transform).pos_component();
        vel = this->GetEntity()->GetVector3(Attr::VelocityVector);
    }
    else
    {
        pos = this->GetTransform().pos_component();
    }
    Audio::Server::Instance()->PlaySoundEffect(this->soundName, pos, vel, this->volume);
    this->OnDeactivate();
}

} // namespace VFX
