//------------------------------------------------------------------------------
//  nvideoserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "video/nvideoserver.h"

nNebulaScriptClass(nVideoServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nVideoServer::nVideoServer() :
    isOpen(false),
    isPlaying(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nVideoServer::~nVideoServer()
{
    n_assert(!this->isOpen);
    n_assert(!this->isPlaying);
}

//------------------------------------------------------------------------------
/**
*/
bool
nVideoServer::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nVideoServer::PlayFile(const char* filename)
{
    n_assert(filename);
    n_assert(!this->isPlaying);
    this->isPlaying = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer::Stop()
{
    n_assert(this->isPlaying);
    this->isPlaying = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer::Trigger()
{
    n_assert(this->isOpen);
}

