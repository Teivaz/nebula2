//------------------------------------------------------------------------------
//  nopenal_core.cc
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#include "nopenal_demo/nopenal_core.h"

//------------------------------------------------------------------------------
/**
*/
nOpenALCore::nOpenALCore() :
    refAudioServer("/sys/servers/audio"),
    soundObjCounts(0)
{
}

//------------------------------------------------------------------------------
/**
*/
nOpenALCore::~nOpenALCore()
{
    this->refAudioServer->Close();
}

//------------------------------------------------------------------------------
/**
    Initialize OpenAL Core
*/
void
nOpenALCore::Init()
{
    n_verify(this->refAudioServer->Open());
}

//------------------------------------------------------------------------------
/**
    ListenerPostition -- update listener's position and direction
*/
void
nOpenALCore::ListenerPosition( float* position, float* angle )
{
    alListenerfv(AL_POSITION, position);
    alListenerfv(AL_ORIENTATION, angle);
}

//------------------------------------------------------------------------------
/**
    LoadFile -- loads a file into a buffer and source
*/
nOpenALObj*
nOpenALCore::LoadFile( char *rsrcname, char *filename )
{
    soundObjs[soundObjCounts] = (nOpenALObj *)this->refAudioServer->NewSound();

    soundObjs[soundObjCounts]->SetName(rsrcname);
    soundObjs[soundObjCounts]->SetFilename(filename);
    soundObjs[soundObjCounts]->Load();

    soundObjCounts++;

    return soundObjs[soundObjCounts-1];
}

//------------------------------------------------------------------------------
/**
    Playfile -- loads a file into a buffer and source, then plays it
*/
nOpenALObj*
nOpenALCore::PlayFile( char *rsrcname, char *filename )
{
    nOpenALObj *poalobj = LoadFile(rsrcname, filename);

    this->refAudioServer->StartSound(poalobj);

    return poalobj;
}

//------------------------------------------------------------------------------
/**
    SetSourcePosition
*/
void
nOpenALCore::SetSourcePosition( nOpenALObj* oalobj, float *position )
{
    matrix44 m;
    m.ident();
    m.M41 = position[0];
    m.M42 = position[1];
    m.M43 = position[2];
    oalobj->SetTransform(m);
    oalobj->Update();
}

//------------------------------------------------------------------------------
/**
    SetSourceVelocity
*/
void
nOpenALCore::SetSourceVelocity( nOpenALObj* oalobj, float *velocity )
{
    vector3 v( velocity[0], velocity[1], velocity[2] );
    oalobj->SetVelocity(v);
    oalobj->Update();
}

//------------------------------------------------------------------------------
/**
    IncrementEnv -- steps through the global environment presets if available
*/
int
nOpenALCore::IncrementEnv()
{
#ifdef USE_EAX
    // increment EAX environment if EAX is available
    EAXSet pfPropSet;
    EAXGet pfPropGet;
    long lEAXVal;
    static unsigned long ulEAXEnv = 0;
    if (EAXlevel != 0)
    {
        pfPropSet = (EAXSet) alGetProcAddress((ALubyte *)"EAXSet");
        if (pfPropSet != NULL)
        {
            lEAXVal = -10000;
            pfPropSet(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOM, 0, &lEAXVal, sizeof(long));
            ulEAXEnv += 1;
            if (ulEAXEnv >= EAX_ENVIRONMENT_COUNT) { ulEAXEnv = EAX_ENVIRONMENT_GENERIC; }
            pfPropSet(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENT, 0, &ulEAXEnv, sizeof(unsigned long));
        }
    }
    return (int) ulEAXEnv;
#else // not using EAX
    return 0;
#endif // USE_EAX
}
