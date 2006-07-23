//------------------------------------------------------------------------------
//  ndsoundserver3_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/ndsoundserver3.h"
#include "kernel/nenv.h"
#include "kernel/ntimeserver.h"
#include "resource/nresourceserver.h"
#include "audio3/ndsound3.h"
#include "audio3/nlistener3.h"

nNebulaClass(nDSoundServer3, "audio3::naudioserver3");

//------------------------------------------------------------------------------
/**
*/
nDSoundServer3::nDSoundServer3() :
    soundManager(0),
    refHwnd("/sys/env/hwnd"),
    dsListener(0),
    lastStreamUpdateCheck(0.0),
    noSoundDevice(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDSoundServer3::~nDSoundServer3()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    n_assert(0 == this->soundManager);
    n_assert(0 == this->dsListener);
}

//------------------------------------------------------------------------------
/**
    Open the audio server. This will initialize the CSoundManager object.
*/
bool
nDSoundServer3::Open()
{
    n_assert(!this->isOpen);
    n_assert(0 == this->soundManager);
    n_assert(0 == this->dsListener);
    HRESULT hr;

    // get public window handle, which has been initialized by the gfx subsystem
    HWND hwnd = (HWND) this->refHwnd->GetI();

    // create DSUtil CSoundManager object
    this->soundManager = n_new(CSoundManager);
    n_assert(this->soundManager);

    // initialize the sound manager
    hr = this->soundManager->Initialize(hwnd, DSSCL_PRIORITY);
    if (SUCCEEDED(hr))
    {
        this->noSoundDevice = false;
    }
    else
    {
        this->isOpen = true;
        this->noSoundDevice = true;
        return false;
    }
    hr = this->soundManager->SetPrimaryBufferFormat(2, 22050, 16);
    n_assert(SUCCEEDED(hr));

    // obtain 3D listener interface
    hr = this->soundManager->Get3DListenerInterface(&(this->dsListener));
    n_assert(SUCCEEDED(hr));
    memset(&(this->dsListenerProps), 0, sizeof(this->dsListenerProps));
    this->dsListenerProps.dwSize = sizeof(this->dsListenerProps);

    return nAudioServer3::Open();
}

//------------------------------------------------------------------------------
/**
    Close the audio server. This will delete the CSoundManager object.
*/
void
nDSoundServer3::Close()
{
    n_assert(this->isOpen);

    // release sound resources
    nResourceServer::Instance()->UnloadResources(nResource::SoundResource | nResource::SoundInstance);

    if (this->dsListener)
    {
        this->dsListener->Release();
        this->dsListener = 0;
    }
    if (this->soundManager)
    {
        n_delete(this->soundManager);
        this->soundManager = 0;
    }
    nAudioServer3::Close();
}

//------------------------------------------------------------------------------
/**
    Begin the audio scene.
*/
bool
nDSoundServer3::BeginScene(nTime t)
{
    if (this->noSoundDevice)
    {
        return true;
    }
    return nAudioServer3::BeginScene(t);
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::UpdateListener(const nListener3& l)
{
    if (this->noSoundDevice)
    {
        return;
    }
    n_assert(this->dsListener);

    const matrix44& m = l.GetTransform();
    const vector3& v  = l.GetVelocity();
    this->dsListenerProps.vPosition.x       = m.M41;
    this->dsListenerProps.vPosition.y       = m.M42;
    this->dsListenerProps.vPosition.z       = m.M43;
    this->dsListenerProps.vVelocity.x       = v.x;
    this->dsListenerProps.vVelocity.y       = v.y;
    this->dsListenerProps.vVelocity.z       = v.z;
    this->dsListenerProps.vOrientFront.x    = m.M31;
    this->dsListenerProps.vOrientFront.y    = m.M32;
    this->dsListenerProps.vOrientFront.z    = m.M33;
    this->dsListenerProps.vOrientTop.x      = m.M21;
    this->dsListenerProps.vOrientTop.y      = m.M22;
    this->dsListenerProps.vOrientTop.z      = m.M23;
    this->dsListenerProps.flDistanceFactor  = 1.0f;
    this->dsListenerProps.flRolloffFactor   = l.GetRollOffFactor();
    this->dsListenerProps.flDopplerFactor   = l.GetDopplerFactor();

    HRESULT hr = this->dsListener->SetAllParameters(&(this->dsListenerProps), DS3D_IMMEDIATE);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::StartSound(nSound3* sound)
{
    if (this->noSoundDevice)
    {
        return;
    }
    n_assert(sound);
    sound->Start();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::UpdateSound(nSound3* sound)
{
    if (this->noSoundDevice)
    {
        return;
    }
    n_assert(sound);
    sound->Update();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::StopSound(nSound3* sound)
{
    if (this->noSoundDevice)
    {
        return;
    }
    n_assert(sound);
    sound->Stop();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::EndScene()
{
    if (this->noSoundDevice)
    {
        return;
    }

    // handle stream and volume updates (only every 1/50 seconds)
    nTime curTime = kernelServer->GetTimeServer()->GetTime();
    nTime diff = curTime - this->lastStreamUpdateCheck;

    // check for time exception
    bool timeException = false;
    if (diff <= 0.0)
    {
        timeException = true;
    }
    if ((diff > 0.02) || (timeException))
    {
        this->lastStreamUpdateCheck = curTime;

        nRoot* rsrcPool = nResourceServer::Instance()->GetResourcePool(nResource::SoundResource);
        nRoot* cur;
        for (cur = rsrcPool->GetHead(); cur; cur = cur->GetSucc())
        {
            nDSoundResource* sound = (nDSoundResource*) cur;
            if (sound->GetStreaming())
            {
                CStreamingSound* snd = (CStreamingSound*) sound->GetCSoundPtr();
                if (snd->IsSoundPlaying())
                {
                    if (snd->CheckStreamUpdate())
                    {
                        snd->HandleWaveStreamNotification(sound->GetLooping());
                    }
                }
            }
        }
    }

    // update sound
    int cat;
    for(cat = 0; cat < NumCategorys; cat++)
    {
        if (this->masterVolumeDirty[cat])
        {
            this->masterVolumeDirty[cat] = false;
            nRoot* rsrcPool = nResourceServer::Instance()->GetResourcePool(nResource::SoundInstance);
            nRoot* cur;
            for (cur = rsrcPool->GetHead(); cur; cur = cur->GetSucc())
            {
                nDSound3* snd = (nDSound3*) cur;
                if (snd->GetCategory() == cat && snd->IsPlaying())
                {
                    snd->Update();
                }
            }
        }
    }

    nAudioServer3::EndScene();
}

//------------------------------------------------------------------------------
/**
*/
nSoundResource*
nDSoundServer3::NewSoundResource(const char* rsrcName)
{
    return (nSoundResource*) nResourceServer::Instance()->NewResource("ndsoundresource", rsrcName, nResource::SoundResource);
}

//------------------------------------------------------------------------------
/**
*/
nSound3*
nDSoundServer3::NewSound()
{
    return (nSound3*) nResourceServer::Instance()->NewResource("ndsound3", 0, nResource::SoundInstance);
}

//------------------------------------------------------------------------------
/**
    Return a pointer to the embedded CSoundManager object.
*/
CSoundManager*
nDSoundServer3::GetSoundManager()
{
    n_assert(this->isOpen);
    return this->soundManager;
}
