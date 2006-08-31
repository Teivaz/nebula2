//------------------------------------------------------------------------------
//  loader/levelloader.cc
//  (C) 2006 RadonLabs GmbH
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
#include "loader/server.h"

namespace Loader
{

ImplementRtti(Loader::LevelLoader, Foundation::RefCounted);
ImplementFactory(Loader::LevelLoader);

//------------------------------------------------------------------------------
/**
*/
bool
LevelLoader::Load(const nString& levelName)
{
    Db::Server* dbServer = Db::Server::Instance();
    Loader::Server* loaderServer = Loader::Server::Instance();

    // update progress bar window
    loaderServer->SetProgressText("Query Database...");
    loaderServer->UpdateProgressDisplay();

    // get the level data from the world database
    Db::Server::Instance()->BeginTransaction();
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

    // load objects through a database reader, that way, only
    // one query is necessary for all objects
    Ptr<Db::Reader> dbReader = Db::Reader::Create();
    dbReader->SetTableName("_Entities");
    dbReader->AddFilterAttr(Db::Attribute(Attr::_Type, "INSTANCE"));
    dbReader->AddFilterAttr(Db::Attribute(Attr::_Level, levelName));
    if (dbReader->Open())
    {
        loaderServer->SetMaxProgressValue(dbReader->GetNumRows());
        Server::Instance()->LoadEntities(dbReader);
        dbReader->Close();
    }

    // update progress bar window
    loaderServer->SetProgressText("Loading Navigation Map...");
    loaderServer->UpdateProgressDisplay();
    NavMapLoader::Load(levelName);

    Db::Server::Instance()->EndTransaction();

    loaderServer->SetProgressText("Level Loader Done...");
    loaderServer->UpdateProgressDisplay();

    return true;
}

} // namespace Loader
