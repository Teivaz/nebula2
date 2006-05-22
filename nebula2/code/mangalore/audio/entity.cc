//------------------------------------------------------------------------------
//  audio/entity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/entity.h"
#include "audio/server.h"
#include "audio3/naudioserver3.h"

namespace Audio
{
ImplementRtti(Audio::Entity, Foundation::RefCounted);
ImplementFactory(Audio::Entity);

//------------------------------------------------------------------------------
/**
*/
Entity::Entity() :
    active(false),
    sound(0),
    volume(1.0f),
    maxVolume(1.0f),
    fadeOutActive(false),
    fadeOutStarted(0.0),
    fadeOutTime(0.0),
    fistFrameAfterStart(false)
{
}

//------------------------------------------------------------------------------
/**
*/
Entity::~Entity()
{
    if (this->active)
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
    Activates the object. This will load the sound resource.
*/
void
Entity::OnActivate()
{
    n_assert(!this->active);
    n_assert(0 == this->sound);
    n_assert(!this->resourceName.IsEmpty());
    Server* server = Audio::Server::Instance();

    // create a new sound object from an existing sound resource
    this->sound = server->CreateSoundFromResourceName(this->GetResourceName());
    if (this->sound)
    {
        // open the sound resource
        if (!this->sound->IsValid())
        {
            bool success = this->sound->Load();
            n_assert(success);
        }
        this->maxVolume = this->sound->GetVolume();
    }
    else
    {
        this->maxVolume = 0.0f;
    }
    this->volume = maxVolume;
    this->active = true;
}

//------------------------------------------------------------------------------
/**
    Deactivates the object. This will unload the sound resource.
*/
void
Entity::OnDeactivate()
{
    n_assert(this->active);
    if (this->sound)
    {
        if (this->IsPlaying())
        {
            this->Stop();
        }
        this->sound->Unload();
        this->sound->Release();
        this->sound = 0;
    }
    this->active = false;
}

//------------------------------------------------------------------------------
/**
    Start sound playback.
*/
void
Entity::Start()
{
    if (this->sound)
    {
        this->sound->SetVolume(this->volume);
        nAudioServer3::Instance()->StartSound(this->sound);
        this->fistFrameAfterStart = true;
    }
}

//------------------------------------------------------------------------------
/**
    Stop sound playback (mostly only makes sense for looping sounds).
*/
void
Entity::Stop()
{
    if (this->sound)
    {
        nAudioServer3::Instance()->StopSound(this->sound);
    }
}

//------------------------------------------------------------------------------
/**
    Update the audio entity parameters. Call this once per frame.
*/
void
Entity::Update()
{
    if (this->sound)
    {
        // handle fade out
        if (this->IsPlaying())
        {
            if (this->fadeOutActive)
            {
                nTime curTime = Server::Instance()->GetTime();
                float age = float(curTime - this->fadeOutStarted);
                if (age < this->fadeOutTime)
                {
                    float vol = 1.0f - n_saturate(age / float(this->fadeOutTime));
                    this->SetVolume(vol);
                }
                else
                {
                    this->Stop();
                    this->fadeOutActive = false;
                }
            }
            else
            {
                this->sound->SetVolume(this->volume);
            }
            nAudioServer3::Instance()->UpdateSound(this->sound);
        }
        this->fistFrameAfterStart = false;
    }
}

//------------------------------------------------------------------------------
/**
    Start a fadeout. Actual volume manipulation will happen during Update().
*/
void
Entity::FadeOut(nTime fadeOutTime)
{
    if (fadeOutTime > 0.05f)
    {
        this->fadeOutActive = true;
        this->fadeOutTime = fadeOutTime;
        this->fadeOutStarted = Server::Instance()->GetTime();
    }
    else
    {
        this->fadeOutActive = false;
        this->Stop();
    }
}

} // namespace Audio
