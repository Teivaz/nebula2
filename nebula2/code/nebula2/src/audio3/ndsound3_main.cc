//------------------------------------------------------------------------------
//  ndsound3_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/ndsound3.h"
#include "audio3/ndsoundserver3.h"
#include "audio3/ndsoundresource.h"

nNebulaClass(nDSound3, "nsound3");

//------------------------------------------------------------------------------
/**
*/
nDSound3::nDSound3() :
    refSoundServer("/sys/servers/audio"),
    dsVolume(0),
    soundIndex(-1)
{
    memset(&(this->ds3DProps), 0, sizeof(this->ds3DProps));
    this->ds3DProps.dwSize = sizeof(this->ds3DProps);
}

//------------------------------------------------------------------------------
/**
*/
nDSound3::~nDSound3()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Creates a shared sound resource, and initializes it.
*/
bool
nDSound3::LoadResource()
{
    n_assert(!this->IsLoaded());
    n_assert(!this->refSoundResource.isvalid());

    // create a sound resource object
    nDSoundResource* rsrc = (nDSoundResource*) this->refSoundServer->NewSoundResource(this->GetFilename().Get());
    n_assert(rsrc);

    // if sound resource not opened yet, do it
    if (!rsrc->IsLoaded())
    {
        rsrc->SetFilename(this->GetFilename());
        rsrc->SetNumTracks(this->GetNumTracks());
        rsrc->SetAmbient(this->GetAmbient());
        rsrc->SetStreaming(this->GetStreaming());
        rsrc->SetLooping(this->GetLooping());
        bool success = rsrc->Load();
        if (!success)
        {
            rsrc->Release();
            return false;
        }
    }
    this->refSoundResource = rsrc;
    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unloads the sound resource object.
*/
void
nDSound3::UnloadResource()
{
    n_assert(this->IsLoaded());
    n_assert(this->refSoundResource.isvalid());
    this->refSoundResource->Release();
    this->refSoundResource.invalidate();
    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::Start()
{
    HRESULT hr;
    CSound* snd = this->GetCSoundPtr();
    n_assert(snd);
    LONG dsVolume = this->GetDSVolume();
    DS3DBUFFER* ds3DProps = this->GetDS3DProps();

    // a streaming sound?
    if (this->GetStreaming())
    {
        // reset buffer and fill with new data
        CStreamingSound* streamingSound = (CStreamingSound*) snd;
        streamingSound->Reset();
        LPDIRECTSOUNDBUFFER dsBuffer = streamingSound->GetBuffer(0);
        streamingSound->FillBufferWithSound(dsBuffer, this->GetLooping());
        this->soundIndex = 0;
    }

    // playback flags
    int flags = 0;
    if (this->GetLooping() || this->GetStreaming())
    {
        flags |= DSBPLAY_LOOPING;
    }
    if (this->GetAmbient())
    {
        // play as 2D sound
        hr = snd->Play(this->GetPriority(), flags, dsVolume, 0, 0, this->soundIndex);
        if (FAILED(hr))
        {
            n_printf("nDSoundServer3: failed to start 2D sound '%s'\n", this->GetFilename());
        }
    }
    else
    {
        // play as 3D sound
        flags |= DSBPLAY_TERMINATEBY_PRIORITY | DSBPLAY_TERMINATEBY_DISTANCE;
        hr = snd->Play3D(ds3DProps, this->GetPriority(), flags, dsVolume, 0, this->soundIndex);
        if (FAILED(hr))
        {
            n_printf("nDSoundServer3: failed to start 3D sound '%s'\n", this->GetFilename());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::Stop()
{
    if (this->soundIndex != -1)
    {
        CSound* snd = this->GetCSoundPtr();
        n_assert(snd);
        n_assert(this->soundIndex >= 0);
        snd->Stop(this->soundIndex);
        this->soundIndex = -1;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::Update()
{
    if (this->soundIndex != -1)
    {
        if (this->GetStreaming())
        {
            CStreamingSound* streamingSound = (CStreamingSound*) this->GetCSoundPtr();
            n_assert(streamingSound);
            if (streamingSound->IsSoundPlaying())
            {
                // update the volume
                LPDIRECTSOUNDBUFFER dsBuffer = streamingSound->GetBuffer(0);
                dsBuffer->SetVolume(this->GetDSVolume());
            }
        }
        else
        {
            // FIXME!
            // CSound needs update method!
            CSound* snd = this->GetCSoundPtr();
            n_assert(snd);
            if (snd->IsSoundPlaying())
            {
                // update the volume
                LPDIRECTSOUNDBUFFER dsBuffer = snd->GetBuffer(this->soundIndex);
                dsBuffer->SetVolume(this->GetDSVolume());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nDSound3::IsPlaying()
{
    if (this->soundIndex != -1)
    {
        if (this->GetStreaming())
        {
            CStreamingSound* snd = (CStreamingSound*) this->GetCSoundPtr();
            return (0 != snd->IsSoundPlaying());
        }
        else
        {
            CSound* snd = this->GetCSoundPtr();
            return (0 != snd->IsSoundPlaying(this->soundIndex));
        }
    }
    else
    {
        return false;
    }
}
