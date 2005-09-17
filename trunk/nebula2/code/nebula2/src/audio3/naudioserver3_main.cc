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
    curTime(0.0),
	isMuted(false)
{
    n_assert(0 == Singleton);
    Singleton = this;

    this->masterVolume.SetSize(NumCategorys);
    this->masterVolume.Clear(1.0f);
    this->masterVolumeDirty.SetSize(NumCategorys);
    this->masterVolumeDirty.Clear(true);
    this->masterVolumeChangedTime.SetSize(NumCategorys);
    this->masterVolumeChangedTime.Clear(this->curTime);
	this->masterVolumeMuted.SetSize(NumCategorys);
}

//------------------------------------------------------------------------------
/**
*/
nAudioServer3::~nAudioServer3()
{
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


//------------------------------------------------------------------------------
/**
*/
nString 
nAudioServer3::CategoryToString(Category cat)
{
    switch(cat)
    {
        case Effect: return "effect";
        case Music:  return "music";
        case Speech: return "speech";
        case Ambient: return "ambient";
        default:
            n_error("nAudioServer3: Invalid Category: %i!", cat);
            return "";
    }
}

//------------------------------------------------------------------------------
/**
*/
nAudioServer3::Category 
nAudioServer3::StringToCategory(const nString& s)
{
    if ("effect" == s)          return Effect;
    else if ("music" == s)      return Music;
    else if ("speech" == s)     return Speech;
    else if ("ambient" == s)    return Ambient;
    else
    {
        n_error("nAudioServer3: Invalid category string '%s'.\n", s.Get());
        return InvalidCategory;
    }
}

//------------------------------------------------------------------------------
/**
    Calls Update() on all playing sounds. This can be used to immediately
    commit changed without having to wait until EndScene().
*/
void
nAudioServer3::UpdateAllSounds()
{
    nRoot* rsrcPool = nResourceServer::Instance()->GetResourcePool(nResource::SoundInstance);
    nRoot* cur;
    for (cur = rsrcPool->GetHead(); cur; cur = cur->GetSucc())
    {
        nSound3* snd = (nSound3*) cur;
        if (snd->IsPlaying())
        {
            snd->Update();
        }
    }    
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::Mute()
{
	n_assert(!this->isMuted);
	int i;
	for (i = 0; i < NumCategorys; i++)
	{
		this->masterVolumeMuted[i] = this->GetMasterVolume((Category)i);
		this->SetMasterVolume((Category)i , 0.0f );
	}

	this->isMuted = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::Unmute()
{
	n_assert(this->isMuted);
	int i;
	for (i = 0; i < NumCategorys; i++)
	{
		this->SetMasterVolume((Category)i, this->masterVolumeMuted[i]);
	}

	this->isMuted = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAudioServer3::IsMuted() const
{
	return this->isMuted;
}
