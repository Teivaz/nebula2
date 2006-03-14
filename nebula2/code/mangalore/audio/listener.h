#ifndef AUDIO_LISTENER_H
#define AUDIO_LISTENER_H
//------------------------------------------------------------------------------
/**
    @class Audio::Listener

    A audio subsystem listener object. Make sure to update the
    listener's transform and velocity each frame and you should be set.
    
    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "audio3/nlistener3.h"
#include "audio3/naudioserver3.h"

//------------------------------------------------------------------------------
namespace Audio
{
class Listener : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Listener);

public:
    /// constructor
    Listener();
    /// destructor
    virtual ~Listener();
    /// commit settings to audio subsystem
    void Commit();
    /// set current transform
    void SetTransform(const matrix44& m);
    /// get current transform
    const matrix44& GetTransform() const;
    /// set current velocity
    void SetVelocity(const vector3& v);
    /// get current velocity
    const vector3& GetVelocity() const;
    /// set rolloff factor
    void SetRollOffFactor(float f);
    /// get rolloff factor
    float GetRollOffFactor() const;
    /// set doppler factor
    void SetDopplerFactor(float f);
    /// get doppler factor
    float GetDopplerFactor() const;

private:
    nListener3 listener;
};

RegisterFactory(Listener);

//------------------------------------------------------------------------------
/**
    Sets the current listener settings in the Nebula2 audio subsystem.
*/
inline
void
Listener::Commit()
{
    nAudioServer3::Instance()->UpdateListener(this->listener);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Listener::SetTransform(const matrix44& m)
{
    this->listener.SetTransform(m);
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
Listener::GetTransform() const
{
    return this->listener.GetTransform();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Listener::SetVelocity(const vector3& v)
{
    this->listener.SetVelocity(v);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
Listener::GetVelocity() const
{
    return this->listener.GetVelocity();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Listener::SetRollOffFactor(float f)
{
    this->listener.SetRollOffFactor(f);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Listener::GetRollOffFactor() const
{
    return this->listener.GetRollOffFactor();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Listener::SetDopplerFactor(float f)
{
    this->listener.SetDopplerFactor(f);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Listener::GetDopplerFactor() const
{
    return this->listener.GetDopplerFactor();
}

}; // namespace Audio

//------------------------------------------------------------------------------
#endif
