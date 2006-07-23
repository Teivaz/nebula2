//------------------------------------------------------------------------------
//  ndsoundresource_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/ndsoundresource.h"
#include "audio3/ndsoundserver3.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nDSoundResource, "audio3::nsoundresource");

//------------------------------------------------------------------------------
/**
*/
nDSoundResource::nDSoundResource() :
    refSoundServer("/sys/servers/audio"),
    dsSound(0),
    dsStreamingSound(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDSoundResource::~nDSoundResource()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Load the sound resource. This will create a CSound or CStreamingSound
    object, depending on the currently set flags.
*/
bool
nDSoundResource::LoadResource()
{
    n_assert(!this->IsLoaded());
    n_assert(0 == this->dsSound);
    n_assert(0 == this->dsStreamingSound);

    CSoundManager* soundManager = this->refSoundServer->GetSoundManager();
    HRESULT hr;

    DWORD creationFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_LOCDEFER;
    if (!this->ambient)
    {
        creationFlags |= DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE;
    }
    if (!this->streaming)
    {
        // create a static sound object
        hr = soundManager->Create(&(this->dsSound), (LPTSTR) this->GetFilename().Get(), creationFlags, DS3DALG_DEFAULT, this->numTracks);
        if (FAILED(hr))
        {
            n_error("nDSoundServer::LoadResource(): Creating static sound '%s' failed!", this->GetFilename().Get());
            return false;
        }
        n_assert(this->dsSound);
    }
    else
    {
        // create a streaming sound object (with a 128 KByte streaming buffer)
        int numNotifications = 4;
        int blockSize = (1<<17) / numNotifications;

        /*
        if((this->GetFilename().CheckExtension("ogg"))||
           (this->GetFilename().CheckExtension("OGG")))
        {
            hr = soundManager->CreateStreamingOgg(&(this->dsStreamingSound), (LPTSTR) mangledPath.Get(), creationFlags,
                DS3DALG_DEFAULT, numNotifications, blockSize);
        }
        else
        {
            */
            hr = soundManager->CreateStreaming(&(this->dsStreamingSound), (LPTSTR) this->GetFilename().Get(), creationFlags,
                DS3DALG_DEFAULT, numNotifications, blockSize);
//        }

        if (FAILED(hr))
        {
            n_error("nDSoundServer::LoadResource(): Creating streaming sound '%s' failed!", this->GetFilename().Get());
            return false;
        }
        n_assert(this->dsStreamingSound);
    }
    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload everything.
*/
void
nDSoundResource::UnloadResource()
{
    n_assert(this->IsLoaded());
    if (this->dsSound)
    {
        n_delete(this->dsSound);
        this->dsSound = 0;
    }
    if (this->dsStreamingSound)
    {
        n_delete(this->dsStreamingSound);
        this->dsStreamingSound = 0;
    }
    this->SetState(Unloaded);
}
