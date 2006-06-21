//------------------------------------------------------------------------------
//  loader/navmaploader.h
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "loader/navmaploader.h"
#include "db/server.h"
#include "db/query.h"
#include "navigation/map.h"
#include "navigation/server.h"
#include "foundation/factory.h"
#include "attr/attributes.h"

namespace Loader
{

//------------------------------------------------------------------------------
/**
*/
bool
NavMapLoader::Load(const nString& levelName)
{
    Db::Server* dbServer = Db::Server::Instance();
    
    // create query which gives us the navmap of this level (if any)
    nString sql;
    sql.Format("SELECT * FROM _Navigation WHERE _Level='%s'", levelName.Get());
    Ptr<Db::Query> query = dbServer->CreateQuery(sql);
    query->Execute();
    if (query->GetNumRows() > 0)
    {
		Ptr<Navigation::Map> map = Navigation::Map::Create();
        nString filename = query->GetAttr(Attr::File, 0).GetString();
        bool success = map->Open(filename);
        if (!success)
        {
            n_error("NavigationMapStreamParser::Parse(): failed load load nav mesh '%s'!", filename.Get());
            return false;
        }
        Navigation::Server::Instance()->SetMap(map);
    }
    return true;
}

} // namespace Loader
