//------------------------------------------------------------------------------
//  managers/entitymanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "managers/entitymanager.h"
#include "game/entity.h"
#include "game/server.h"
#include "attr/attributes.h"
#include "physics/server.h"
#include "managers/factorymanager.h"
#include "db/server.h"
#include "db/query.h"

namespace Managers
{
ImplementRtti(Managers::EntityManager, Game::Manager);
ImplementFactory(Managers::EntityManager);

EntityManager* EntityManager::Singleton = 0;

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
EntityManager::EntityManager() :
    #ifdef __NEBULA_STATS__
    statsNumEntities("mangaNumEntities", nArg::Int),
    statsNumLiveEntities("mangaNumLiveEntities", nArg::Int),
    statsNumSleepingEntities("mangaNumSleepingEntities", nArg::Int),
    #endif
    entities(256, 256),
    entityRegistry(1024, 1024),
    isInOnFrame(false)
{
    n_assert(0 == Singleton);
    Singleton = this;

    PROFILER_INIT(this->profOnBeginFrame, "profMangaEntityManagerBeginFrame");
    PROFILER_INIT(this->profOnMoveBefore, "profMangaEntityManagerMoveBefore");
    PROFILER_INIT(this->profPhysics, "profMangaEntityManagerPhysics");
    PROFILER_INIT(this->profOnMoveAfter, "profMangaEntityManagerMoveAfter");
    PROFILER_INIT(this->profOnRender, "profMangaEntityManagerRender");
    PROFILER_INIT(this->profFrame, "profMangaEntityManagerFrame");
    PROFILER_INIT(this->profUpdateRegistry, "profMangaEntityManagerUpdateRegistry");
}

//------------------------------------------------------------------------------
/**
*/
EntityManager::~EntityManager()
{
    n_assert(0 == this->entities.Size());
    n_assert(0 == this->entityRegistry.Size());
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    To make it possible to remove a entity while looping over the entity array
    only the ptr will set to 0 (to not change the array layout).
    Those 0 ptr's will be ignored in the OnFrame loops, and cleared on end of
    frame.
*/
void
EntityManager::RemoveEntityFromRegistry(Game::Entity* entity)
{
    n_assert(entity);

    if (this->isInOnFrame)
    {
        // [mse] 16.05.2006
        // look first in new entities, if entity was added in the same frame
        nArray<Ptr<Entity> >::iterator iter = this->newEntitys.Find(entity);
        if(0 != iter)
        {
            this->newEntitys.Erase(iter);
        }
        else
        {
            // save removed entity in array (to make shure the entity does not get destroyed until end of frame)
            this->removedEntitys.Append(entity);

            // inside OnFrame, just set the ptrs to 0, so the entity is not longer triggered
            this->entityRegistry.GetElement(entity->GetUniqueId()) = 0;

            nArray<Ptr<Entity> >::iterator iter = this->entities.Find(entity);
            n_assert(iter);
            iter->operator =(0);
        }
    }
    else
    {
        // direct remove entity from arrays
        this->entityRegistry.Rem(entity->GetUniqueId());

        nArray<Ptr<Entity> >::iterator iter = this->entities.Find(entity);
        n_assert(iter);
        this->entities.Erase(iter);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EntityManager::AddEntityToReqistry(Game::Entity* entity)
{
    n_assert(entity);

    if (this->isInOnFrame)
    {
        // do not add the entity directly, to prevent a partly OnFrame trigger
        this->newEntitys.Append(entity);
    }
    else
    {
        // out side the OnFrame trigger, direct add the entity
        this->entities.Append(entity);
        this->entityRegistry.Add(entity->GetUniqueId(), entity);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EntityManager::UpdateRegistry()
{
    // cleanup the removed entitys, they should be destroyed now
    this->removedEntitys.Clear();

    int entityIndex;
    // remove all 0 ptrs from ID KeyArray
    for (entityIndex = 0; entityIndex < this->entityRegistry.Size(); /*empty*/)
    {
        if (this->entityRegistry.GetElementAt(entityIndex) == 0)
        {
            this->entityRegistry.RemByIndex(entityIndex);
        }
        else
        {
            // next element
            entityIndex++;
        }
    }

    // remove 0 ptrs from entity array and add new entitys
    for (entityIndex = 0; entityIndex < this->entities.Size(); /*empty*/)
    {
        if (this->entities[entityIndex] != 0)
        {
            // next entity
            entityIndex ++;
        }
        else
        {
            // 0 ptr found
            if (this->newEntitys.Size() > 0)
            {
                // move one of the new entity at this 0 ptr
                this->entities[entityIndex] = this->newEntitys.Back();
                this->newEntitys.Erase(this->newEntitys.Size() - 1);

                // add to ID KeyArray
                this->entityRegistry.Add(this->entities[entityIndex]->GetUniqueId(), this->entities[entityIndex]);

                // next entity
                entityIndex ++;
            }
            else
            {
                // remove the 0 ptr
                this->entities.Erase(entityIndex);

                // not next entity
            }
        }
    }

    // remove remaining 0 ptrs from entity array
    while (this->newEntitys.Size() > 0)
    {
        // move remaining new entitys to entity array
        this->entities.Append(this->newEntitys.Back());
        this->newEntitys.Erase(this->newEntitys.Size() - 1);

        // add to ID KeyArray
        this->entityRegistry.Add(this->entities.Back()->GetUniqueId(), this->entities.Back());
    }
}

//------------------------------------------------------------------------------
/**
    Attach a game entity to the entity manager. This means the entity is
    activated and added to the "world". The entity will be added to
    the internal entity array, and the method OnActivate() will be
    invoked on the entity.

    Invoke OnStart if the game world is already running.
*/
void
EntityManager::AttachEntity(Entity* entity)
{
    n_assert(entity);
    entity->OnActivate();

    this->AddEntityToReqistry(entity);

    if (Game::Server::Instance()->HasStarted())
    {
        entity->OnStart();
    }
}

//------------------------------------------------------------------------------
/**
    Remove a game entity from the entity manager. This basically removes
    the entity from the world. This invokes the method OnDeactivate() on the
    entity and removes the entity from internal arrays.
*/
void
EntityManager::RemoveEntity(Entity* entity)
{
    n_assert(entity);
    entity->OnDeactivate();

    this->RemoveEntityFromRegistry(entity);
}

//------------------------------------------------------------------------------
/**
    Delete a game entity, this removes the entity from the world and also delete
    the DB record of this entity.
*/
void
EntityManager::DeleteEntity(Entity* entity)
{
    n_assert(entity);
    n_assert(entity->HasAttr(Attr::GUID));

    // to make sure, the entity still exists, when removing from level
    Ptr<Entity> lastPtr = entity;

    // remove the entity from active world
    this->RemoveEntity(entity);

    // create a delete query for this entity
    Db::Server* dbServer = Db::Server::Instance();
    Ptr<Db::Query> deleteQuery = dbServer->CreateQuery();
    deleteQuery->SetTableName("_Entities");
    deleteQuery->AddWhereAttr(entity->GetAttr(Attr::GUID));

    // remove from DB (don't fail on error)
    deleteQuery->BuildDeleteStatement();
    deleteQuery->Execute(false);
}

//------------------------------------------------------------------------------
/**
    Remove all entities from the entity manager.
*/
void
EntityManager::RemoveAllEntities()
{
    n_assert(!this->isInOnFrame); // make sure the entitys will be deleted, or this would be a endless loop!
    while (this->entities.Size() > 0)
    {
        this->RemoveEntity(this->entities.Back());
    }
}

//------------------------------------------------------------------------------
/**
    Return number of entities attached to the entity manager.
*/
int
EntityManager::GetNumEntities() const
{
    return this->entities.Size();
}

//------------------------------------------------------------------------------
/**
    Get entity at given index.
*/
Entity*
EntityManager::GetEntityAt(int index) const
{
    return this->entities[index].get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Access to entity array.
*/
const nArray<Ptr<Entity> >&
EntityManager::GetEntities() const
{
    return this->entities;
}

//------------------------------------------------------------------------------
/**
    This method is called once per frame by the game server and implements
    the Game subsystem's game loop.
*/
void
EntityManager::OnFrame()
{
    PROFILER_START(this->profFrame);
    n_assert(!this->isInOnFrame);
    this->isInOnFrame = true;

    int entityIndex;
    int numEntities = this->entities.Size();

    // get stats
    #if __NEBULA_STATS__
    int numLiveEntities = 0;
    int numSleepingEntities = 0;
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        if (this->entities[entityIndex]->GetEntityPool() == Entity::LivePool)
        {
            numLiveEntities++;
        }
        else
        {
            numSleepingEntities++;
        }
    }
    this->statsNumEntities->SetI(numEntities);
    this->statsNumLiveEntities->SetI(numLiveEntities);
    this->statsNumSleepingEntities->SetI(numSleepingEntities);
    #endif

    // invoke OnBeginFrame() on all entities
    PROFILER_START(this->profOnBeginFrame);
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        if (this->entities[entityIndex].isvalid())
        {
            if (this->entities[entityIndex]->GetEntityPool() == Entity::LivePool)
            {
                this->entities[entityIndex]->OnBeginFrame();
            }
        }
    }
    PROFILER_STOP(this->profOnBeginFrame);

    // invoke OnMoveBefore() on all entities
    PROFILER_START(this->profOnMoveBefore);
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        if (this->entities[entityIndex] != 0)
        {
            if (this->entities[entityIndex]->GetEntityPool() == Entity::LivePool)
            {
                this->entities[entityIndex]->OnMoveBefore();
            }
        }
    }
    PROFILER_STOP(this->profOnMoveBefore);

    // trigger the physics subsystem
    PROFILER_START(this->profPhysics);
    Physics::Server::Instance()->Trigger();
    PROFILER_STOP(this->profPhysics);

    // invoke OnMoveAfter() on all entities
    PROFILER_START(this->profOnMoveAfter);
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        if (this->entities[entityIndex] != 0)
        {
            if (this->entities[entityIndex]->GetEntityPool() == Entity::LivePool)
            {
                this->entities[entityIndex]->OnMoveAfter();
            }
        }
    }
    PROFILER_STOP(this->profOnMoveAfter);

    // invoke OnRender() on all entities
    PROFILER_START(this->profOnRender);
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        if (this->entities[entityIndex] != 0)
        {
            if (this->entities[entityIndex]->GetEntityPool() == Entity::LivePool)
            {
                this->entities[entityIndex]->OnRender();
            }
        }
    }
    PROFILER_STOP(this->profOnRender);

    n_assert(this->isInOnFrame);
    this->isInOnFrame = false;

    // update the reqistry, delete removed entitys, append new entitys, cleanup 0 ptrs
    PROFILER_START(this->profUpdateRegistry);
    this->UpdateRegistry();
    PROFILER_STOP(this->profUpdateRegistry);

    PROFILER_STOP(this->profFrame);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Entity>
EntityManager::GetEntityById(uint id) const
{
    Entity* entity = 0;
    this->entityRegistry.Find(id, entity);
    if (entity && entity->GetEntityPool() == Entity::LivePool)
    {
        return entity;
    }
    return Ptr<Entity>();
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityManager::ExistsEntityById(uint id) const
{
    Game::Entity* entity = 0;
    this->entityRegistry.Find(id, entity);
    if (entity && entity->GetEntityPool() == Entity::LivePool)
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityManager::ExistsEntityByGuid(const nString& guid, bool liveOnly) const
{
    Db::Attribute dbAttr(Attr::GUID);
    dbAttr.SetString(guid);

    return this->ExistsEntitiesByAttr(dbAttr, liveOnly);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Entity>
EntityManager::GetEntityByGuid(const nString& guid, bool liveOnly)
{
    Db::Attribute dbAttr(Attr::GUID);
    dbAttr.SetString(guid);

    nArray<Ptr<Game::Entity> > entitys = this->GetEntitiesByAttr(dbAttr, liveOnly, true);

    if (entitys.Size() > 1)
    {
        n_error("EntityManager::GetEntityByGuid: found %i entities the GUID '%s'! GUID must be unique!", entitys.Size(), guid.Get());
    }
    else if (entitys.Size() == 1)
    {
        return entitys[0];
    }
    return Ptr<Entity>();
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityManager::ExistsEntityByName(const nString& name, bool liveOnly) const
{
    Db::Attribute dbAttr(Attr::Name);
    dbAttr.SetString(name);

    return this->ExistsEntitiesByAttr(dbAttr, liveOnly);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Entity>
EntityManager::GetEntityByName(const nString& name, bool liveOnly)
{
    Db::Attribute dbAttr(Attr::Name);
    dbAttr.SetString(name);

    nArray<Ptr<Game::Entity> > entitys = this->GetEntitiesByAttr(dbAttr, liveOnly, true);

    if (entitys.Size() > 1)
    {
        n_error("EntityManager::GetEntityByName: found %i entities with the name '%s'! name must be unique!", entitys.Size(), name.Get());
    }
    else if (entitys.Size() == 1)
    {
        return entitys[0];
    }
    return Ptr<Entity>();
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityManager::ExistsEntitiesByAttr(const Db::Attribute& attr, bool liveOnly) const
{
    nArray<Db::Attribute> attributes;
    attributes.Append(attr);

    return this->ExistsEntitiesByAttrs(attributes, liveOnly);
}

//------------------------------------------------------------------------------
/**
*/
nArray<Ptr<Game::Entity> >
EntityManager::GetEntitiesByAttr(const Db::Attribute& attr, bool liveOnly, bool onlyFirstEntity, bool failOnDBError)
{
    nArray<Db::Attribute> attributes;
    attributes.Append(attr);

    return this->GetEntitiesByAttrs(attributes, liveOnly, onlyFirstEntity, failOnDBError);
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityManager::ExistsEntitiesByAttrs(const nArray<Db::Attribute>& attributes, bool liveOnly) const
{
    // search in the active entitys
    int entityIndex;
    int numEntities = this->GetNumEntities();
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        Entity* entity = this->GetEntityAt(entityIndex);
        // entity exist?
        if (entity)
        {
            // is in the right pool?
            if (!liveOnly || entity->GetEntityPool() == Entity::LivePool)
            {
                // has all attribute?
                int attributeIndex;
                bool hasAllAttributes = true;
                for (attributeIndex = 0; attributeIndex < attributes.Size(); attributeIndex++)
                {
                    if (!entity->HasAttr(attributes[attributeIndex].GetAttributeID())
                        || (entity->GetAttr(attributes[attributeIndex].GetAttributeID()) != attributes[attributeIndex]))
                    {
                        hasAllAttributes = false;
                        break;
                    }
                }

                if (hasAllAttributes)
                {
                    return true; // found one
                }
            }
        }
    }

    // search in the db
    Ptr<Db::Query> dbQuery = Db::Server::Instance()->CreateQuery();
    dbQuery->SetTableName("_Entities");
    dbQuery->AddWhereAttr(Db::Attribute(Attr::_Type, nString("INSTANCE")));

    // add attributes as where clause
    int i;
    for (i = 0; i < attributes.Size(); i++)
    {
        dbQuery->AddWhereAttr(attributes[i]);
    }
    // define results
    dbQuery->AddResultAttr(Attr::GUID);
    dbQuery->BuildSelectStatement();

    if (dbQuery->Execute())
    {
        return dbQuery->GetNumRows() > 0;
    }

    // fallthrough, something went wrong
    nString err;
    for (i = 0; i < attributes.Size(); i++)
    {
        err.Append(attributes[i].GetName());
        err.Append("=");
        err.Append(attributes[i].AsString());
        err.Append(" ");
    }
    n_error("Managers::EntityManager::ExistsEntitiesByAttrs(): failed to execute query with keys '%s' into world database!", err.Get());
    return false;
}

//------------------------------------------------------------------------------
/**
    Generic function to find entities by attributes.

    @param liveOnly         set if only live enities are requested
    @param onlyFirstEntity  set to stop search if the 1st entity was found

*/
nArray<Ptr<Game::Entity> >
EntityManager::GetEntitiesByAttrs(const nArray<Db::Attribute>& attributes, bool liveOnly, bool onlyFirstEntity, bool failOnDBError)
{
    // collect active entitys
    nArray<Ptr<Entity> > entities;
    int entityIndex;
    int numEntities = this->GetNumEntities();
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        Entity* entity = this->GetEntityAt(entityIndex);
        // entity exist?
        if (entity)
        {
            // is in the right pool?
            if (!liveOnly || entity->GetEntityPool() == Entity::LivePool)
            {
                // has all attribute?
                int attributeIndex;
                bool hasAllAttributes = true;
                for (attributeIndex = 0; attributeIndex < attributes.Size(); attributeIndex++)
                {
                    if (!entity->HasAttr(attributes[attributeIndex].GetAttributeID())
                        || (entity->GetAttr(attributes[attributeIndex].GetAttributeID()) != attributes[attributeIndex]))
                    {
                        hasAllAttributes = false;
                        break;
                    }
                }

                if (hasAllAttributes)
                {
                    entities.Append(entity);
                    if (onlyFirstEntity)
                    {
                        return entities;
                    }
                }
            }
        }
    }

    if (!liveOnly)
    {
        // get entitys from db
        nArray<Entity*> dbEntitys = this->CreateSleepingEntities(attributes, entities, failOnDBError);
        int i;
        for (i = 0; i < dbEntitys.Size(); i++)
        {
            entities.Append(dbEntitys[i]);
        }
    }

    return entities;
}

//------------------------------------------------------------------------------
/**
    This method creates a new sleeping entities from the database using the
    provided key attributes (usually Attr::Name or Attr::GUID) and attaches
    it to the world. Please note that the method may return a array with 0
    elements if such entities are not found in the database!

    If there are filteredEntitys those will be filtered in the DB querty by
    GUID.
*/
nArray<Entity*>
EntityManager::CreateSleepingEntities(const nArray<Db::Attribute>& keyAttributes, const nArray<Ptr<Game::Entity> >& filteredEntitys, bool failOnDBError)
{
    nArray<Entity*> entities = FactoryManager::Instance()->CreateEntitiesByKeyAttrs(keyAttributes, filteredEntitys, Entity::SleepingPool, failOnDBError);
    int i;
    for (i = 0; i < entities.Size(); i++)
    {
        this->AttachEntity(entities[i]);
    }
    return entities;
}

//------------------------------------------------------------------------------
/**
*/
void
EntityManager::OnRenderDebug()
{
    int entityIndex;
    int numEntities = this->entities.Size();

    // invoke OnRenderDebug() on all entities
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        if (this->entities[entityIndex] != 0)
        {
            if (this->entities[entityIndex]->GetEntityPool() == Entity::LivePool)
            {
                this->entities[entityIndex]->OnRenderDebug();
            }
        }
    }
}
} // namespace Managers
