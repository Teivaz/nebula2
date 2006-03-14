#ifndef VFX_SHAKEEFFECT_H
#define VFX_SHAKEEFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::ShakeEffect

    A shake effect applies a shake shake to the world as long as it's alive.
    Get the resulting accumulated shake for a given position using the 
    VFX::Server::ComputeAccumulatedShake() method.

    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "mathlib/nmath.h"

//------------------------------------------------------------------------------
namespace VFX
{
class ShakeEffect : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(ShakeEffect);

public:
    /// constructor
    ShakeEffect();
    /// destructor
    virtual ~ShakeEffect();
    /// set position
    void SetPosition(const vector3& p);
    /// get position
    const vector3& GetPosition() const;
    /// set range
    void SetRange(float r);
    /// get range
    float GetRange() const;
    /// set intensity
    void SetIntensity(float i);
    /// get intensity
    float GetIntensity() const;
    /// set duration in seconds
    void SetDuration(nTime s);
    /// get duration in seconds
    nTime GetDuration() const;
    /// start shake effect playback
    void Play();
    /// stop shake effect playback
    void Stop();
    /// return true if still playing
    bool IsPlaying() const;
    /// trigger the shake effect
    void Trigger();
    /// get current intensity, valid after Trigger has been called
    float GetCurrentIntensity() const;

private:
    vector3 position;
    float range;
    float intensity;
    bool isPlaying;
    nTime duration;
    nTime startTime;
    float curIntensity;     // current intensity, updated by Trigger
};

RegisterFactory(ShakeEffect);

//------------------------------------------------------------------------------
/**
*/
inline
void
ShakeEffect::SetPosition(const vector3& p)
{
    this->position = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
ShakeEffect::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ShakeEffect::SetRange(float r)
{
    this->range = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ShakeEffect::GetRange() const
{
    return this->range;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ShakeEffect::SetDuration(nTime d)
{
    // don't allow a zero duration
    if (d <= N_TINY)
    {
        d = 0.001f;
    }
    this->duration = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
ShakeEffect::GetDuration() const
{
    return this->duration;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ShakeEffect::IsPlaying() const
{
    return this->isPlaying;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ShakeEffect::SetIntensity(float i)
{
    this->intensity = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ShakeEffect::GetIntensity() const
{
    return this->intensity;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ShakeEffect::GetCurrentIntensity() const
{
    return this->curIntensity;
}

}; // namespace Vfx
//------------------------------------------------------------------------------
#endif