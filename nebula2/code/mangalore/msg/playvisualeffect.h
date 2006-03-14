#ifndef MSG_PLAYVISUALEFFECT_H
#define MSG_PLAYVISUALEFFECT_H
//------------------------------------------------------------------------------
/**
    @class Msg::PlayVisualEffect

    Play a visual effect through the VFX subsystem.
    
    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class PlayVisualEffect : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(PlayVisualEffect);
    DeclareMsgId;

public:
    /// constructor
    PlayVisualEffect();
    /// set effect name (as defined in effect bank)
    void SetName(const nString& n);
    /// get effect name
    const nString& GetName() const;
    /// set world transform
    void SetTransform(const matrix44& m);
    /// get world transform
    const matrix44& GetTransform() const;

private:
    nString name;
    matrix44 transform;
};

RegisterFactory(PlayVisualEffect);

//------------------------------------------------------------------------------
/**
*/
inline
PlayVisualEffect::PlayVisualEffect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlayVisualEffect::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
PlayVisualEffect::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlayVisualEffect::SetTransform(const matrix44& m)
{
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
PlayVisualEffect::GetTransform() const
{
    return this->transform;
}

}; // namespace Msg
//------------------------------------------------------------------------------
#endif