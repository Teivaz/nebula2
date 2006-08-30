#ifndef APPLICATION_APP_H
#define APPLICATION_APP_H
//------------------------------------------------------------------------------
/**
    @class Application::App

    The App class is where specific application classes are derived
    from. The Application class defines the highlevel runtime environment
    wrapper for a specific game. It goes through specific application states
    which may display UI screens to the user, load and start a level,
    manage game options.

    A game application should derive a subclass from class Application and
    create an object of that class in its main() function.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "gfx2/ndisplaymode2.h"
#include "foundation/server.h"
#include "message/server.h"
#include "graphics/server.h"
#include "audio/server.h"
#include "physics/server.h"
#include "input/server.h"
#include "loader/server.h"
#include "navigation/server.h"
#include "game/server.h"
#include "vfx/server.h"
#include "db/server.h"
#include "ui/server.h"
#include "ceui/server.h"
#include "script/server.h"
#include "tools/ncmdlineargs.h"
#include "util/nstartupchecker.h"
#include "managers/entitymanager.h"
#include "managers/envquerymanager.h"
#include "managers/factorymanager.h"
#include "managers/focusmanager.h"
#include "managers/savegamemanager.h"
#include "managers/setupmanager.h"
#include "managers/timemanager.h"

#define MANGALORE_USE_CEGUI

//------------------------------------------------------------------------------
namespace Application
{
class StateHandler;

class App
{
public:
    /// constructor
    App();
    /// destructor
    virtual ~App();
    /// get instance pointer
    static App* Instance();

    /// set command line arguments
    void SetCmdLineArgs(const nCmdLineArgs& args);
    /// get command line arguments
    const nCmdLineArgs& GetCmdLineArgs() const;
    /// open application
    virtual bool Open();
    /// close the application
    virtual void Close();
    /// is application open
    bool IsOpen() const;
    /// run the application
    virtual void Run();

    /// returns the application name (override in subclass)
    virtual nString GetAppName() const;
    /// returns the version string (override in subclass)
    virtual nString GetAppVersion() const;
    /// returns vendor name (override in subclass)
    virtual nString GetVendorName() const;
    /// return true if this is a pure fullscreen app (override in subclass)
    virtual bool GetForceFullscreen() const;
    /// get path to startup.tcl
    virtual nString GetStartupPath() const;

    /// get project directory
    const nString& GetProjectDirectory() const;
    /// get display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// get path to current db
    const nString& GetWorldDb() const;
    /// get path to current level
    const nString& GetStartupLevel() const;
    // get startup save game
    const nString& GetStartupSavegame() const;
    /// get optional feature set string
    const nString& GetFeatureSet() const;
    /// get optional render path override
    const nString& GetRenderPath() const;

    /// add an application state handler
    void AddStateHandler(StateHandler* state);
    /// find a state handler by name
    StateHandler* FindStateHandlerByName(const nString& stateName) const;
    /// find a state handler by class
    StateHandler* FindStateHandlerByRtti(const Foundation::Rtti& rtti) const;
    /// return pointer to current state handler
    StateHandler* GetCurrentStateHandler() const;
    /// return state handler of current state
    const nString& GetCurrentState() const;
    /// get number of application states
    int GetNumStates() const;
    /// get state handler at index
    StateHandler* GetStateHandlerAt(int index) const;
    /// request a new state which will be applied at the end of the frame
    void RequestState(const nString& stateName);

    /// start capturing
    void StartFrameCapture();
    /// stop capturing
    void StopFrameCapture();
    /// return true if currently capturing
    bool IsCapturing() const;

protected:
    /// called per frame by Run(), override in subclass as needed
    virtual void OnFrame();
    /// check if application startup conditions are met
    virtual bool DoStartupCheck();
    /// setup application defaults, override in subclass
    virtual void SetupFromDefaults();
    /// parse command line args, override in subclass if new args are desired
    virtual void SetupFromCmdLineArgs();
    /// setup the application from a user profile
    virtual void SetupFromProfile();
    /// setup the app's time sources (called by SetupGameSubsystem)
    virtual void SetupTimeSources();
    /// setup the app's input mapping (called in SetupSubsystems())
    virtual void SetupDefaultInputMapping();
    /// setup the Game subsystem
    virtual void SetupGameSubsystem();
    /// cleanup the Game subsystem
    virtual void CleanupGameSubsystem();
    /// setup other Mangalore subsystems
    virtual void SetupSubsystems();
    /// cleanup Mangalore subsystems
    virtual void CleanupSubsystems();
    /// setup application state handlers
    virtual void SetupStateHandlers();
    /// cleanup application state handlers
    virtual void CleanupStateHandlers();
    /// perform a state transition
    virtual void DoStateTransition();
    /// set an application state
    void SetState(const nString& s);
    /// set project directory
    void SetProjectDirectory(const nString& dir);
    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// set world database path
    void SetWorldDb(const nString& path);
    /// set startup level path
    void SetStartupLevel(const nString& path);
    // set startup save game
    void SetStartupSavegame(const nString& savegame);
    /// set optional graphics feature set feature set string
    void SetFeatureSet(const nString& s);
    /// set optional graphics render path override
    void SetRenderPath(const nString& s);

    static App* Singleton;

    Foundation::Server* foundationServer;
    Ptr<Game::Server> gameServer;
    Ptr<Graphics::Server> graphicsServer;
    Ptr<Physics::Server> physicsServer;
    Ptr<Input::Server> inputServer;
    Ptr<Loader::Server> loaderServer;
    Ptr<Message::Server> messageServer;
    Ptr<Audio::Server> audioServer;
    Ptr<Navigation::Server> navigationServer;
    Ptr<VFX::Server> vfxServer;
    Ptr<Db::Server> dbServer;
    Ptr<UI::Server> uiServer;
#ifdef MANGALORE_USE_CEGUI
    Ptr<CEUI::Server> ceuiServer;
#endif
    Ptr<Script::Server> scriptServer;

    Ptr<Managers::EntityManager> entityManager;
    Ptr<Managers::EnvQueryManager> envQueryManager;
    Ptr<Managers::FocusManager> focusManager;
    Ptr<Managers::SaveGameManager> saveGameManager;
    Ptr<Managers::SetupManager> setupManager;
    Ptr<Managers::FactoryManager> factoryManager;

    nStartupChecker startupChecker;
    nCmdLineArgs cmdLineArgs;
    nString projDir;
    nString startupDb;
    nString startupLevel;
    nString startupSavegame;
    nString featureSet;
    nString renderPath;
    nDisplayMode2 displayMode;
    bool isOpen;
    bool isRunning;

    nString requestedState;
    nString curState;
    nString nextState;
    nArray<Ptr<StateHandler> > stateHandlers;
};

//------------------------------------------------------------------------------
/**
*/
inline
App*
App::Instance()
{
    n_assert(0 != App::Singleton);
    return App::Singleton;
}


