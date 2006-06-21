//------------------------------------------------------------------------------
//  loader/entityloader.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "loader/entityloader.h"
#include "db/query.h"
#include "db/server.h"
#include "game/entity.h"
#include "managers/factorymanager.h"
#include "managers/entitymanager.h"
#include "attr/attributes.h"

namespace Loader
{
using namespace Game;
using namespace Managers;

ImplementRtti(EntityLoader, EntityLoaderBase);
ImplementFactory(EntityLoader);

//------------------------------------------------------------------------------
/**
*/
EntityLoader::EntityLoader()
{
}

//------------------------------------------------------------------------------
/**
*/
EntityLoader::~EntityLoader()
{
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityLoader::Load(const nString& levelName)
{
    Db::Server* dbServer = Db::Server::Instance();

    // for each category...
    Ptr<Db::Query> categoryQuery = dbServer->CreateCategoriesQuery();
    categoryQuery->Execute();
    int categoryIndex = 0;
    int numCategories = categoryQuery->GetNumRows();
    for (categoryIndex = 0; categoryIndex < numCategories; categoryIndex++)
    {
        nString curCategory = categoryQuery->GetAttr(Attr::Name, categoryIndex).GetString();
        
        // !!!!!!!!!!!!!!!!!!!!!!!!
        // HACK: Skip tree category
        if (curCategory != "Tree")
        {
            // get guids of all objects in current level of current category
            Ptr<Db::Query> entityQuery = dbServer->CreateLevelCategoryGuidQuery(levelName, curCategory);
            entityQuery->Execute();

            // for each entity...
            int entityIndex;
            int numEntities = entityQuery->GetNumRows();
            for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
            {
                // create entity by guid, this will fully initialize the entity from the database
                nString guid = entityQuery->GetAttr(Attr::GUID, entityIndex).GetString();
                Ptr<Entity> gameEntity = FactoryManager::Instance()->CreateEntityByGuid(guid);

                // attach the entity to the world
                EntityManager::Instance()->AttachEntity(gameEntity);            
            }
        }
    }
    return true;
}

} // namespace Loader


