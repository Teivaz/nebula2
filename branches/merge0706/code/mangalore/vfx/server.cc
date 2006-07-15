//------------------------------------------------------------------------------
//  vfx/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "vfx/server.h"
#include "message/server.h"
#include "vfx/shakeeffect.h"
#include "foundation/factory.h"

namespace VFX
{
ImplementRtti(VFX::Server, Foundation::RefCounted);
ImplementFactory(VFX::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    curTime(0.0),
    activeEffects(256, 256),
    activeShakeEffects(256, 256),
    statsNumActiveEffects("vfxNumEffects", nArg::Int),
    statsNumActiveShakeEffects("vfxNumShakeEffects", nArg::Int)
{
    n_assert(0 == Singleton);
    Singleton = this;
    this->effectHandler.create();
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(Singleton);
    Singleton = 0;
    if (this->isOpen)
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);

    // register the effect handler
    Message::Server::Instance()->RegisterPort(this->effectHandler);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::Close()
{
    n_assert(this->isOpen);

    // unregister the effect handler
    Message::Server::Instance()->UnregisterPort(this->effectHandler);

    // clear active effects
    this->activeEffects.Clear();
    this->activeShakeEffects.Clear();

    if (this->effectBank.isvalid())
    {
        this->CloseEffectBank();
    }

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Open an effect bank with the specified file name and make it current.
*/
bool
Server::OpenEffectBank(const nString& name)
{
    n_assert(!this->effectBank.isvalid());
    this->effectBank = Bank::Create();
    this->effectBank->SetFilename(name);
    return this->effectBank->Open();
}

//------------------------------------------------------------------------------
/**
    Close and release current effect bank.
*/
void
Server::CloseEffectBank()
{
    n_assert(this->effectBank.isvalid());
    this->effectBank->Close();
    this->effectBank = 0;
}

//------------------------------------------------------------------------------
/**
    Returns pointer to the current visual effects bank (can be 0).
*/
Bank*
Server::GetEffectBank() const
{
    return this->effectBank.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Start playing a visual effect. An effect under this name must exist as a
    template in the currently set effect bank. This will create a new effect object
    which renders itself until the effect has expired. Once expired, the
    effect will remove itself. Make sure to call the Trigger() method
    once per frame for correct garbage collection!

    @param  effectName      name of effect to play
    @param  transform       position/orientation at which to place the effect
*/
void
Server::PlayEffect(const nString& effectName, const matrix44& transform)
{
    n_assert(this->effectBank.isvalid());

    // create a new effect object and add it to the currently active effects
    Effect* newEffect = this->effectBank->CreateEffect(effectName, transform);
    if (newEffect)
    {
        newEffect->SetTime(this->curTime);
        newEffect->Activate();
        this->activeEffects.Append(newEffect);
    }
}

//------------------------------------------------------------------------------
/**
    Create a new particle effect
*/
Effect*
Server::CreateEffect(const nString& effectName, const matrix44& transform)
{
    n_assert(this->effectBank.isvalid());

    // create a new effect object and add it to the currently active effects
    Effect* newEffect = this->effectBank->CreateEffect(effectName, transform);
    if (newEffect)
    {
        newEffect->SetTime(this->curTime);
        newEffect->Activate();
        this->activeEffects.Append(newEffect);
        return newEffect;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Start playing a shake effect.
*/
void
Server::PlayShakeEffect(const vector3& pos, float range, float duration, float intensity)
{
    // create a new shake effect
    Ptr<ShakeEffect> newShakeEffect = ShakeEffect::Create();
    newShakeEffect->SetPosition(pos);
    newShakeEffect->SetRange(range);
    newShakeEffect->SetDuration(duration);
    newShakeEffect->SetIntensity(intensity);
    newShakeEffect->Play();
    this->activeShakeEffects.Append(newShakeEffect);
}

//------------------------------------------------------------------------------
/**
    The BeginScene() method should be called once per frame. Before rendering
    happens.
*/
void
Server::BeginScene()
{
    // process pending effect play messages
    this->effectHandler->HandlePendingMessages();

    // trigger active shake effects
    int i;
    int num = this->activeShakeEffects.Size();
    for (i = 0; i < num; i++)
    {
        this->activeShakeEffects[i]->Trigger();
    }
}

//------------------------------------------------------------------------------
/**
    The EndScene() method must be called after rendering has happened.
    Expired effects will be removed here.
*/
void
Server::EndScene()
{
    // garbage collect expired effects
    nArray<Ptr<Effect> >::iterator effectIter;
    for (effectIter = this->activeEffects.Begin(); effectIter != this->activeEffects.End();)
    {
        if ((*effectIter)->IsActive())
        {
            (*effectIter)->SetTime(this->curTime);
            (*effectIter)->Update();
            effectIter++;
        }
        else if (1 == (*effectIter)->GetRefCount())
        {
            effectIter = this->activeEffects.Erase(effectIter);
        }
        else
        {
            effectIter++;
        }
    }

    // garbage collect expired shake effects
    nArray<Ptr<ShakeEffect> >::iterator shakeIter;
    for (shakeIter = this->activeShakeEffects.Begin(); shakeIter != this->activeShakeEffects.End();)
    {
        if (!(*shakeIter)->IsPlaying())
        {
            n_assert(1 == (*shakeIter)->GetRefCount());
            shakeIter = this->activeShakeEffects.Erase(shakeIter);
        }
        else
        {
            shakeIter++;
        }
    }

    // update statistics
    this->statsNumActiveEffects->SetI(this->activeEffects.Size());
    this->statsNumActiveShakeEffects->SetI(this->activeShakeEffects.Size());
}

//------------------------------------------------------------------------------
/**
    Computes the accumulated shake intensity at a given position.
*/
float
Server::ComputeShakeIntensityAtPosition(const vector3& pos)
{
    float shake = 0.0f;
    int i;
    int num = this->activeShakeEffects.Size();
    for (i = 0; i < num; i++)
    {
        ShakeEffect* curShake = this->activeShakeEffects[i];
        if (curShake->IsPlaying())
        {
            vector3 distVec = pos - curShake->GetPosition();
            float absDist = distVec.len();
            if (absDist < curShake->GetRange())
            {
                float attenuate = 1.0f - n_saturate(absDist / curShake->GetRange());
                shake += attenuate * curShake->GetCurrentIntensity();
            }
        }
    }
    return shake;
}

} // namespace VFX
