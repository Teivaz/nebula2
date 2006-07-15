//------------------------------------------------------------------------------
//  nopenalserver_main.cc
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#include "nopenal/nopenalobj.h"
#include "nopenal/nopenalserver.h"
#include "nopenal/nopenalresource.h"

nNebulaClass(nOpenALObj, "nsound3");

//------------------------------------------------------------------------------
/**
*/
nOpenALObj::nOpenALObj() :
    refSoundServer("/sys/servers/audio"),
    m_handle(-1)
{
}

//------------------------------------------------------------------------------
/**
*/
nOpenALObj::~nOpenALObj()
{
    if (this->IsValid())
    {
        this->Stop();
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Creates a shared sound resource, and initializes it.
*/
bool
nOpenALObj::LoadResource()
{
    n_assert(!this->IsValid());
    n_assert(!this->refSoundResource.isvalid());

    // create a sound resource object
    nOpenALResource* rsrc = (nOpenALResource*) this->refSoundServer->NewSoundResource(this->GetName());
    n_assert(rsrc);

    // if sound resource not opened yet, do it
    if (!rsrc->IsValid())
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

    this->m_handle = rsrc->getHANDLE();
    this->refSoundResource = rsrc;
    //this->SetValid(true);
    this->SetState(Valid);

    return true;
}

//------------------------------------------------------------------------------
/**
    Unloads the sound resource object.
*/
void
nOpenALObj::UnloadResource()
{
    n_assert(this->IsValid());
    n_assert(this->refSoundResource.isvalid());
    this->refSoundResource->Release();
    this->refSoundResource.invalidate();
    //this->SetValid(false);
    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
*/
void
nOpenALObj::Start()
{
    n_assert(this->IsValid());
    unsigned int source = this->refSoundServer->getSource(m_handle);
    alSourcePlay(source);
}

//------------------------------------------------------------------------------
/**
*/
void
nOpenALObj::Stop()
{
    unsigned int source = this->refSoundServer->getSource(m_handle);
    alSourceStop(source);
}

//------------------------------------------------------------------------------
/**
*/
void
nOpenALObj::Update()
{
    unsigned int source = this->refSoundServer->getSource(m_handle);

    vector3 position( transform.M41, transform.M42, transform.M43 );

    alSourcefv(source, AL_POSITION, (ALfloat *)&position);
    alSourcefv(source, AL_VELOCITY, (ALfloat *)&velocity);
    alSourcei(source, AL_LOOPING, this->GetLooping());
}

//------------------------------------------------------------------------------
/**
*/
void
nOpenALObj::SetVolume(float v)
{
    unsigned int source = this->refSoundServer->getSource(m_handle);
    this->volume = v;
    this->volumeDirty = true;
    alSourcef(source, AL_GAIN, this->volume);
}

//------------------------------------------------------------------------------
/**
*/
float
nOpenALObj::GetVolume() const
{
    return this->volume;
}
