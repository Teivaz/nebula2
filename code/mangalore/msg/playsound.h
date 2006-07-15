#ifndef MSG_PLAYSOUND_H
#define MSG_PLAYSOUND_H
//------------------------------------------------------------------------------
/**
    @class Message::PlaySound

    Play a sound at the specific position.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class PlaySound : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(PlaySound);
    DeclareMsgId;

public:
    /// constructor
    PlaySound();
    /// set sound name (as defined in wave bank)
    void SetName(const nString& n);
    /// get sound name
    const nString& GetName() const;
    /// set sound position
    void SetPosition(const vector3& pos);
    /// get sound position
    const vector3& GetPosition() const;
    /// set sound velocity
    void SetVelocity(const vector3& vel);
    /// get sound velocity
    const vector3& GetVelocity() const;
    /// set sound volume
    void SetVolume(float v);
    /// get sound volume
    float GetVolume() const;

private:
    nString name;
    vector3 position;
    vector3 velocity;
    float volume;
};

RegisterFactory(PlaySound);

//------------------------------------------------------------------------------
/**
*/
inline
PlaySound::PlaySound() :
    volume(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlaySound::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
PlaySound::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlaySound::SetPosition(const vector3& pos)
{
    this->position = pos;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
PlaySound::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlaySound::SetVelocity(const vector3& vel)
{
    this->velocity = vel;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
PlaySound::GetVelocity() const
{
    return this->velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlaySound::SetVolume(float v)
{
    this->volume = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
PlaySound::GetVolume() const
{
    return this->volume;
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
