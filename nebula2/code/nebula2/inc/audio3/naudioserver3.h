#ifndef N_AUDIOSERVER3_H
#define N_AUDIOSERVER3_H
//------------------------------------------------------------------------------
/**
    @class nAudioServer3
    @ingroup NebulaAudioSystem

    @brief Audio subsystem 3 server.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nSound3;
class nSoundResource;
class nListener3;

class nAudioServer3 : public nRoot
{
public:
    /// constructor
    nAudioServer3();
    /// destructor
    virtual ~nAudioServer3();
    /// get pointer to server singleton
    static nAudioServer3* Instance();
    /// open the audio device
    virtual bool Open();
    /// close the audio device
    virtual void Close();
    /// reset all sounds
    virtual void Reset();
    /// begin an audio frame
    virtual bool BeginScene(nTime time);
    /// update listener attributes
    virtual void UpdateListener(const nListener3& l);
    /// start a sound
    virtual void StartSound(nSound3* s);
    /// update a sound
    virtual void UpdateSound(nSound3* s);
    /// stop a sound
    virtual void StopSound(nSound3* s);
    /// end the audio frame
    virtual void EndScene();
    /// set the master volume (0.0 .. 1.0)
    void SetMasterVolume(float v);
    /// get the master volume
    float GetMasterVolume() const;
    /// create a non-shared sound object
    virtual nSound3* NewSound();
    /// create a shared sound resource object
    virtual nSoundResource* NewSoundResource(const char* rsrcName);

private:
    static nAudioServer3* Singleton;

protected:
    bool isOpen;
    bool inBeginScene;
    float masterVolume;
    bool masterVolumeDirty;
    nTime masterVolumeChangedTime;
    nTime curTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAudioServer3*
nAudioServer3::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAudioServer3::SetMasterVolume(float v)
{
    this->masterVolume = v;
    this->masterVolumeDirty = true;
    this->masterVolumeChangedTime = this->curTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAudioServer3::GetMasterVolume() const
{
    return this->masterVolume;
}

//------------------------------------------------------------------------------
#endif    

