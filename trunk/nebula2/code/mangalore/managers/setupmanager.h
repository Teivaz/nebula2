#ifndef MANAGERS_SETUPMANAGER_H
#define MANAGERS_SETUPMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Managers::SetupManager

    The setup manager initializes the game world before a new level file
    is loaded or a location is entered. This standard class implements
    some default setup, like creating an empty graphics and physics subsystem
    level, derive a more advanced setup in your own subclass.
    
    (C) 2005 Radon Labs GmbH
*/
#include "game/manager.h"
#include "mathlib/bbox.h"

//------------------------------------------------------------------------------
namespace Managers
{
class SetupManager : public Game::Manager
{
    DeclareRtti;
	DeclareFactory(SetupManager);

public:
    /// constructor
    SetupManager();
    /// destructor
    virtual ~SetupManager();
    /// get instance pointer
    static SetupManager* Instance();
    /// set current level name
    void SetCurrentLevel(const nString& n);
    /// get current level name
    nString GetCurrentLevel() const;
    /// set optional world dimensions
    void SetWorldDimensions(const bbox3& box);
    /// get world dimensions
    const bbox3& GetWorldDimensions() const;
    /// setup an empty game world
    void SetupEmptyWorld();
    /// setup world from level
    void SetupWorldFromCurrentLevel();
    /// cleanup the game world
    void CleanupWorld();

private:
    static SetupManager* Singleton;
    bbox3 worldBox;
};

RegisterFactory(SetupManager);

//------------------------------------------------------------------------------
/**
*/
inline
SetupManager*
SetupManager::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
SetupManager::SetWorldDimensions(const bbox3& box)
{
    this->worldBox = box;
}

//------------------------------------------------------------------------------
/**
*/
inline
const bbox3&
SetupManager::GetWorldDimensions() const
{
    return this->worldBox;
}

}; // namespace Managers
//------------------------------------------------------------------------------
#endif
