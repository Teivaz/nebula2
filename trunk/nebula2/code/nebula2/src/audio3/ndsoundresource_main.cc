//------------------------------------------------------------------------------
//  ndsoundresource_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/ndsoundresource.h"
#include "audio3/ndsoundserver3.h"
#include "kernel/nfileserver2.h"
#include "audio3/ndsound3.h"

nNebulaClass(nDSoundResource, "nsoundresource");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    ndsoundresource

    @cppclass
    nDSoundResource
    
    @superclass
    nsoundresource
    
    @classinfo
    Docs needed.
*/


//------------------------------------------------------------------------------
/**
*/
nDSoundResource::nDSoundResource() :
    refSoundServer("/sys/servers/audio"),
    dsSound(0)
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
    n_assert(0 != this->dsSound);

    nFileServer2* fileServer = kernelServer->GetFileServer();

    // get mangled path name
    nString mangledPath = fileServer->ManglePath(this->GetFilename().Get());
    
    DWORD creationFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_LOCDEFER;
    if (!this->ambient)
    {
        creationFlags |= DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE;
    }
    if (!this->streaming)
    {
        // create a static sound object
        if (refSoundServer->Create(&(this->dsSound), (LPTSTR)mangledPath.Get(), creationFlags, DS3DALG_DEFAULT, this->numTracks) == false)
        {
            n_printf("nDSoundServer::LoadResource(): Creating static sound '%s' failed!\n", mangledPath.Get());
            return false;
        }
        n_assert(this->dsSound);
    }
    else
    {
        // create a streaming sound object (with a 64 KByte streaming buffer)
        int numNotifications = 2;
        int blockSize = (1<<18) / numNotifications;
        if (refSoundServer->CreateStreaming(&(this->dsSound), (LPTSTR)mangledPath.Get(), creationFlags, DS3DALG_DEFAULT, numNotifications, blockSize) == false)
        {
            n_printf("nDSoundServer::LoadResource(): Creating streaming sound '%s' failed!\n", mangledPath.Get());
            return false;
        }
        n_assert(this->dsSound);
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
    //if (this->dsSound)
    //{
    //    delete this->dsSound;
    //    this->dsSound = 0;
    //}
    //if (this->dsStreamingSound)
    //{
    //    delete this->dsStreamingSound;
    //    this->dsStreamingSound = 0;
    //}
    this->SetState(Unloaded);
}
