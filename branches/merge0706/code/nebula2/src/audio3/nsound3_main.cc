//------------------------------------------------------------------------------
//  nsound3_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/nsound3.h"

nNebulaClass(nSound3, "nresource");

//------------------------------------------------------------------------------
/**
    - 17-May-04     floh    bugfix: missing cone member initializations
*/
nSound3::nSound3() :
    numTracks(5),
    ambient(false),
    streaming(false),
    looping(false),
    priority(0),
    volume(1.0f),
    minDist(10.0f),
    maxDist(100.0f),
    insideConeAngle(0),
    outsideConeAngle(360),
    coneOutsideVolume(1.0f),
    category(nAudioServer3::Effect),
    volumeDirty(true),
    props3DDirty(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSound3::~nSound3()
{
    if (!this->IsUnloaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSound3::Start()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nSound3::Stop()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nSound3::Update()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nSound3::IsPlaying()
{
    return false;
}



