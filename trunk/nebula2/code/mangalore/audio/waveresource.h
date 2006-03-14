#ifndef AUDIO_WAVERESOURCE_H
#define AUDIO_WAVERESOURCE_H
//------------------------------------------------------------------------------
/**
    @class Audio::WaveResource

    A wave resource is an entry in a WaveBank.
    
    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "kernel/nref.h"
#include "util/nstring.h"
#include "audio3/nsound3.h"

//------------------------------------------------------------------------------
namespace Audio
{
class WaveResource : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(WaveResource);

public:
    /// constructor
    WaveResource();
    /// destructor
    virtual ~WaveResource();
    /// set resource name
    void SetName(const nString& n);
    /// get resource name
    const nString& GetName() const;
    /// add a sound object
    void AddSoundObject(nSound3* obj);
    /// get number of sound objects
    int GetNumSoundObjects() const;
    /// get sound object at index
    nSound3* GetSoundObjectAt(int i) const;
    /// get a random sound object
    nSound3* GetRandomSoundObject() const;
    /// set xml table sound volume
    void SetVolume(float v);
    /// get xml table sound volume
    float GetVolume() const;
    /// return true if any of the contained sounds are playing
    bool IsPlaying() const;

private:
    nString name;
    float volume;
    nArray<nRef<nSound3> > sounds;
};

RegisterFactory(WaveResource);

//------------------------------------------------------------------------------
/**
*/
inline
void
WaveResource::SetName(const nString& n)
{
    n_assert(n.IsValid());
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
WaveResource::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WaveResource::AddSoundObject(nSound3* obj)
{
    n_assert(obj);
    this->sounds.Append(obj);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
WaveResource::GetNumSoundObjects() const
{
    return this->sounds.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nSound3*
WaveResource::GetSoundObjectAt(int i) const
{
    return this->sounds[i];
}

//------------------------------------------------------------------------------
/**
*/
inline
nSound3*
WaveResource::GetRandomSoundObject() const
{
    int rndIndex = rand() % this->GetNumSoundObjects();
    return this->sounds[rndIndex];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WaveResource::SetVolume(float v)
{
    this->volume = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
WaveResource::GetVolume() const
{
    return this->volume;
}

}; // namespace Audio
//------------------------------------------------------------------------------
#endif