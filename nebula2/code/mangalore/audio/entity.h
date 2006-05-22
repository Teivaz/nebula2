#ifndef AUDIO_ENTITY_H
#define AUDIO_ENTITY_H
//------------------------------------------------------------------------------
/**
    @class Audio::Entity

    An audio entity associates a sound object with position
    and velocity.
    
    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "audio3/nsound3.h"

//------------------------------------------------------------------------------
namespace Audio
{
class Entity : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Entity);

public:
    /// constructor
    Entity();
    /// destructor
    virtual ~Entity();
    /// called when activated (i.e. attached to a game entity)
    virtual void OnActivate();
    /// called when deactivated
    virtual void OnDeactivate();
    /// set the current world space transform
    void SetTransform(const matrix44& m);
    /// get the current world space transform
    const matrix44& GetTransform() const;
    /// set the current world space velocity
    void SetVelocity(const vector3& vel);
    /// get the current world space velocity
    const vector3& GetVelocity() const;
    /// set the wave resource name
    void SetResourceName(const nString& name);
    /// get the wave resource name
    const nString& GetResourceName() const;
    /// start playback
    void Start();
    /// stop playback
    void Stop();
    /// return true if currently playing
    bool IsPlaying() const;
    /// initiate a fade out
    void FadeOut(nTime fadeOutTime);
    /// update audio entity (call once per frame)
    void Update();
    /// set volume
    void SetVolume(float f);
    /// get volume
    float GetVolume() const;

private:
    bool active;
    nString resourceName;
    matrix44 transform;
    vector3 velocity;
    float volume;
    float maxVolume;
    nSound3* sound;
    bool fistFrameAfterStart;
    bool fadeOutActive;
    nTime fadeOutStarted;
    nTime fadeOutTime;
};

RegisterFactory(Entity);

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetTransform(const matrix44& m)
{
    if (this->sound)
    {
        this->sound->SetTransform(m);
    }
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
Entity::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetVelocity(const vector3& v)
{
    if (this->sound)
    {
        this->sound->SetVelocity(v);
    }
    this->velocity = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
Entity::GetVelocity() const
{
    return this->velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetResourceName(const nString& n)
{
    this->resourceName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Entity::GetResourceName() const
{
    return this->resourceName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetVolume(float f)
{
    if (this->sound)
    {
        this->sound->SetVolume(f * this->maxVolume);
    }
    this->volume = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Entity::GetVolume() const
{
    return this->GetVolume();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Entity::IsPlaying() const
{
    if (this->sound)
    {
        if (this->fistFrameAfterStart)
        {
            // the sound has been started but never updated,
            // dsound will return false, but the sound will start
            // in the next frame
            return true;
        }
        else
        {
            return this->sound->IsPlaying();
        }
    }
    else
    {
        return false;
    }
}

}; // namespace Audio
//------------------------------------------------------------------------------
#endif    
