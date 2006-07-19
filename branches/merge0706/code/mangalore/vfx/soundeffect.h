#ifndef VFX_SOUNDEFFECT_H
#define VFX_SOUNDEFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::SoundEffect
  
    Plays a sound effect at the position of the entity if exists, or the 
    transform.
    
    (C) 2006 Radon Labs GmbH
*/    
#include "vfx/effect.h"

//------------------------------------------------------------------------------
namespace VFX
{
class SoundEffect : public Effect
{
    DeclareRtti;
    DeclareFactory(SoundEffect);
public:
    /// constructor
    SoundEffect();
    /// destructor
    virtual ~SoundEffect();
    /// set sound name
    void SetSoundName(const nString& n);
    /// get sound name
    const nString& GetSoundName() const;
    /// set sound volume
    void SetVolume(float v);
    /// get sound volume
    float GetVolume() const;
    /// start the effect, called by OnFrame() after delay is over
    virtual void OnStart();

private:
    nString soundName;
    float volume;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
SoundEffect::SetSoundName(const nString& n)
{
    this->soundName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
SoundEffect::GetSoundName() const
{
    return this->soundName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
SoundEffect::SetVolume(float v)
{
    this->volume = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
SoundEffect::GetVolume() const
{
    return this->volume;
}

}; // namespace VFX
//------------------------------------------------------------------------------
#endif
