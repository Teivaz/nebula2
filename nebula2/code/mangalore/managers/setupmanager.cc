//------------------------------------------------------------------------------
//  managers/setupmanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "managers/setupmanager.h"
#include "managers/entitymanager.h"
#include "physics/server.h"
#include "graphics/server.h"
#include "foundation/factory.h"
#include "util/celltreebuilder.h"
#include "physics/level.h"
#include "loader/server.h"
#include "db/query.h"
#include "db/server.h"
#include "foundation/ptr.h"
#include "attr/attributes.h"
#include "managers/savegamemanager.h"
#include "game/server.h"
#include "navigation/server.h"

namespace Managers
{
ImplementRtti(Managers::SetupManager, Game::Manager);
ImplementFactory(Managers::SetupManager);

SetupManager* SetupManager::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
SetupManager::SetupManager() :
    worldBox(vector3(0.0f, 0.0f, 0.0f), vector3(500.0f, 100.0, 500.0f))
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
SetupManager::~SetupManager()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Set the current level name, this will immediately update the
    global database attribute "CurrentLevel". A hard error is thrown
    if the level doesn't exist.
*/
void
SetupManager::SetCurrentLevel(const nString& levelName)
{
    // make sure the level actually exists in the world database
    Ptr<Db::Query> levelQuery = Db::Server::Instance()->CreateLevelQuery(levelName);
    levelQuery->Execute();
    if (levelQuery->GetNumRows() == 0)
    {
        n_error("SetupManager::SetCurrentLevel(): level '%s' not found in world database!", levelName.Get());
    }

    // update the CurrentLevel global attribute
    Db::Server::Instance()->SetGlobalString(Attr::CurrentLevel, levelName);
}

//------------------------------------------------------------------------------
/**
    Get the current level name directly from the database.
*/
nString
SetupManager::GetCurrentLevel() const
{
    return Db::Server::Instance()->GetGlobalString(Attr::CurrentLevel);
}

//------------------------------------------------------------------------------
/**
    Setup a new, empty world.
*/
void
SetupManager::SetupEmptyWorld()
{
    n_assert(EntityManager::Instance()->GetNumEntities() == 0);

    // create a new physics level
	Ptr<Physics::Level> physicsLevel = Physics::Level::Create();
    Physics::Server::Instance()->SetLevel(physicsLevel);

    // create a new graphics level
	Ptr<Graphics::Level> graphicsLevel = Graphics::Level::Create();
    Util::CellTreeBuilder cellTreeBuilder;
    cellTreeBuilder.BuildQuadTree(graphicsLevel, 3, this->worldBox);
    Graphics::Server::Instance()->SetLevel(graphicsLevel);

    // open navigation subsystem
    Navigation::Server::Instance()->Open();
}

//------------------------------------------------------------------------------
/**
    Setup a new world from the level name defined by the currently
    set level name.
*/
void
SetupManager::SetupWorldFromCurrentLevel()
{
    n_assert(EntityManager::Instance()->GetNumEntities() == 0);

    // open navigation subsystem
    Navigation::Server::Instance()->Open();

    // load level from database
    Loader::Server::Instance()->LoadLevel(this->GetCurrentLevel());
}

//------------------------------------------------------------------------------
/**
    Cleanup the game world. This should undo the stuff in SetupWorld().
    Override this method in a subclass if your app needs different
    behaviour.
*/
void
SetupManager::CleanupWorld()
{
    // flush world state back into the database
    Game::Server::Instance()->Save();

    // clear all game entities
    EntityManager::Instance()->RemoveAllEntities();

    // clear physics and graphics level
    Physics::Server::Instance()->SetLevel(0);
    Graphics::Server::Instance()->SetLevel(0);

    // shutdown some subsystems
    Navigation::Server::Instance()->Close();
}

} // namespace Game
