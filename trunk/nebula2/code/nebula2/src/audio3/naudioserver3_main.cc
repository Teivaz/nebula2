//------------------------------------------------------------------------------
//  naudioserver3_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/naudioserver3.h"

nNebulaClass(nAudioServer3, "nroot");

//------------------------------------------------------------------------------
/**
*/
nAudioServer3::nAudioServer3() :
    isOpen(false),
    inBeginScene(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAudioServer3::~nAudioServer3()
{
    n_assert(!this->isOpen);
}

//------------------------------------------------------------------------------
/**
*/
bool
nAudioServer3::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAudioServer3::BeginScene()
{
    n_assert(this->isOpen);
    n_assert(!this->inBeginScene);
    this->inBeginScene = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::EndScene()
{
    n_assert(this->isOpen);
    n_assert(this->inBeginScene);
    this->inBeginScene = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::UpdateListener(const nListener3& /*l*/)
{
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::StartSound(nSound3* s)
{
    n_assert(s);
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::UpdateSound(nSound3* s)
{
    n_assert(s);
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer3::StopSound(nSound3* s)
{
    n_assert(s);
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
nSoundResource*
nAudioServer3::NewSoundResource(const char* /*rsrcName*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nSound3*
nAudioServer3::NewSound()
{
    return 0;
}
