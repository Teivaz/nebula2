//------------------------------------------------------------------------------
//  application/playvideohandler.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/playvideohandler.h"

#include "video/nvideoserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "managers/timemanager.h"
#include "game/time/systemtimesource.h"

namespace Application
{
ImplementRtti(Application::PlayVideoHandler, Application::StateHandler);
ImplementFactory(Application::PlayVideoHandler);

using namespace Managers;
using namespace Game;
//------------------------------------------------------------------------------
/**
*/
PlayVideoHandler::PlayVideoHandler() :
    enableScaling(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PlayVideoHandler::~PlayVideoHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
PlayVideoHandler::OnStateEnter(const nString& prevState)
{
    TimeManager::Instance()->ResetAll();
    TimeManager::Instance()->Update();
    StateHandler::OnStateEnter(prevState);
    n_assert(!this->videoFileName.IsEmpty());
    nVideoServer::Instance()->SetEnableScaling(this->GetEnableScaling());
    nVideoServer::Instance()->PlayFile(this->videoFileName.Get());
}

//------------------------------------------------------------------------------
/**
*/
nString
PlayVideoHandler::OnFrame()
{
    nString returnState = App::Instance()->GetCurrentState();

    TimeManager::Instance()->Update();
    nInputServer* inputServer = nInputServer::Instance();
    nVideoServer* videoServer = nVideoServer::Instance();
    nGfxServer2* gfxServer    = nGfxServer2::Instance();

    // distribute timestamps to interested subsystems
    if (gfxServer->Trigger())
    {
        // trigger input and video servers
        videoServer->Trigger();
        inputServer->Trigger(SystemTimeSource::Instance()->GetTime());

	    // check if video has finished
	    if (!videoServer->IsPlaying())
	    {
            returnState = this->nextState;
        }
    }
    else
    {
        // handle Alt-F4 pressed
        returnState = "Exit";
    }

    // flush input events
    inputServer->FlushEvents();

    // trigger kernel server at end of frame
    nKernelServer::Instance()->Trigger();

    return returnState;
}

} // namespace Application
