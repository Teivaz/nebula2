#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Audio::Server

    The Audio::Server object is the central object of the audio subsystem.
    
    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "audio/wavebank.h"
#include "audio/listener.h"
#include "audio3/nsound3.h"
#include "audio/soundfxhandler.h"

//------------------------------------------------------------------------------
namespace Audio
{
class Entity;

class Server : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get instance pointer
    static Server* Instance();
    /// initialize the audio subsystem
    bool Open();
    /// close the audio subsystem
    void Close();
    /// open wavebank of the specified name and make current
    bool OpenWaveBank(const nString& name);
    /// close the current wave bank
    void CloseWaveBank();
    /// get the current wavebank
    WaveBank* GetWaveBank() const;
    /// set current time
    void SetTime(nTime t);
    /// get current time
    nTime GetTime() const;
    /// begin the audio scene
    void BeginScene();
    /// end the audio scene
    void EndScene();
    /// set the current listener (incrs refcount)
    void SetListener(Listener* listener);
    /// get the current listener
    Listener* GetListener() const;
    /// get pointer to sound fx handler
    SoundFxHandler* GetSoundFxHandler();
    /// directly play a sound effect
    void PlaySoundEffect(const nString& fxName, const vector3& pos, const vector3& vel, float volume);
    /// create an audio entity
    Entity* CreateEntity();
    /// attach an audio entity
    void AttachEntity(Entity* e);
    /// remove an audio entity
    void RemoveEntity(Entity* e);
    /// return number of audio entities
    int GetNumEntities() const;
    /// get pointer to audio entity at index
    Entity* GetEntityAt(int index) const;

private:
    static Server* Singleton;
    friend class Entity;

    /// create a sound object from a wavebank resource name
    nSound3* CreateSoundFromResourceName(const nString& name);

    bool isOpen;
    nTime time;
    Ptr<WaveBank> waveBank;
    Ptr<Listener> defaultListener;
    Ptr<Listener> curListener;
    Ptr<SoundFxHandler> soundFxHandler;
    nArray<Ptr<Entity> > entityArray;
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
int
Server::GetNumEntities() const
{
    return this->entityArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
Entity*
Server::GetEntityAt(int index) const
{
    return this->entityArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
SoundFxHandler*
Server::GetSoundFxHandler()
{
    return this->soundFxHandler;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetTime(nTime t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Server::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
    Get pointer to current wavebank.

    @return     pointer to current wavebank (can be 0)
*/
inline
WaveBank*
Server::GetWaveBank() const
{
    return this->waveBank.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Returns a pointer to the current listener.

    @return     pointer to current listener object
*/
inline
Listener*
Server::GetListener() const
{
    if (curListener != 0)
    {
        return this->curListener;
    }

    return 0;
}

}; // namespace Audio
//------------------------------------------------------------------------------
#endif
