#ifndef VFX_ANIMEFFECT_H
#define VFX_ANIMEFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::AnimEffect
    
    Starts an animation on the effect's target entity.
    
    (C) 2006 Radon Labs GmbH
*/
#include "vfx/effect.h"

//------------------------------------------------------------------------------
namespace VFX
{
class AnimEffect : public Effect
{
    DeclareRtti;
    DeclareFactory(AnimEffect);
public:
    /// constructor
    AnimEffect();
    /// destructor
    virtual ~AnimEffect();
    /// set overlay animation name
    void SetAnimName(const nString& n);
    /// get overlay animation name
    const nString& GetAnimName() const;
    /// set optional duration override (default is 0.0: use original duration)
    void SetAnimDurationOverride(nTime d);
    /// get optional duration override 
    nTime GetAnimDurationOverride() const;
    /// stop the current overlay animation
    void SetStopAnim(bool b);
    /// get stop overlay anim flag
    bool GetStopAnim() const;
    /// start the effect, called by OnFrame() after delay is over
    virtual void OnStart();

private:
    nString animName;
    nTime durationOverride;
    bool stopAnim;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
AnimEffect::SetAnimDurationOverride(nTime d)
{
    this->durationOverride = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
AnimEffect::GetAnimDurationOverride() const
{
    return this->durationOverride;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
AnimEffect::SetAnimName(const nString& n)
{
    n_assert(n.IsValid());
    this->animName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
AnimEffect::GetAnimName() const
{
    return this->animName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
AnimEffect::SetStopAnim(bool b)
{
    this->stopAnim = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
AnimEffect::GetStopAnim() const
{
    return this->stopAnim;
}

}; // namespace VFX
//------------------------------------------------------------------------------
#endif