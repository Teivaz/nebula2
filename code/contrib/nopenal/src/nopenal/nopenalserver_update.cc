//------------------------------------------------------------------------------
//  nopenalserver_update.cc
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#include "nopenal/nopenalserver.h"
#include "resource/nresourceserver.h"
#include "nopenal/nopenalobj.h"
#include "kernel/ntimeserver.h"
#include "audio3/nlistener3.h"

//------------------------------------------------------------------------------
/**
    Begin the audio scene.
*/
bool
nOpenALServer::BeginScene(nTime time)
{
    return nAudioServer3::BeginScene(time);
}

//------------------------------------------------------------------------------
/**
*/
void
nOpenALServer::UpdateListener(const nListener3& l)
{
    const matrix44& m = l.GetTransform();
    const vector3& v  = l.GetVelocity();

    ALfloat listenerPos[]={0.0,0.0,0.0};
    ALfloat listenerVel[]={0.0,0.0,0.0};
    ALfloat listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0};  // Listener facing into the screen

    listenerPos[0] = m.M41;
    listenerPos[1] = m.M42;
    listenerPos[2] = m.M43;
    listenerVel[0] = v.x;
    listenerVel[1] = v.y;
    listenerVel[2] = v.z;
    listenerOri[0] = -m.M31;
    listenerOri[1] = -m.M32;
    listenerOri[2] = -m.M33;
    listenerOri[3] = m.M21;
    listenerOri[4] = m.M22;
    listenerOri[5] = m.M23;

    int error;

    // Position ...
    alListenerfv(AL_POSITION,listenerPos);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        n_printf("alListenerfv POSITION : %s\n", alGetString(error));
        return;
    }

    // Velocity ...
    alListenerfv(AL_VELOCITY,listenerVel);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        n_printf("alListenerfv VELOCITY : %s\n", alGetString(error));
        return;
    }

    // Orientation ...
    alListenerfv(AL_ORIENTATION,listenerOri);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        n_printf("alListenerfv ORIENTATION : %s\n", alGetString(error));
        return;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nOpenALServer::StartSound(nSound3* sound)
{
    n_assert(sound);
    sound->Start();
}

//------------------------------------------------------------------------------
/**
*/
void
nOpenALServer::UpdateSound(nSound3* sound)
{
    n_assert(sound);
    sound->Update();
}

//------------------------------------------------------------------------------
/**
*/
void
nOpenALServer::StopSound(nSound3* sound)
{
    n_assert(sound);
    sound->Stop();
}

//------------------------------------------------------------------------------
/**
*/
void
nOpenALServer::EndScene()
{
    // handle stream updates (only every 1/20 seconds)
    nTime curTime = kernelServer->GetTimeServer()->GetTime();
    nTime diff = curTime - this->lastStreamUpdateCheck;
    if (diff > 0.05)
    {
        this->lastStreamUpdateCheck = curTime;

        //nRoot* rsrcPool = this->refResourceServer->GetResourcePool(nResource::SoundResource);
        //nRoot* cur;
        //for (cur = rsrcPool->GetHead(); cur; cur = cur->GetSucc())
        //{
        //    nSoundResource* sound = (nDSoundResource*) cur;
        //    if (sound->GetStreaming())
        //    {
        //        nSound3 *snd = sound->getSound3();
        //        if (snd->IsSoundPlaying())
        //        {
        //            if (snd->CheckStreamUpdate())
        //            {
        //                snd->HandleWaveStreamNotification(sound->GetLooping());
        //            }
        //        }
        //    }
        //}
    }
    nAudioServer3::EndScene();
}
