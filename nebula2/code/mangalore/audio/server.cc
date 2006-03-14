//------------------------------------------------------------------------------
//  audio/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/naudioserver3.h"
#include "kernel/nfileserver2.h"
#include "audio/waveresource.h"
#include "audio/server.h"
#include "audio/entity.h"
#include "foundation/factory.h"

namespace Audio
{
ImplementRtti(Audio::Server, Foundation::RefCounted);
ImplementFactory(Audio::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() : isOpen(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
    this->defaultListener.create();
    this->soundFxHandler.create();
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);
    n_assert(waveBank == 0);
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the audio subsystem.
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);
    nFileServer2* fileServer = nFileServer2::Instance();

    // setup Nebula path assigns
    nString audioAssign = fileServer->ManglePath("proj:export/audio");
    fileServer->SetAssign("audio", audioAssign.Get());

    // open the Nebula audio server
    if (!nAudioServer3::Instance()->Open())
    {
        n_error("Audio::Server::Open(): failed to open Nebula2 audio server!");
        return false;
    }
    
    // make the default listener current
    this->SetListener(this->defaultListener);

    // register the sound fx handler
    Message::Server::Instance()->RegisterPort(this->soundFxHandler);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the audio subsystem.
*/
void
Server::Close()
{
    n_assert(this->isOpen);
    
    // unregister the sound fx handler
    Message::Server::Instance()->UnregisterPort(this->soundFxHandler);

    // remove all audio entities
    while (this->GetNumEntities() > 0)
    {
        Entity* curEntity = this->GetEntityAt(this->GetNumEntities() - 1);  
        this->RemoveEntity(curEntity);  
    }

    if (this->waveBank.isvalid())
    {
        this->CloseWaveBank();
    }
    this->SetListener(0);
    this->isOpen = false;

    // close Nebula audio server
    nAudioServer3::Instance()->Close();
}

//------------------------------------------------------------------------------
/**
    Create a new audio entity.
*/
Entity*
Server::CreateEntity()
{
	return Audio::Entity::Create();
}

//------------------------------------------------------------------------------
/**
    Attach an audio entity to the server. Attached audio entities will
    have their Update() method called during Trigger().
*/
void
Server::AttachEntity(Entity* e)
{
    n_assert(e);
    e->OnActivate();
    this->entityArray.Append(e);
}

//------------------------------------------------------------------------------
/**
    Remove an audio entity from the server. Removed entities will stop
    playing and will no longer be updated during Trigger().
*/
void
Server::RemoveEntity(Entity* e)
{
    n_assert(e);
    e->OnDeactivate();
    nArray<Ptr<Entity> >::iterator iter = this->entityArray.Find(e);
    n_assert(iter);
    this->entityArray.Erase(iter);
}

//------------------------------------------------------------------------------
/**
    Open a wave bank with the specified file name and make it current.
*/
bool
Server::OpenWaveBank(const nString& name)
{
    n_assert(!this->waveBank.isvalid());
    this->waveBank = WaveBank::Create();
    this->waveBank->SetFilename(name);
    return this->waveBank->Open();
}

//------------------------------------------------------------------------------
/**
    Close and release current wave bank.
*/
void
Server::CloseWaveBank()
{
    n_assert(this->waveBank.isvalid());
    this->waveBank->Close();
    this->waveBank = 0;
}

//------------------------------------------------------------------------------
/**
    Begin audio scene.
*/
void
Server::BeginScene()
{
    nAudioServer3::Instance()->BeginScene(this->time);
}

//------------------------------------------------------------------------------
/**
    End audio scene.
*/
void
Server::EndScene()
{
    // handle pending audio messages
    this->soundFxHandler->HandlePendingMessages();

    // trigger all attached entities
    int num = this->GetNumEntities();
    int i;
    for (i = 0; i < num; i++)
    {
        this->entityArray[i]->Update();
    }

    // commit listener settings
    if (this->curListener != 0)
    {
        this->curListener->Commit();
    }

    // call nebula audio server
    nAudioServer3::Instance()->EndScene();
}

//------------------------------------------------------------------------------
/**
    Create a sound resource from a wavebank resource name. This is
    a private convenience method used by other Audio subsystem classes.
    NOTE: the method can return NULL!

    @param  name    a resource name
    @return         pointer to a new nSound3 object, or 0 if not exists
*/
nSound3*
Server::CreateSoundFromResourceName(const nString& name)
{
    n_assert(waveBank != 0);
    WaveResource* waveResource = this->waveBank->FindResource(name);
    if (waveResource)
    {
        nSound3* orig = waveResource->GetSoundObjectAt(0);
        n_assert(orig);
        nSound3* newSound = nAudioServer3::Instance()->NewSound();
        newSound->SetFilename(orig->GetFilename());
        newSound->CopySoundAttrsFrom(orig);
        newSound->SetVolume(waveResource->GetVolume());
        return newSound;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Set the current listener. Increments the refcount of the
    new listener, and decrements the refcount of any previous listener.
    Note that listener parameters are "rendered" in Server::EndScene().

    @param  listener    pointer to a new listener object (can be 0)
*/
void
Server::SetListener(Listener* listener)
{
    curListener = listener;
}

//------------------------------------------------------------------------------
/**
    Directly play a sound effect.
*/
void
Server::PlaySoundEffect(const nString& fxName, const vector3& pos, const vector3& vel, float volume)
{
    n_assert(fxName.IsValid());

    // configure the nSound3 object
    WaveBank* waveBank = this->GetWaveBank();
    if (waveBank)
    {
        WaveResource* waveResource = waveBank->FindResource(fxName);
        if (waveResource)
        {
            nSound3* sound = waveResource->GetRandomSoundObject();
            n_assert(sound);

            matrix44 transform;
            transform.translate(pos);

            // configure the sound
            sound->SetTransform(transform);
            sound->SetVelocity(vel);
            sound->SetVolume(waveResource->GetVolume() * volume);

            // and fire the sound
            nAudioServer3::Instance()->StartSound(sound);
        }
    }
}

} // namespace Audio