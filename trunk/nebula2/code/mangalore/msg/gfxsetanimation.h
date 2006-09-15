#ifndef MSG_GFXSETANIMATION_H
#define MSG_GFXSETANIMATION_H
//------------------------------------------------------------------------------
/**
    @class Message::GfxSetAnimation

    Set base or overlay animation on an actor.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"
#include "graphics/charentity.h"

//------------------------------------------------------------------------------
namespace Message
{
class GfxSetAnimation : public Msg
{
    DeclareRtti;
    DeclareFactory(GfxSetAnimation);
    DeclareMsgId;

public:
    /// constructor
    GfxSetAnimation();

    /// set optional fade in time
    void SetFadeInTime(nTime t);
    /// get optional fade in time
    nTime GetFadeInTime() const;

    /// set base animation
    void SetBaseAnimation(const nString& n);
    /// get base animation
    const nString& GetBaseAnimation() const;

    /// set mixed base animation names and weights
    void SetMixedBaseAnimation(const nArray<nString>& n, const nArray<float>& w);
    /// get mixed base animation names
    const nArray<nString>& GetMixedBaseAnimationNames() const;
    /// get mixed base animation weights
    const nArray<float>& GetMixedBaseAnimationWeights() const;

    /// set optional base animation time offset
    void SetBaseAnimTimeOffset(nTime t);
    /// get base animation time offset
    nTime GetBaseAnimTimeOffset() const;

    /// set overlay animation (override base anim)
    void SetOverlayAnimation(const nString& n);
    /// get overlay animation
    const nString& GetOverlayAnimation() const;

    /// set mixed overlay animation names and weights
    void SetMixedOverlayAnimation(const nArray<nString>& n, const nArray<float>& w);
    /// get mixed overlay animation names
    const nArray<nString>& GetMixedOverlayAnimationNames() const;
    /// get mixed overlay animation weights
    const nArray<float>& GetMixedOverlayAnimationWeights() const;

    /// set optional overlay anim duration override
    void SetOverlayAnimDurationOverride(nTime t);
    /// get optional overlay anim duration override
    nTime GetOverlayAnimDurationOverride() const;
    /// stop current overlay animation
    void SetOverlayAnimStop(bool b);
    /// get overlay anim stop flag
    bool GetOverlayAnimStop() const;

private:
    nTime fadeInTime;
    nString baseAnim;
    nString overlayAnim;
    nTime baseAnimTimeOffset;
    nTime overlayAnimDurationOverride;
    bool stopOverlayAnim;

    nArray<nString> mixedBaseAnimNames;
    nArray<float> mixedBaseAnimWeights;
    nArray<nString> mixedOverlayAnimNames;
    nArray<float> mixedOverlayAnimWeights;
};

RegisterFactory(GfxSetAnimation);

//------------------------------------------------------------------------------
/**
*/
inline
GfxSetAnimation::GfxSetAnimation() :
    fadeInTime(0.2),
    baseAnimTimeOffset(0.0),
    overlayAnimDurationOverride(0.0),
    stopOverlayAnim(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetAnimation::SetBaseAnimation(const nString& n)
{
    this->baseAnim = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GfxSetAnimation::GetBaseAnimation() const
{
    return this->baseAnim;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetAnimation::SetMixedBaseAnimation(const nArray<nString>& n, const nArray<float>& w)
{
    this->mixedBaseAnimNames = n;
    this->mixedBaseAnimWeights = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
GfxSetAnimation::GetMixedBaseAnimationNames() const
{
    return this->mixedBaseAnimNames;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<float>&
GfxSetAnimation::GetMixedBaseAnimationWeights() const
{
    return this->mixedBaseAnimWeights;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetAnimation::SetFadeInTime(nTime t)
{
    this->fadeInTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
GfxSetAnimation::GetFadeInTime() const
{
    return this->fadeInTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetAnimation::SetBaseAnimTimeOffset(nTime t)
{
    this->baseAnimTimeOffset = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
GfxSetAnimation::GetBaseAnimTimeOffset() const
{
    return this->baseAnimTimeOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetAnimation::SetOverlayAnimation(const nString& n)
{
    this->overlayAnim = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GfxSetAnimation::GetOverlayAnimation() const
{
    return this->overlayAnim;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetAnimation::SetMixedOverlayAnimation(const nArray<nString>& n, const nArray<float>& w)
{
    this->mixedOverlayAnimNames = n;
    this->mixedOverlayAnimWeights = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
GfxSetAnimation::GetMixedOverlayAnimationNames() const
{
    return this->mixedOverlayAnimNames;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<float>&
GfxSetAnimation::GetMixedOverlayAnimationWeights() const
{
    return this->mixedOverlayAnimWeights;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetAnimation::SetOverlayAnimDurationOverride(nTime t)
{
    this->overlayAnimDurationOverride = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
GfxSetAnimation::GetOverlayAnimDurationOverride() const
{
    return this->overlayAnimDurationOverride;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetAnimation::SetOverlayAnimStop(bool b)
{
    this->stopOverlayAnim = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
GfxSetAnimation::GetOverlayAnimStop() const
{
    return this->stopOverlayAnim;
}

} // namespace Msg
#endif
