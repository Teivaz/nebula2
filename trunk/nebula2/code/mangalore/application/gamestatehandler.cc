//------------------------------------------------------------------------------
//  application/gamestatehandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/gamestatehandler.h"
#include "kernel/nscriptserver.h"
#include "misc/nconserver.h"
#include "gfx2/ngfxserver2.h"
#include "particle/nparticleserver.h"
#include "particle/nparticleserver2.h"
#include "gui/nguiserver.h"
#include "video/nvideoserver.h"
#include "input/ninputserver.h"
#include "navigation/server.h"
#include "managers/setupmanager.h"
#include "managers/timemanager.h"
#include "managers/focusmanager.h"
#include "managers/savegamemanager.h"

namespace Application
{
ImplementRtti(Application::GameStateHandler, Application::StateHandler);
ImplementFactory(Application::GameStateHandler);

using namespace Managers;

//------------------------------------------------------------------------------
/**
*/
GameStateHandler::GameStateHandler() :
    setupMode(EmptyWorld),
    exitState("Exit"),
    physicsVisualizationEnabled(false),
    fovVisualization(false),
    gameEntityVisualizationEnabled(false)
{
    PROFILER_INIT(this->profCompleteFrame, "profMangaCompleteFrame");
    PROFILER_INIT(this->profParticleUpdates, "profMangaParticleUpdates");
    PROFILER_INIT(this->profRender, "profMangaRender");
}

//------------------------------------------------------------------------------
/**
*/
GameStateHandler::~GameStateHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
GameStateHandler::OnStateEnter(const nString& prevState)
{
    TimeManager::Instance()->ResetAll();
    TimeManager::Instance()->Update();

    this->physicsVisualizationEnabled = false;
    this->graphicsVisualizationEnabled = false;

    SetupManager* setupManager = SetupManager::Instance();
    SaveGameManager* saveGameManager = SaveGameManager::Instance();

    if (EmptyWorld == this->setupMode)
    {
        setupManager->SetupEmptyWorld();
    }
    else if (NewGame == this->setupMode)
    {
        // use override start level, or the startup level from the world database?
        saveGameManager->SetStartLevelOverride(this->GetLevelName());
        saveGameManager->NewGame();
    }
    else if (ContinueGame == this->setupMode)
    {
        saveGameManager->ContinueGame();
    }
    else if (LoadLevel == this->setupMode)
    {
        // show progress bar UI
        Loader::Server* loaderServer = Loader::Server::Instance();
        loaderServer->OpenProgressIndicator();

        loaderServer->SetProgressText("On Load Before...");
        loaderServer->UpdateProgressDisplay();
        setupManager->SetCurrentLevel(this->GetLevelName());
        this->OnLoadBefore();

        loaderServer->SetProgressText("Setup World...");
        loaderServer->UpdateProgressDisplay();
        setupManager->SetupWorldFromCurrentLevel();

        loaderServer->SetProgressText("On Load After...");
        loaderServer->UpdateProgressDisplay();
        this->OnLoadAfter();
        loaderServer->CloseProgressIndicator();
    }
    else if (LoadSaveGame == this->setupMode)
    {
        saveGameManager->LoadGame(this->GetSaveGame());
    }

    // clear the startup level and save game name
    this->SetLevelName("");
    this->SetSaveGame("");

    // update the focus manager, so that focus entities are set correctly
    FocusManager::Instance()->OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
GameStateHandler::OnStateLeave(const nString& nextState)
{
    SetupManager::Instance()->CleanupWorld();
}

//------------------------------------------------------------------------------
/**
    The per-frame handler method.
*/
nString
GameStateHandler::OnFrame()
{
    PROFILER_START(this->profCompleteFrame);
    bool running = true;

    // let the time manager update its time
    TimeManager::Instance()->Update();

    // toggle visualizations
    if (nInputServer::Instance()->GetButton("togglePhysicsVisualization"))
    {
        this->physicsVisualizationEnabled = !this->physicsVisualizationEnabled;
    }
    if (nInputServer::Instance()->GetButton("toggleGraphicsVisualization"))
    {
        this->graphicsVisualizationEnabled = !this->graphicsVisualizationEnabled;
    }
    if (nInputServer::Instance()->GetButton("toggleFOVVisualization"))
    {
        this->fovVisualization = !this->fovVisualization;
    }
    if (nInputServer::Instance()->GetButton("toggleGameEntityVisualization"))
    {
        this->gameEntityVisualizationEnabled = !this->gameEntityVisualizationEnabled;
    }

    // trigger subsystem and Nebula servers
    nVideoServer::Instance()->Trigger();
    Input::Server::Instance()->Trigger();
    running &= Foundation::Server::Instance()->GetScriptServer()->Trigger();

    // trigger the audio and game subsystems
    Audio::Server::Instance()->BeginScene();
    Navigation::Server::Instance()->OnBeginFrame();
    Game::Server::Instance()->OnFrame();
    Navigation::Server::Instance()->OnEndFrame();
    Audio::Server::Instance()->EndScene();
    VFX::Server::Instance()->BeginScene();

    PROFILER_START(this->profParticleUpdates);
    nParticleServer::Instance()->Trigger();
    nParticleServer2::Instance()->Trigger();
    PROFILER_STOP(this->profParticleUpdates);

    PROFILER_START(this->profRender);
    running &= Graphics::Server::Instance()->Trigger();
    if (Graphics::Server::Instance()->BeginRender())
    {
        UI::Server::Instance()->Render();
        Graphics::Server::Instance()->Render();
#ifdef MANGALORE_USE_CEGUI
        CEUI::Server::Instance()->Render();
#endif
        if (this->graphicsVisualizationEnabled)
        {
            Graphics::Server::Instance()->RenderDebug();
        }
        if (this->physicsVisualizationEnabled)
        {
            Physics::Server::Instance()->RenderDebug();
        }
        if (this->fovVisualization)
        {
            Navigation::Server::Instance()->RenderDebug();
        }
        if (this->gameEntityVisualizationEnabled)
        {
            Game::Server::Instance()->RenderDebug();
        }
        Graphics::Server::Instance()->EndRender();
    }
    PROFILER_STOP(this->profRender);
    VFX::Server::Instance()->EndScene();

    // trigger kernel server
    nKernelServer::Instance()->Trigger();

    PROFILER_STOP(this->profCompleteFrame);
    if (!running)
    {
        return this->exitState;
    }
    else
    {
        return this->GetName();
    }
}

} // namespace Application
