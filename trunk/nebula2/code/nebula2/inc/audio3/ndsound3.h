#ifndef N_DSOUND3_H
#define N_DSOUND3_H
//------------------------------------------------------------------------------
/**
    @class nDSound3

    DirectSound based implementation of nSound3. Works together with
    nDSoundServer3 and nDSoundResource.
    
    (C) 2003 RadonLabs GmbH
*/
#include "audio3/nsound3.h"
#include "audio3/ndsoundresource.h"
#include "audio3/ndsound_wavfmt.h"

class nDSoundServer3;

//------------------------------------------------------------------------------
class nDSound3 : public nSound3
{
public:
    /// constructor
    nDSound3();
    /// destructor
    virtual ~nDSound3();
    /// start the sound
    virtual void Start();
    /// stop the sound
    virtual void Stop();
    /// update the sound
    virtual void Update();
    /// get pointer to embedded nDSoundResource object
    nDSoundResource* GetDSoundResource();
    /// get the volume converted to DirectSound conventions
    LONG GetDSVolume();
    /// get DirectSound 3D-Soundbuffer properties
    DS3DBUFFER* GetDS3DProps();
    /// convert a linear volume into a DirectSound volume value
    static LONG AsDirectSoundVolume(float vol);

protected:
    /// load the resource 
    virtual bool LoadResource();
    /// unload the resource
    virtual void UnloadResource();

private:
    nAutoRef<nDSoundServer3> refSoundServer;
    nRef<nDSoundResource> refSoundResource;
    LONG dsVolume;
    DS3DBUFFER ds3DProps;


    // CSound, CSoundStreaming ports


protected:
    // sound
    LPDIRECTSOUNDBUFFER*    m_apDSBuffer;
    DWORD                   m_dwDSBufferSize;
    nDSound_WavFmt*         m_pWaveFile;
    DWORD                   m_dwNumBuffers;
    DWORD                   m_dwCreationFlags;

    // streaming
    DWORD                   m_dwLastPlayPos;
    DWORD                   m_dwPlayProgress;
    DWORD                   m_dwNotifySize;
    DWORD                   m_dwTriggerWriteOffset;
    DWORD                   m_dwNextWriteOffset;
    BOOL                    m_bFillNextNotificationWithSilence;
    DWORD                   m_dwDS3DPROPSMode;

    bool                RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB,
                                       BOOL* pbWasRestored );

public:
    /// create direct sound buffer
    void                createDSBuffer( LPDIRECTSOUNDBUFFER* apDSBuffer,
                                        DWORD dwDSBufferSize, 
                                        DWORD dwNumBuffers,
                                        nDSound_WavFmt* pWaveFile,
                                        DWORD dwCreationFlags );
    /// create direct sound buffer for streaming
    void                createDSBufferStreaming( LPDIRECTSOUNDBUFFER pDSBuffer,
                                                 DWORD dwDSBufferSize, 
                                                 nDSound_WavFmt* pWaveFile,
                                                 DWORD dwNotifySize );
    /// get IID_IDirectSound3DBuffer by index
    long                Get3DBufferInterface( DWORD dwIndex,
                                              LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );
    /// fill buffer with sound
    bool                FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB,
                                             BOOL bRepeatWavIfBufferLarger );
    /// get free(temporary pause or non-using) buffer
    LPDIRECTSOUNDBUFFER GetFreeBuffer();
    /// get dsound buffer by index
    LPDIRECTSOUNDBUFFER GetBuffer( DWORD dwIndex );
    /// play
    long                Play( DWORD dwPriority = 0,
                              DWORD dwFlags = 0,
                              LONG lVolume = 0,
                              LONG lFrequency = -1,
                              LONG lPan = 0 );
    /// play 3d
    bool                Play3D( LPDS3DBUFFER p3DBuffer,
                                DWORD dwPriority = 0,
                                DWORD dwFlags = 0,
                                LONG lFrequency = 0 );
    /// reset, rewind to 0
    long                Reset();
    /// is sound playing?
    BOOL                IsSoundPlaying();
    /// stream notification
    bool                HandleWaveStreamNotification( BOOL bLoopedPlay );
    /// get current write cursor position and than check is inside
    bool                CheckStreamUpdate();
    /// is inside
    bool                Inside(DWORD pos, DWORD start, DWORD end);
};

//------------------------------------------------------------------------------
/**
    Convert a linear volume between 0.0f and 1.0f into a Dezibel-based
    DirectSound-Volume
*/
inline
LONG
nDSound3::AsDirectSoundVolume(float vol)
{
    float scaledVolume = 0.0f;
    if (vol > 0.0f)
    {
        const float minVol = 0.6f;
        const float maxVol = 1.0f;
        scaledVolume = minVol + (vol * (maxVol - minVol));
    }
    float dsVol = DSBVOLUME_MIN + ((DSBVOLUME_MAX - DSBVOLUME_MIN) * scaledVolume);
    return (LONG) dsVol;
}

//------------------------------------------------------------------------------
/**
    Get volume as DirectSound volume value.
*/
inline
LONG
nDSound3::GetDSVolume()
{
    if (this->volumeDirty)
    {
        this->volumeDirty = false;
        this->dsVolume = AsDirectSoundVolume(this->volume);
    }
    return this->dsVolume;
}

//------------------------------------------------------------------------------
/**
    Get pointer to DirectSound 3D-SoundBuffer properties.
*/
inline
DS3DBUFFER*
nDSound3::GetDS3DProps()
{
    if (this->props3DDirty)
    {
        this->props3DDirty = false;
        this->ds3DProps.vPosition.x = this->transform.M41;
        this->ds3DProps.vPosition.y = this->transform.M42;
        this->ds3DProps.vPosition.z = this->transform.M43;
        this->ds3DProps.vVelocity.x = this->velocity.x;
        this->ds3DProps.vVelocity.y = this->velocity.y;
        this->ds3DProps.vVelocity.z = this->velocity.z;
        this->ds3DProps.dwInsideConeAngle = (DWORD) this->insideConeAngle;
        this->ds3DProps.dwOutsideConeAngle = (DWORD) this->outsideConeAngle;
        this->ds3DProps.vConeOrientation.x = this->transform.M31;
        this->ds3DProps.vConeOrientation.y = this->transform.M32;
        this->ds3DProps.vConeOrientation.z = this->transform.M33;
        this->ds3DProps.lConeOutsideVolume = AsDirectSoundVolume(this->coneOutsideVolume);
        this->ds3DProps.flMinDistance = this->minDist;
        this->ds3DProps.flMaxDistance = this->maxDist;
        this->ds3DProps.dwMode = this->m_dwDS3DPROPSMode;
    }
    return &(this->ds3DProps);
}

//------------------------------------------------------------------------------
/**
*/
inline
nDSoundResource* 
nDSound3::GetDSoundResource()
{
    return this->refSoundResource.get();
}

//------------------------------------------------------------------------------
#endif

