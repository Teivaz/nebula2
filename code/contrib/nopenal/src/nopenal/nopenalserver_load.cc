//------------------------------------------------------------------------------
//  nopenalserver_load.cc
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#include "nopenal/nopenalserver.h"
#include "resource/nresourceserver.h"

//------------------------------------------------------------------------------
/**
*/
nSoundResource*
nOpenALServer::NewSoundResource(const char* rsrcName)
{
    return (nSoundResource*) this->refResourceServer->NewResource("nopenalresource", rsrcName, nResource::SoundResource);
}

//------------------------------------------------------------------------------
/**
*/
nSound3*
nOpenALServer::NewSound()
{
    return (nSound3 *) this->refResourceServer->NewResource("nopenalobj", 0, nResource::SoundInstance);
}

//------------------------------------------------------------------------------
/**
*/
bool
nOpenALServer::createBufSrc()
{
    // create buffer
    alGetError(); /* clear */
    alGenBuffers(1, &buffer[nextBuffer]);
    check_al_error();
    //if(alGetError() != AL_NO_ERROR)
    //{
    //    return false;
    //}

    // create source
    alGetError(); /* clear */
    alGenSources(1, &source[nextSource]);
    check_al_error();
    //if(alGetError() != AL_NO_ERROR)
    //{
    //    return false;
    //}

    return true;
}