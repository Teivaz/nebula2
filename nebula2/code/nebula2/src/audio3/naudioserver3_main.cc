//------------------------------------------------------------------------------
//  naudioserver3_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/naudioserver3.h"
#include "audio3/nsound3.h"
#include "resource/nresourceserver.h"

nNebulaScriptClass(nAudioServer3, "nroot");
nAudioServer3* nAudioServer3::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nAudioServer3::nAudioServer3() :
    isOpen(false),
    inBeginScene(false),
    masterVolume(1.0f),
    masterVolumeDirty(true),
    curTime(0.0),
    masterVolumeChangedTime(0.0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nAudioServer3::~nAudioServer3()
{
    if (this->isOpen)
    {
        this->Close();
    }
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAudioServer3::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::Reset()
{
    nRoot* rsrcPool = nResourceServer::Instance()->GetResourcePool(nResource::SoundInstance);
    nRoot* cur;
    for (cur = rsrcPool->GetHead(); cur; cur = cur->GetSucc())
    {
        nSound3* snd = (nSound3*) cur;
        if (snd->IsPlaying())
        {
            snd->Stop();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nAudioServer3::BeginScene(nTime t)
{
    n_assert(this->isOpen);
    n_assert(!this->inBeginScene);
    this->curTime = t;
    this->inBeginScene = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::EndScene()
{
    n_assert(this->isOpen);
    n_assert(this->inBeginScene);
    this->inBeginScene = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::UpdateListener(const nListener3& /*l*/)
{
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::StartSound(nSound3* s)
{
    n_assert(s);
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::UpdateSound(nSound3* s)
{
    n_assert(s);
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::StopSound(nSound3* s)
{
    n_assert(s);
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
nSoundResource*
nAudioServer3::NewSoundResource(const char* /*rsrcName*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nSound3*
nAudioServer3::NewSound()
{
    return 0;
}
