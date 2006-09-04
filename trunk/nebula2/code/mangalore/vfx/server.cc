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
    statsNumActiveEffects("vfxNumEffects", nArg::Int)
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
    Message::Server::Instance()->UnregisterPort(this->effectHandler);

    // cleanup active effects
    int num = this->activeEffects.Size();
    for (int i = 0; i < num; i++)
    {
        if (!this->activeEffects[i]->IsFinished())
        {
            this->activeEffects[i]->OnDeactivate();
        }
        this->activeEffects[i] = 0;
    }
    this->activeEffects.Clear();
    this->CloseEffectBank();
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
    if (this->effectBank.isvalid())
    {
        this->effectBank->Close();
        this->effectBank = 0;
    }
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
    Attach a generic effect to the world.
*/
void
Server::AttachEffect(Effect* effect)
{
    n_assert(0 != effect);
    effect->OnActivate();
    this->activeEffects.Append(effect);
}

//------------------------------------------------------------------------------
/**
    Remove an effect from the world.
*/
void
Server::RemoveEffect(Effect* effect)
{
    n_assert(0 != effect);
    int index = this->activeEffects.FindIndex(effect);
    if (-1 != index)
    {
        if (!this->activeEffects[index]->IsFinished())
        {
            this->activeEffects[index]->OnDeactivate();
        }
        this->activeEffects.Erase(index);
    }
}

//------------------------------------------------------------------------------
/**
*/
GraphicsEffect*
Server::FindGraphicsEffectTemplate(const nString& effectName)
{
    return this->effectBank->FindEffect(effectName);
}

//------------------------------------------------------------------------------
/**
*/
GraphicsEffect*
Server::CreateGraphicsEffect(const nString& effectName, const matrix44& transform)
{
    GraphicsEffect* newEffect = this->effectBank->CreateGraphicsEffect(effectName, transform);
    return newEffect;
}

//------------------------------------------------------------------------------
/**
*/
ShakeEffect*
Server::CreateShakeEffect(const matrix44& tform, float range, float duration, float intensity)
{
    // create a new shake effect
    Ptr<ShakeEffect> newShakeEffect = ShakeEffect::Create();
    newShakeEffect->SetTransform(tform);
    newShakeEffect->SetRange(range);
    newShakeEffect->SetDuration(duration);
    newShakeEffect->SetIntensity(intensity);
    return newShakeEffect;
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
}

//------------------------------------------------------------------------------
/**
    The EndScene() method must be called after rendering has happened.
    Expired effects will be removed here.
*/
void
Server::EndScene()
{
    // update active effects
    int num = this->activeEffects.Size();
    for (int i = 0; i < num; i++)
    {
        if (!this->activeEffects[i]->IsFinished())
        {
            this->activeEffects[i]->OnFrame();
        }
    }

    // garbage collect expired effects
    nArray<Ptr<Effect> >::iterator effectIter;
    for (effectIter = this->activeEffects.Begin(); effectIter != this->activeEffects.End();)
    {
        if ((*effectIter)->IsFinished())
        {
            effectIter = this->activeEffects.Erase(effectIter);
        }
        else
        {
            effectIter++;
        }
    }

    // update statistics
    this->statsNumActiveEffects->SetI(this->activeEffects.Size());
}

//------------------------------------------------------------------------------
/**
    Computes the accumulated shake intensity at a given position.
*/
float
Server::ComputeShakeIntensityAtPosition(const vector3& pos)
{
    float shake = 0.0f;
    int num = this->activeEffects.Size();
    for (int i = 0; i < num; i++)
    {
        if (this->activeEffects[i]->IsA(ShakeEffect::RTTI))
        {
            ShakeEffect* curShake = (ShakeEffect*) this->activeEffects[i].get();
            if (curShake->IsPlaying())
            {
                vector3 distVec = pos - curShake->ComputeWorldSpaceTransform().pos_component();
                float absDist = distVec.len();
                if (absDist < curShake->GetRange())
                {
                    float attenuate = 1.0f - n_saturate(absDist / curShake->GetRange());
                    shake += attenuate * curShake->GetCurrentIntensity();
                }
            }
        }
    }
    return shake;
}

} // namespace VFX