//------------------------------------------------------------------------------
/**
*/
inline
int
App::GetNumStates() const
{
    return this->stateHandlers.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
StateHandler*
App::GetStateHandlerAt(int index) const
{
    return this->stateHandlers[index];
}

//------------------------------------------------------------------------------
/**
    Returns the currently active application state. Can be 0 if no valid
    state is set.
*/
inline
const nString&
App::GetCurrentState() const
{
    return this->curState;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
App::SetProjectDirectory(const nString& dir)
{
    this->projDir = dir;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
App::GetProjectDirectory() const
{
    return this->projDir;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
App::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
App::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
App::SetWorldDb(const nString& l)
{
    this->startupDb = l;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
App::GetWorldDb() const
{
    return this->startupDb;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
App::SetStartupLevel(const nString& l)
{
    this->startupLevel = l;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
App::GetStartupLevel() const
{
    return this->startupLevel;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
App::SetFeatureSet(const nString& s)
{
    this->featureSet = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
App::GetFeatureSet() const
{
    return this->featureSet;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
App::SetRenderPath(const nString& s)
{
    this->renderPath = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
App::GetRenderPath() const
{
    return this->renderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
App::SetStartupSavegame(const nString& n)
{
    this->startupSavegame = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
App::GetStartupSavegame() const
{
    return this->startupSavegame;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
App::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
App::SetCmdLineArgs(const nCmdLineArgs& args)
{
    this->cmdLineArgs = args;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nCmdLineArgs&
App::GetCmdLineArgs() const
{
    return this->cmdLineArgs;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
App::GetStartupPath() const
{
    return "";
}

};
//------------------------------------------------------------------------------
#endif
