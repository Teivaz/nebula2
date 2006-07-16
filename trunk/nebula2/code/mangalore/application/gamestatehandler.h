#ifndef APPLICATION_GAMESTATEHANDLER_H
#define APPLICATION_GAMESTATEHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Application::GameStateHandler

    The game state handler sets up the game world and runs the game loop.
    Setup can happen in one of several modes:

    * EMPTY WORLD: doesn't require a world database
    * NEW GAME: game starts from a fresh copy of the world database
    * CONTINUE GAME: game continues at the last valid position
    * LOAD LEVEL: game starts at the given level
    * LOAD SAVE GAME: game starts with the given save game

    (C) 2006 RadonLabs GmbH
*/
#include "application/statehandler.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nshader2.h"

//------------------------------------------------------------------------------
namespace Application
{
class GameStateHandler : public StateHandler
{
    DeclareRtti;
	DeclareFactory(GameStateHandler);

public:
    /// setup modes
    enum SetupMode
    {
        EmptyWorld,
        NewGame,
        ContinueGame,
        LoadLevel,
        LoadSaveGame,
    };

    /// constructor
    GameStateHandler();
    /// destructor
    virtual ~GameStateHandler();
    /// set the setup mode
    void SetSetupMode(SetupMode mode);
    /// get the setup mode
    SetupMode GetSetupMode() const;
    /// set database filename
    void SetDbName(const nString& n);
    /// get database name
    const nString& GetDbName() const;
    /// set level filename, required by setup mode LoadLevel
    void SetLevelName(const nString& n);
    /// get level name
    const nString& GetLevelName() const;
    /// set save game name, required by setup mode LoadSaveGame
    void SetSaveGame(const nString& n);
    /// get save game name
    const nString& GetSaveGame() const;
    /// configure followup state when Alt-F4 or Windows close button is pressed (default: App::Exit)
    void SetExitState(const nString& state);
    /// get Alt-F4 state
    const nString& GetExitState() const;
    /// called when the state represented by this state handler is entered
    virtual void OnStateEnter(const nString& prevState);
    /// called when the state represented by this state handler is left
    virtual void OnStateLeave(const nString& nextState);
    /// called each frame as long as state is current, return new state
    virtual nString OnFrame();

protected:
    /// update timestamps for subsystems
    virtual void UpdateSubsystemTimes();

    SetupMode setupMode;
    nString exitState;
    nString dbName;
    nString levelName;
    nString saveGame;
    bool physicsVisualizationEnabled;
    bool graphicsVisualizationEnabled;
    bool fovVisualization;
};

RegisterFactory(GameStateHandler);

//------------------------------------------------------------------------------
/**
*/
inline
void
GameStateHandler::SetSetupMode(SetupMode mode)
{
    this->setupMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
GameStateHandler::SetupMode
GameStateHandler::GetSetupMode() const
{
    return this->setupMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GameStateHandler::SetExitState(const nString& state)
{
    this->exitState = state;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GameStateHandler::GetExitState() const
{
    return this->exitState;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GameStateHandler::SetDbName(const nString& n)
{
    this->dbName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GameStateHandler::GetDbName() const
{
    return this->dbName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GameStateHandler::SetLevelName(const nString& n)
{
    this->levelName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GameStateHandler::GetLevelName() const
{
    return this->levelName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GameStateHandler::SetSaveGame(const nString& n)
{
    this->saveGame = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GameStateHandler::GetSaveGame() const
{
    return this->saveGame;
}

};
//------------------------------------------------------------------------------
#endif
