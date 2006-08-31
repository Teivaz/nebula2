//------------------------------------------------------------------------------
//  loader/entityloader.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "loader/entityloader.h"
#include "db/query.h"
#include "db/server.h"
#include "game/entity.h"
#include "managers/factorymanager.h"
#include "managers/entitymanager.h"
#include "attr/attributes.h"
#include "loader/server.h"

namespace Loader
{
using namespace Game;
using namespace Managers;

ImplementRtti(Loader::EntityLoader, Loader::EntityLoaderBase);
ImplementFactory(Loader::EntityLoader);

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
EntityLoader::Load(Db::Reader* dbReader)
{
    n_assert(0 != dbReader);

    const nString treeCategory("Tree");
    const nString envCategory("_Environment");

    // for each row in the reader object
    int rowIndex;
    int numRows = dbReader->GetNumRows();
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        dbReader->SetToRow(rowIndex);

        // get category of current row...
        nString category = dbReader->GetString(Attr::_Category);

        // FIXME: HACK, skip Tree and _Environment category (handled by different loader)
        if ((category != treeCategory) && (category != envCategory))
        {
            // create entity
            Ptr<Entity> gameEntity = FactoryManager::Instance()->CreateEntityByDbReader(dbReader, rowIndex);

            // update progress indicator
            this->UpdateProgressIndicator(gameEntity);

            // attach the entity to the world
            EntityManager::Instance()->AttachEntity(gameEntity);
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
EntityLoader::UpdateProgressIndicator(Game::Entity* gameEntity)
{
    n_assert(gameEntity);
    Loader::Server* loaderServer = Loader::Server::Instance();

    nString id;
    if (gameEntity->HasAttr(Attr::Id))
    {
        id = gameEntity->GetString(Attr::Id);
    }
    nString category;
    if (gameEntity->HasAttr(Attr::_Category))
    {
        category = gameEntity->GetString(Attr::_Category);
    }
    nString graphics;
    if (gameEntity->HasAttr(Attr::Graphics))
    {
        graphics = gameEntity->GetString(Attr::Graphics);
    }
    nString progressText;
    progressText.Format("Loading game entity '%s' (category='%s', gfx=%s)...", id.Get(), category.Get(), graphics.Get());
    loaderServer->SetProgressText(progressText);
    loaderServer->AdvanceProgress(1);
    loaderServer->UpdateProgressDisplay();
}

} // namespace Loader


