//------------------------------------------------------------------------------
//  nopenalhelper.cc
//
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#include "nopenaldemo/nopenalhelper.h"

//------------------------------------------------------------------------------
/**
*/
nOpenALHelper::nOpenALHelper() :
    soundObjCounts(0)
{
}

//------------------------------------------------------------------------------
/**
*/
nOpenALHelper::~nOpenALHelper()
{
}

//------------------------------------------------------------------------------
/**
    @param pos
    @param angle
*/
void
nOpenALHelper::ListenerPosition(float* pos, float* angle)
{
    alListenerfv(AL_POSITION, pos);
    alListenerfv(AL_ORIENTATION, angle);
}

//------------------------------------------------------------------------------
/**
    loads a file into a buffer and source

    @param rsrcname resource name.
    @param filename file name which to be loaded.

    @return return nOpenAL sound object which loaded.
*/
nOpenALObj*
nOpenALHelper::LoadFile(char *rsrcname, char *filename)
{
    soundObjs[soundObjCounts] = (nOpenALObj*)nOpenALServer::Instance()->NewSound();

    soundObjs[soundObjCounts]->SetName(rsrcname);
    soundObjs[soundObjCounts]->SetFilename(filename);
    soundObjs[soundObjCounts]->Load();

    soundObjCounts++;

    return soundObjs[soundObjCounts-1];
}

//------------------------------------------------------------------------------
/**
    loads a file into a buffer and source, then plays it
*/
nOpenALObj*
nOpenALHelper::PlayFile(char *rsrcname, char *filename)
{
    nOpenALObj *poalobj = LoadFile(rsrcname, filename);

    nOpenALServer::Instance()->StartSound(poalobj);

    return poalobj;
}

//------------------------------------------------------------------------------
/**
    @param oalobj sound object to set the postion
    @param position the position to set to the sound object.
*/
void
nOpenALHelper::SetSourcePosition(nOpenALObj* oalobj, float *position)
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
    @param oalobj sound object to set the velocity
    @param velocity the velocity to set to the sound object.
*/
void
nOpenALHelper::SetSourceVelocity(nOpenALObj* oalobj, float *velocity)
{
    vector3 v( velocity[0], velocity[1], velocity[2] );
    oalobj->SetVelocity(v);
    oalobj->Update();
}

//------------------------------------------------------------------------------
/**
    
*/
int
nOpenALHelper::IncrementEnv()
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
