#ifndef N_AUDIOSERVER3_H
#define N_AUDIOSERVER3_H
//------------------------------------------------------------------------------
/**
    @class nAudioServer3
    @ingroup Audio3

    Audio subsystem 3 server.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "util/nfixedarray.h"

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
    /// return true if currently open
    bool IsOpen() const;
    /// reset all sounds
    virtual void Reset();
    /// begin an audio frame
    virtual bool BeginScene(nTime time);
    /// is inside BeginScene()<->EndScene() ?
    virtual bool IsInBeginScene() const;
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
    /// update sounds (can be called outside of BeginScene()/EndScene() to immediately commit changes
    virtual void UpdateAllSounds();
    /// sound categorys
    enum Category
    {
        Effect = 0,
        Music,
        Speech,
        Ambient,
        NumCategorys,
        InvalidCategory,
    };
    /// convert category to string
    static nString CategoryToString(Category category);
    /// convert string to category
    static Category StringToCategory(const nString& string);
    /// set the master volume (0.0 .. 1.0)
    void SetMasterVolume(Category category, float v);
    /// get the master volume
    float GetMasterVolume(Category category) const;
    /// create a non-shared sound object
    virtual nSound3* NewSound();
    /// create a shared sound resource object
    virtual nSoundResource* NewSoundResource(const char* rsrcName);

    /// mute all category volumes 
    void Mute();
    /// restore all muted category volumes
    void Unmute();
    /// is currently muted?
    bool IsMuted() const;

private:
    static nAudioServer3* Singleton;

protected:
    bool isOpen;
    bool inBeginScene;
    nFixedArray<float> masterVolume;
    nFixedArray<bool>  masterVolumeDirty;
    nFixedArray<nTime> masterVolumeChangedTime;
    nFixedArray<float> masterVolumeMuted;
    bool isMuted;
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
bool
nAudioServer3::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAudioServer3::SetMasterVolume(Category category, float v)
{
    n_assert(category >= 0 && category < NumCategorys);
    this->masterVolume[category] = v;
    this->masterVolumeDirty[category] = true;
    this->masterVolumeChangedTime[category] = this->curTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAudioServer3::GetMasterVolume(Category category) const
{
    n_assert(category >= 0 && category < NumCategorys);
    return this->masterVolume[category];
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAudioServer3::IsInBeginScene() const
{
    return this->inBeginScene;
};


//------------------------------------------------------------------------------
#endif    

