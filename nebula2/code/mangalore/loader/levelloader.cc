//------------------------------------------------------------------------------
//  loader/levelloader.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "loader/levelloader.h"
#include "loader/server.h"
#include "physics/server.h"
#include "physics/level.h"
#include "graphics/server.h"
#include "foundation/factory.h"
#include "util/celltreebuilder.h"
#include "db/server.h"
#include "db/query.h"
#include "attr/attributes.h"
#include "loader/environmentloader.h"
#include "loader/entityloader.h"
#include "loader/navmaploader.h"
#include "game/server.h"

namespace Loader
{

//------------------------------------------------------------------------------
/**
*/
bool
LevelLoader::Load(const nString& levelName)
{
    Db::Server* dbServer = Db::Server::Instance();

    // get the level data from the world database
    Ptr<Db::Query> levelQuery = dbServer->CreateLevelQuery(levelName);
    levelQuery->Execute();
    if (levelQuery->GetNumRows() == 0)
    {
        // unknown level...
        n_error("LevelLoader::Load(): level '%s' not found in world database!", levelName.Get());
        return false;
    }
    if (levelQuery->GetNumRows() > 1)
    {
        // more then one level of that name, corrupt database?
        n_error("LevelLoader::Load(): more then one level '%s' in world database!", levelName.Get());
        return false;
    }

    // setup new physics level
	Ptr<Physics::Level> physicsLevel = Physics::Level::Create();
    Physics::Server::Instance()->SetLevel(physicsLevel);

    // setup new graphics level
    vector3 levelCenter = levelQuery->GetAttr(Attr::Center, 0).GetVector3();
    vector3 levelExtents = levelQuery->GetAttr(Attr::Extents, 0).GetVector3();
    bbox3 levelBox(levelCenter, levelExtents);
	Ptr<Graphics::Level> graphicsLevel = Graphics::Level::Create();
    Util::CellTreeBuilder cellTreeBuilder;
    cellTreeBuilder.BuildQuadTree(graphicsLevel, 5, levelBox);
    Graphics::Server::Instance()->SetLevel(graphicsLevel);

    // load objects
    Db::Server::Instance()->BeginTransaction();

    // load entities from db
    Server::Instance()->LoadEntities(levelName);

    NavMapLoader::Load(levelName);
    Db::Server::Instance()->EndTransaction();

    // invoke OnLoad() on everything
    Game::Server::Instance()->Load();

    return true;
}

}; // namespace Loader
