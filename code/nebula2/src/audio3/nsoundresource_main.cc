//------------------------------------------------------------------------------
//  nsoundresource_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/nsoundresource.h"

nNebulaClass(nSoundResource, "nresource");

//------------------------------------------------------------------------------
/**
*/
nSoundResource::nSoundResource() :
    numTracks(5),
    ambient(false),
    streaming(false),
    looping(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSoundResource::~nSoundResource()
{
    if (!this->IsUnloaded())
    {
        this->Unload();
    }
}
