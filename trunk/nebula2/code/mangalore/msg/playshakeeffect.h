#ifndef MSG_PLAYSHAKEEFFECT_H
#define MSG_PLAYSHAKEEFFECT_H
//------------------------------------------------------------------------------
/**
    @class Message::PlayShakeEffect

    Message to play a camera shake effect.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class PlayShakeEffect : public Msg
{
    DeclareRtti;
    DeclareFactory(PlayShakeEffect);
    DeclareMsgId;

public:
    /// constructor
    PlayShakeEffect();
    /// set transform
    void SetTransform(const matrix44& tform);
    /// get transform
    const matrix44& GetTransform() const;
    /// set range
    void SetRange(float r);
    /// get range
    float GetRange() const;
    /// set duration
    void SetDuration(float d);
    /// get duration
    float GetDuration() const;
    /// set intensity
    void SetIntensity(float i);
    /// get intensity
    float GetIntensity() const;

private:
    matrix44 transform;
    float range;
    float duration;
    float intensity;
};

RegisterFactory(PlayShakeEffect);

//------------------------------------------------------------------------------
/**
*/
inline
PlayShakeEffect::PlayShakeEffect() :
    range(1.0f),
    duration(1.0f),
    intensity(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlayShakeEffect::SetTransform(const matrix44& tform)
{
    this->transform = tform;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
PlayShakeEffect::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlayShakeEffect::SetRange(float r)
{
    this->range = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
PlayShakeEffect::GetRange() const
{
    return this->range;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlayShakeEffect::SetDuration(float d)
{
    this->duration = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
PlayShakeEffect::GetDuration() const
{
    return this->duration;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlayShakeEffect::SetIntensity(float i)
{
    this->intensity = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
PlayShakeEffect::GetIntensity() const
{
    return this->intensity;
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
