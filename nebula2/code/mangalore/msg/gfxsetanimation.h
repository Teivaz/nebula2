#ifndef MSG_GFXSETANIMATION_H
#define MSG_GFXSETANIMATION_H
//------------------------------------------------------------------------------
/**
    @class Message::GfxSetAnimation
    
    Set base or overlay animation on an actor.
    
    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class GfxSetAnimation : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(GfxSetAnimation);
    DeclareMsgId;

public:
    /// constructor
    GfxSetAnimation();
    /// set base animation
    void SetBaseAnimation(const nString& n);
    /// get base animation
    const nString& GetBaseAnimation() const;
    /// set optional base animation time offset
    void SetBaseAnimTimeOffset(float t);
    /// get base animation time offset
    float GetBaseAnimTimeOffset() const;
    /// set overlay animation (override base anim)
    void SetOverlayAnimation(const nString& n);
    /// get overlay animation
    const nString& GetOverlayAnimation() const;

private:
    nString baseAnim;
    nString overlayAnim;
    float baseAnimTimeOffset;
};

RegisterFactory(GfxSetAnimation);

//------------------------------------------------------------------------------
/**
*/
inline
GfxSetAnimation::GfxSetAnimation() :
    baseAnimTimeOffset(0.0f)
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
GfxSetAnimation::SetBaseAnimTimeOffset(float t)
{
    this->baseAnimTimeOffset = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
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

}; // namespace Msg
//------------------------------------------------------------------------------
#endif
