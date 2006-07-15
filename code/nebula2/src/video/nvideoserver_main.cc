//------------------------------------------------------------------------------
//  nvideoserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "video/nvideoserver.h"

nNebulaScriptClass(nVideoServer, "nroot");

nVideoServer* nVideoServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nVideoServer::nVideoServer() :
    isOpen(false),
    isPlaying(false),
    scalingEnabled(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nVideoServer::~nVideoServer()
{
    n_assert(!this->isOpen);
    n_assert(!this->isPlaying);
    n_assert(Singleton);
    Singleton = 0;
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

//------------------------------------------------------------------------------
/**
*/
nVideoPlayer*
nVideoServer::NewVideoPlayer(nString name)
{
//    n_assert(this->isOpen);
    return 0;
}

//------------------------------------------------------------------------------
/**
   delete video player
*/
void   
nVideoServer::DeleteVideoPlayer(nVideoPlayer* player)
{
};
