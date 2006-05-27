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
    fovVisualization(false)
{
    // empty
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
    this->physicsVisualizationEnabled = false;
    this->graphicsVisualizationEnabled = false;
    this->UpdateSubsystemTimes();

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
        setupManager->SetCurrentLevel(this->GetLevelName());

        // notify application state handler
        this->OnLoadBefore();

        // setup world from current game
        setupManager->SetupWorldFromCurrentLevel();

        // notify application state handler
        this->OnLoadAfter();
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
    This updates the timing information for all Mangalore subsystems.
*/
void
GameStateHandler::UpdateSubsystemTimes()
{
    nTime time = App::Instance()->GetTime();
    nTime frameTime = App::Instance()->GetFrameTime();
    Input::Server::Instance()->SetTime(time);
    Physics::Server::Instance()->SetTime(time);
    VFX::Server::Instance()->SetTime(time);
    Audio::Server::Instance()->SetTime(time);
    Graphics::Server::Instance()->SetTime(time);
    Graphics::Server::Instance()->SetFrameTime(frameTime);
    UI::Server::Instance()->SetTime(time);
    UI::Server::Instance()->SetFrameTime(frameTime);
#ifdef MANGALORE_USE_CEGUI
    CEUI::Server::Instance()->SetTime(time);
    CEUI::Server::Instance()->SetFrameTime(frameTime);
#endif
    nGuiServer::Instance()->SetTime(time);
    TimeManager::Instance()->SetTime(time);
    TimeManager::Instance()->SetFrameTime(frameTime);
}

//------------------------------------------------------------------------------
/**
    The per-frame handler method.
*/
nString
GameStateHandler::OnFrame()
{
    bool running = true;

    // distribute timestamps to interested subsystems
    this->UpdateSubsystemTimes();

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

    // trigger subsystem and Nebula servers
    nVideoServer::Instance()->Trigger();
    Input::Server::Instance()->Trigger();
    running &= Foundation::Server::Instance()->GetScriptServer()->Trigger();

    // trigger the audio and game subsystems
    Audio::Server::Instance()->BeginScene();
    Game::Server::Instance()->OnFrame();
    Audio::Server::Instance()->EndScene();
    VFX::Server::Instance()->BeginScene();
    nParticleServer::Instance()->Trigger();
    nParticleServer2::Instance()->Trigger();
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
        Graphics::Server::Instance()->EndRender();
    }
    VFX::Server::Instance()->EndScene();

    // trigger kernel server
    nKernelServer::Instance()->Trigger();

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
