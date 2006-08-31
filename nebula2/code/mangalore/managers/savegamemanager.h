#ifndef MANAGERS_SAVEGAMEMANAGER_H
#define MANAGERS_SAVEGAMEMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Managers::SaveGameManager

    Manages savegames.

    (C) 2005 Radon Labs GmbH
*/
#include "game/manager.h"

//------------------------------------------------------------------------------
namespace Managers
{
class SaveGameManager : public Game::Manager
{
    DeclareRtti;
	DeclareFactory(SaveGameManager);

public:
    /// constructor
    SaveGameManager();
    /// destructor
    virtual ~SaveGameManager();
    /// return instance pointer
    static SaveGameManager* Instance();
    /// return true if a current game exists
    static bool CurrentGameExists();
    /// optional method to override start level for NewGame()
    void SetStartLevelOverride(const nString& levelName);
    /// get optional override start level
    const nString& GetStartLevelOverride() const;
    /// setup a new game
    virtual bool NewGame();
    /// continue game
    virtual bool ContinueGame();
    /// create a new save game
    virtual bool SaveGame(const nString& saveGameName);
    /// load an existing save game
    virtual bool LoadGame(const nString& saveGameName);
    /// get name of startup level (defined in world database)
    nString GetStartupLevel();

private:

    static SaveGameManager* Singleton;
    nString overrideStartLevel;
};

RegisterFactory(SaveGameManager);

//------------------------------------------------------------------------------
/**
*/
inline
SaveGameManager*
SaveGameManager::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
    Override the startup level. Normally this is read from the world database.
*/
inline
void
SaveGameManager::SetStartLevelOverride(const nString& levelName)
{
    this->overrideStartLevel = levelName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
SaveGameManager::GetStartLevelOverride() const
{
    return this->overrideStartLevel;
}

} // namespace Managers
//------------------------------------------------------------------------------
#endif
