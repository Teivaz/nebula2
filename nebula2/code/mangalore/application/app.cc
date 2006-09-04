//------------------------------------------------------------------------------
//  application/app.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/app.h"
#include "foundation/factory.h"
#include "kernel/nfileserver2.h"
#include "gui/nguiserver.h"
#include "application/statehandler.h"
#include "loader/entityloader.h"
#include "loader/environmentloader.h"
#include "game/time/systemtimesource.h"
#include "game/time/gametimesource.h"
#include "game/time/inputtimesource.h"
#include "game/time/guitimesource.h"

#ifdef __NEBULA_STATS__
// count the DB queries per frame
#include "sql/nsqlite3query.h"
#endif

nNebulaUsePackage(ui);

namespace Application
{
App* App::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
App::App() :
    foundationServer(0),
    isOpen(false),
    isRunning(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
App::~App()
{
    n_assert(!this->isOpen);
    n_assert(!this->isRunning);
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Override this method in subclasses to return a different application name.
*/
nString
App::GetAppName() const
{
    return "Mangalore";
}

//------------------------------------------------------------------------------
/**
    Override this method in subclasses to return a different version string.
*/
nString
App::GetAppVersion() const
{
    return "1.0";
}

//------------------------------------------------------------------------------
/**
    Get the application vendor. This is usually the publishers company name.
*/
nString
App::GetVendorName() const
{
    return "Radon Labs GmbH";
}

//------------------------------------------------------------------------------
/**
    Return true if this application may only be run in fullscreen mode.
    By default the application may also run in windowed mode. Override this
    method in your subclass if fullscreen should be enforced.
*/
bool
App::GetForceFullscreen() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    This method checks whether all startup conditions for the application
    are met (like the proper Direct3D version installed, etc..). The
    method is called from App::Open(). Override this method in a subclass
    if different checks are needed.
*/
bool
App::DoStartupCheck()
{
    // check if app is already running
    // FIXME: LOCALIZE!
    nString errorMsgAlreadyRunning;
    errorMsgAlreadyRunning.Format("Cannot start '%s' because it is already\n"
                                  "running in another desktop session!", this->GetAppName().Get());
    if (this->startupChecker.CheckAlreadyRunning(this->GetVendorName(),
                                                 this->GetAppName(),
                                                 this->displayMode.GetWindowTitle(),
                                                 this->GetAppName(),
                                                 errorMsgAlreadyRunning))
    {
        // application is already running
        return false;
    }

#ifdef __WIN32__
    // check if D3D can be initialized
    // FIXME: LOCALIZE!
    nString errorMsgD3D("Cannot initialize Direct3D!");
    if (!this->startupChecker.CheckDirect3D(this->GetAppName(), errorMsgD3D))
    {
        // could not initialize Direct3D
        return false;
    }

    // check if DirectSound can be initialized
    nString errorMsgDS("Cannot initialize DirectSound!");
    if (!this->startupChecker.CheckDirectSound(this->GetAppName(), errorMsgDS))
    {
        // could not initialize DirectSound
        return false;
    }
#endif

    // all ok
    return true;
}

//------------------------------------------------------------------------------
/**
    Setup the default settings for this application which are then overriden
    by SetupFromCmdLine() and SetupFromProfile() (the methods will be called
    in this order by App::Open(). Override this method in your App subclass if
    your application needs different defaults.
*/
void
App::SetupFromDefaults()
{
    // setup display mode
    nDisplayMode2 mode;
    nString windowTitle = this->GetVendorName() + " - " + this->GetAppName() + " - " + this->GetAppVersion();
    mode.SetWindowTitle(windowTitle.Get());
    mode.SetXPos(0);
    mode.SetYPos(0);
    mode.SetWidth(1024);
    mode.SetHeight(768);
    mode.SetType(nDisplayMode2::Windowed);
    mode.SetVerticalSync(false);
    mode.SetDialogBoxMode(true);
    mode.SetIcon("Icon");
    this->SetDisplayMode(mode);
}

//------------------------------------------------------------------------------
/**
    Change the app settings according to a user profile. This method is
    called by App::Open() after SetupFromDefault() and before
    SetupFromCmdLineArgs().
*/
void
App::SetupFromProfile()
{
    // FIXME
}

//------------------------------------------------------------------------------
/**
    Change the app settings from the provided command line args. This
    parses the command line arguments as provided to the application by
    App::SetCmdLineArgs(). Override this method in your App subclass if you
    need different command line args. Please be aware though that every Mangalore
    application should accept the standard command line arguments as implemented
    in this method.

    This method is called by App::Open() after SetupFromDefaults() and
    SetupFromProfile().
*/
void
App::SetupFromCmdLineArgs()
{
    // setup optional startup savegame or level paths
    this->SetStartupSavegame(this->cmdLineArgs.GetStringArg("-loadgame", 0));
    this->SetStartupLevel(this->cmdLineArgs.GetStringArg("-level", 0));
    this->SetWorldDb(this->cmdLineArgs.GetStringArg("-db", 0));

    // setup display mode
    nDisplayMode2 mode = this->GetDisplayMode();
    if (this->cmdLineArgs.HasArg("-fullscreen"))
    {
        if (this->cmdLineArgs.GetBoolArg("-fullscreen") | this->GetForceFullscreen())
        {
            mode.SetType(nDisplayMode2::Fullscreen);
        }
        else
        {
            mode.SetType(nDisplayMode2::Windowed);
        }
    }
    mode.SetXPos(this->cmdLineArgs.GetIntArg("-x", mode.GetXPos()));
    mode.SetYPos(this->cmdLineArgs.GetIntArg("-y", mode.GetYPos()));
    mode.SetWidth(this->cmdLineArgs.GetIntArg("-w", mode.GetWidth()));
    mode.SetHeight(this->cmdLineArgs.GetIntArg("-h", mode.GetHeight()));
    mode.SetAntiAliasSamples(this->cmdLineArgs.GetIntArg("-aa", mode.GetAntiAliasSamples()));
    this->SetDisplayMode(mode);

    // set project directory override
    nString projDirArg = this->cmdLineArgs.GetStringArg("-projdir", 0);
    if (projDirArg.IsValid())
    {
        this->SetProjectDirectory(projDirArg);
    }

    // set feature set override
    nString featureSetArg = this->cmdLineArgs.GetStringArg("-featureset", 0);
    if (featureSetArg.IsValid())
    {
        this->SetFeatureSet(featureSetArg);
    }

    // set render path override
    nString renderPathArg = this->cmdLineArgs.GetStringArg("-renderpath", 0);
    if (renderPathArg.IsValid())
    {
        this->SetRenderPath(renderPathArg);
    }
}

//------------------------------------------------------------------------------
/**
    Setup the default input mapping.
*/
void
App::SetupDefaultInputMapping()
{
    Input::Server* inputServer = Input::Server::Instance();

    // per default the input server keep track of the 2d windows mouse position
    inputServer->AddMapping("mousePosition", "mouse0:position");

    inputServer->AddMapping("mouseLMB", "mouse0:btn0");
    inputServer->AddMapping("mouseRMB", "mouse0:btn1");
    inputServer->AddMapping("mouseMMB", "mouse0:btn2");
    inputServer->AddMapping("mouseWheelDown", "relmouse0:+zbtn");
    inputServer->AddMapping("mouseWheelUp", "relmouse0:-zbtn");
    inputServer->AddMapping("mouseLeft", "relmouse0:-x");
    inputServer->AddMapping("mouseRight", "relmouse0:+x");
    inputServer->AddMapping("mouseUp", "relmouse0:-y");
    inputServer->AddMapping("mouseDown", "relmouse0:+y");

    // for the character/text input add a default mapping
    inputServer->AddMapping("textinput", "keyb0:characters");

    inputServer->AddMapping("ctrl", "keyb0:ctrl");
    inputServer->AddMapping("shift", "keyb0:shift");
    inputServer->AddMapping("space", "keyb0:space");
    inputServer->AddMapping("tab", "keyb0:tab");
    inputServer->AddMapping("escape", "keyb0:esc");

    inputServer->AddMapping("timeReset", "keyb0:t");
}

//------------------------------------------------------------------------------
/**
    This setups the time sources for the application and attaches them
    to the time manager. Overwrite/extend as needed in subclass.
*/
void
App::SetupTimeSources()
{
    Ptr<Game::SystemTimeSource> systemTimeSource = Game::SystemTimeSource::Create();
    Ptr<Game::GameTimeSource> gameTimeSource = Game::GameTimeSource::Create();
    Ptr<Game::InputTimeSource> inputTimeSource = Game::InputTimeSource::Create();
    Ptr<Game::GuiTimeSource> guiTimeSource = Game::GuiTimeSource::Create();
    Managers::TimeManager* timeManager = Managers::TimeManager::Instance();
    timeManager->AttachTimeSource(systemTimeSource);
    timeManager->AttachTimeSource(gameTimeSource);
    timeManager->AttachTimeSource(inputTimeSource);
    timeManager->AttachTimeSource(guiTimeSource);
}

//------------------------------------------------------------------------------
/**
    Setup the Game subsystem. This is most likely to be different in
    a derived application, so it lives in its own method which can
    be overwritten by a subclass.
*/
void
App::SetupGameSubsystem()
{
    this->gameServer = Game::Server::Create();
    this->gameServer->Open();

    // setup the time manager first before everything else
    Ptr<Managers::TimeManager> timeManager = Managers::TimeManager::Create();
    this->gameServer->AttachManager(timeManager);
    this->SetupTimeSources();

    // setup the other managers...
    this->entityManager = Managers::EntityManager::Create();
    this->envQueryManager = Managers::EnvQueryManager::Create();
    this->focusManager = Managers::FocusManager::Create();
    this->saveGameManager = Managers::SaveGameManager::Create();
    this->setupManager = Managers::SetupManager::Create();
    this->factoryManager = Managers::FactoryManager::Create();
    this->gameServer->AttachManager(entityManager);
    this->gameServer->AttachManager(envQueryManager);
    this->gameServer->AttachManager(focusManager);
    this->gameServer->AttachManager(saveGameManager);
    this->gameServer->AttachManager(setupManager);
    this->gameServer->AttachManager(factoryManager);
}

//------------------------------------------------------------------------------
/**
    Cleanup the game subsystem.
*/
void
App::CleanupGameSubsystem()
{
    this->gameServer->RemoveManager(this->factoryManager);
    this->gameServer->RemoveManager(this->setupManager);
    this->gameServer->RemoveManager(this->saveGameManager);
    this->gameServer->RemoveManager(this->focusManager);
    this->gameServer->RemoveManager(this->envQueryManager);
    this->gameServer->RemoveManager(this->entityManager);
    this->factoryManager = 0;
    this->setupManager = 0;
    this->saveGameManager = 0;
    this->focusManager = 0;
    this->envQueryManager = 0;
    this->entityManager = 0;
    this->gameServer->Close();
    this->gameServer = 0;
}

//------------------------------------------------------------------------------
/**
    Setup the Mangalore subsystems. Override this method if you need
    a different Mangalore subsystem setup.
*/
void
App::SetupSubsystems()
{
    // setup the foundation subsystem
    this->foundationServer->SetProjectDir(this->projDir);
    if(this->GetStartupPath().IsValid())
    {
        this->foundationServer->SetStartupPath(this->GetStartupPath());
    }
    this->foundationServer->Open();

    // setup the script subsystem
    this->scriptServer = Script::Server::Create();
    this->scriptServer->Open();

    // setup the message subsystem
    this->messageServer = Message::Server::Create();
    this->messageServer->Open();

    // setup the db subsystem
    this->dbServer = Db::Server::Create();

    // setup the physics subsystem
    this->physicsServer = Physics::Server::Create();
    this->physicsServer->Open();

    // setup the graphics subsystem
    this->graphicsServer = Graphics::Server::Create();
    this->graphicsServer->SetDisplayMode(this->displayMode);
    this->graphicsServer->SetRenderPath(this->renderPath);
    this->graphicsServer->SetFeatureSet(this->featureSet);
    this->graphicsServer->Open();

    // setup the input subsystem
    this->inputServer = Input::Server::Create();
    this->SetupDefaultInputMapping();
    this->inputServer->Open();

    // setup the audio subsystem
    this->audioServer = Audio::Server::Create();
    this->audioServer->Open();
    if (nFileServer2::Instance()->FileExists("data:tables/sound.xml"))
    {
        this->audioServer->OpenWaveBank("data:tables/sound.xml");
    }
    else
    {
        n_printf("Warning: tables:sound.xml doesn't exist!\n");
    }

    // setup the vfx subsystem
    this->vfxServer = VFX::Server::Create();
    this->vfxServer->Open();
    if (nFileServer2::Instance()->FileExists("data:tables/effects.xml"))
    {
        this->vfxServer->OpenEffectBank("data:tables/effects.xml");
    }
    else
    {
        n_printf("Warning: data:tables/effects.xml doesn't exist!\n");
    }

    // setup the navigation subsystem
    this->navigationServer = Navigation::Server::Create();

    // setup the loader subsystem
    this->loaderServer = Loader::Server::Create();
    this->loaderServer->Open();

    // attach loader to Loader::Server
    Ptr<Loader::EntityLoader> entityloader = Loader::EntityLoader::Create();
    this->loaderServer->AttachLoader(entityloader);

    Ptr<Loader::EnvironmentLoader> environmentloader = Loader::EnvironmentLoader::Create();
    this->loaderServer->AttachLoader(environmentloader);

    // setup the gui subsystem (FIXME)
    nGuiServer* guiServer = nGuiServer::Instance();
    guiServer->SetRootPath("/res/gui");
    guiServer->SetDisplaySize(vector2(1024.0f, 768.0f));
    guiServer->Open();
    this->uiServer = UI::Server::Create();
    this->uiServer->Open();
#ifdef MANGALORE_USE_CEGUI
    this->ceuiServer = CEUI::Server::Create();
    this->ceuiServer->Open();
#endif
}

//------------------------------------------------------------------------------
/**
    Cleanup the Mangalore subsystems.
*/
void
App::CleanupSubsystems()
{
    // cleanup gui stuff
    nGuiServer::Instance()->Close();
    this->uiServer->Close();
    this->uiServer = 0;
#ifdef MANGALORE_USE_CEGUI
    this->ceuiServer->Close();
    this->ceuiServer = 0;
#endif

    // cleanup loader subsystem
    this->loaderServer->Close();
    this->loaderServer = 0;

    // cleanup navigation subsystem
    if (this->navigationServer->IsOpen())
    {
        this->navigationServer->Close();
    }
    this->navigationServer = 0;

    // cleanup vfx subsystem
    this->vfxServer->Close();
    this->vfxServer = 0;

    // cleanup audio subsystem
    this->audioServer->Close();
    this->audioServer = 0;

    // cleanup graphics subsystem
    this->graphicsServer->Close();
    this->graphicsServer = 0;

    // cleanup physics subsystem
    this->physicsServer->Close();
    this->physicsServer = 0;

    // cleanup input subsysyem
    this->inputServer->Close();
    this->inputServer = 0;

    // cleanup the db subsystem
    this->dbServer = 0;

    // cleanup the message subsystem
    this->messageServer->Close();
    this->messageServer = 0;

    //  cleanup the script subsystem
    this->scriptServer->Close();
    this->scriptServer = 0;

    // cleanup the foundation subsystem
    this->foundationServer->Close();
}

//------------------------------------------------------------------------------
/**
    Setup the application state handlers. This method is called by App::Open()
    after the Mangalore subsystems have been initialized. Override this method
    to create and attach your application state handlers with the
    application object.
*/
void
App::SetupStateHandlers()
{
    // FIXME
}

//------------------------------------------------------------------------------
/**
    Cleanup the application state handlers. This will call the
    OnRemoveFromApplication() method on all attached state handlers
    and release them. Usually you don't need to override this method in
    your app.
*/
void
App::CleanupStateHandlers()
{
    // release game state handlers
    this->requestedState.Clear();
    this->curState.Clear();
    this->nextState.Clear();

    // cleanup state handlers
    int num = this->stateHandlers.Size();
    for (int i = 0; i < num; i++)
    {
        this->stateHandlers[i]->OnRemoveFromApplication();
    }
    this->stateHandlers.Clear();
}

//------------------------------------------------------------------------------
/**
    Open the application, this will initialize the runtime environment.
    Return true when successful. The next method call should be Run(),
    which doesn't return until the application terminates.
*/
bool
App::Open()
{
    n_assert(!this->isOpen);
    nString logName = this->GetAppName() + " - " + this->GetAppVersion();
    this->foundationServer = n_new(Foundation::Server(this->GetVendorName(), this->GetAppName(), logName));

    // setup members
    this->isRunning = false;

    // initialize app settings
    this->SetupFromDefaults();
    this->SetupFromProfile();
    this->SetupFromCmdLineArgs();

    // check if system meets startup preconditions
    if (!this->DoStartupCheck())
    {
        return false;
    }

    // setup the Mangalore subsystems
    this->SetupSubsystems();
    this->SetupGameSubsystem();

    // setup application state handlers
    this->curState.Clear();
    this->nextState.Clear();
    this->SetupStateHandlers();

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the application. Shuts down the runtime environment.
*/
void
App::Close()
{
    n_assert(this->isOpen);
    n_assert(!this->isRunning);

    this->CleanupStateHandlers();
    this->CleanupGameSubsystem();
    this->CleanupSubsystems();

    n_delete(this->foundationServer);
    this->foundationServer = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    This is a general OnFrame() method for the application object. If
    your subclass needs to do work in the application object per frame,
    then override this method. But please be aware that most per-frame
    work should be done in the App's state handlers.

    NOTE: This method is called from App::Run() after the current state
    handler's OnFrame() method has been called.
*/
void
App::OnFrame()
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
    Run the application. This method will return when the application wishes
    to exit.
*/
void
App::Run()
{
    while (this->GetCurrentState() != "Exit")
    {
        #ifdef __NEBULA_STATS__
        // reset the per frame db access counter
        nSQLite3Query::dbAccessCount = 0;
        #endif

        // call the current state handler
        StateHandler* curStateHandler = this->FindStateHandlerByName(this->GetCurrentState());
        n_assert(curStateHandler);
        nString newState = curStateHandler->OnFrame();

        // call the app's OnFrame() method
        this->OnFrame();

        // a requested state always overrides the returned state
        if (this->requestedState.IsValid())
        {
            this->SetState(this->requestedState);
        }
        else if (newState != curStateHandler->GetName())
        {
            // a normal state transition
            this->SetState(newState);
        }

        // give up time slice
        n_sleep(0.0);
        #ifdef __NEBULA_STATS__
        // show the db access count
        nWatched dbAccess("profMangaDBQuerysPerFrame", nArg::Int);
        dbAccess->SetI(nSQLite3Query::dbAccessCount);
        #endif
    }
}


//------------------------------------------------------------------------------
/**
    Do a state transition. This method is called by SetState() when the
    new state is different from the previous state.
*/
void
App::DoStateTransition()
{
    // cleanup previous state
    if (this->curState.IsValid())
    {
        StateHandler* curStateHandler = this->FindStateHandlerByName(this->curState);
        n_assert(curStateHandler);
        curStateHandler->OnStateLeave(this->nextState);
    }

    // initialize new state
    nString prevState = this->curState;
    this->curState = this->nextState;
    if (this->nextState.IsValid())
    {
        StateHandler* nextStateHandler =  this->FindStateHandlerByName(this->nextState);
        if (nextStateHandler)
        {
            nextStateHandler->OnStateEnter(prevState);
        }
    }
    this->requestedState.Clear();
}

//------------------------------------------------------------------------------
/**
    Request a new state. This is a public method to switch states
    (SetState() is private because it invokes some internal voodoo). The
    requested state will be activated at the end of the frame.
*/
void
App::RequestState(const nString& s)
{
    this->requestedState = s;
}

//------------------------------------------------------------------------------
/**
    Set a new application state. This method will call DoStateTransition().
*/
void
App::SetState(const nString& s)
{
    this->nextState = s;
    this->DoStateTransition();
}

//------------------------------------------------------------------------------
/**
    Register a state handler object with the application.

    @param  handler  pointer to a state handler object
*/
void
App::AddStateHandler(StateHandler* handler)
{
    this->stateHandlers.Append(handler);
    handler->OnAttachToApplication();
}

//------------------------------------------------------------------------------
/**
    Find a state handler by name.

    @param          name of state to return the state handler for
    @return         pointer to state handler object associated with the state (can be 0)
*/
StateHandler*
App::FindStateHandlerByName(const nString& name) const
{
    int num = this->GetNumStates();
    for (int i = 0; i < num; i++)
    {
        if (this->stateHandlers[i]->GetName() == name)
        {
            return this->stateHandlers[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Find a state handler by RTTI.
*/
StateHandler*
App::FindStateHandlerByRtti(const Foundation::Rtti& rtti) const
{
    int num = this->GetNumStates();
    for (int i = 0; i < num; i++)
    {
        if (this->stateHandlers[i]->IsInstanceOf(rtti))
        {
            return this->stateHandlers[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Get the current state handler.
*/
StateHandler*
App::GetCurrentStateHandler() const
{
    n_assert(this->curState.IsValid());
    StateHandler* curStateHandler = this->FindStateHandlerByName(this->curState);
    n_assert(0 != curStateHandler);
    return curStateHandler;
}

} // namespace Application
