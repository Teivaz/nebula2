//------------------------------------------------------------------------------
//  ncutsceneappstate_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/ncutsceneappstate.h"
#include "video/nvideoserver.h"
#include "application/napplication.h"
#include "input/ninputserver.h"

nNebulaClass(nCutSceneAppState, "nappstate");

//------------------------------------------------------------------------------
/**
*/
nCutSceneAppState::nCutSceneAppState() :
    enableScaling(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCutSceneAppState::~nCutSceneAppState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nCutSceneAppState::OnStateEnter(const nString& prevState)
{
    n_assert(!this->videoFilename.IsEmpty());

    // launch video through the Nebula video server
    nVideoServer* videoServer = nVideoServer::Instance();
    if (videoServer->IsPlaying())
    {
        videoServer->Stop();
    }
    videoServer->SetEnableScaling(this->enableScaling);
    videoServer->PlayFile(this->videoFilename.Get());
}

//------------------------------------------------------------------------------
/**
*/
void
nCutSceneAppState::OnStateLeave(const nString& nextState)
{
    nVideoServer* videoServer = nVideoServer::Instance();
    if (videoServer->IsPlaying())
    {
        videoServer->Stop();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCutSceneAppState::OnFrame()
{
    n_assert(!this->nextState.IsEmpty());
    nVideoServer* videoServer = nVideoServer::Instance();

    if (videoServer->IsPlaying())
    {
        // check input server for key press for early termination
        nInputServer* inputServer = nInputServer::Instance();
        if (inputServer->GetButton("esc") || inputServer->GetButton("space"))
        {
            this->app->SetState(this->GetNextState());
        }
    }
    else
    {
        // video playback has finished, move on to next app state
        this->app->SetState(this->GetNextState());
    }
}
