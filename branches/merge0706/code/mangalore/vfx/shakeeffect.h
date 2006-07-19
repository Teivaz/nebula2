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
#include "vfx/effect.h"
#include "mathlib/nmath.h"

//------------------------------------------------------------------------------
namespace VFX
{
class ShakeEffect : public Effect
{
    DeclareRtti;
	DeclareFactory(ShakeEffect);

public:
    /// constructor
    ShakeEffect();
    /// destructor
    virtual ~ShakeEffect();
    /// set range
    void SetRange(float r);
    /// get range
    float GetRange() const;
    /// set intensity
    void SetIntensity(float i);
    /// get intensity
    float GetIntensity() const;
    /// start the effect
    virtual void OnStart();
    /// trigger the effect
    virtual void OnFrame();
    /// get current intensity, valid after Update has been called
    float GetCurrentIntensity() const;

private:
    float range;
    float intensity;
    float curIntensity;     // current intensity, updated by Trigger
};

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

} // namespace VFX
//------------------------------------------------------------------------------
#endif
