//------------------------------------------------------------------------------
//  managers/entitymanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "managers/entitymanager.h"
#include "game/entity.h"
#include "attr/attributes.h"
#include "physics/server.h"
#include "managers/factorymanager.h"

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
    entityRegistry(1024, 1024)
{
    n_assert(0 == Singleton);
    Singleton = this;
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
    Attach a game entity to the entity manager. This means the entity is
    activated and added to the "world". The entity will be added to
    the internal entity array, and the method OnActivate() will be 
    invoked on the entity.
*/
void
EntityManager::AttachEntity(Entity* entity)
{
    n_assert(entity);
    entity->OnActivate();
    this->entities.Append(entity);
    this->entityRegistry.Add(entity->GetUniqueId(), entity);
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
    nArray<Ptr<Entity> >::iterator iter = this->entities.Find(entity);
    n_assert(iter);
    this->entityRegistry.Rem(entity->GetUniqueId());
    this->entities.Erase(iter);
}

//------------------------------------------------------------------------------
/**
    Remove all entities from the entity manager.
*/
void
EntityManager::RemoveAllEntities()
{
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
    return this->entities[index];
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
    Find entity by its unique id. Returns 0 is not attached to the 
    entity manager. This method will never return a sleeping entity.
*/
Entity*
EntityManager::FindEntityById(uint id)
{
    Entity* entity = 0;
    if (0 != id)
    {
        this->entityRegistry.Find(id, entity);
        if (entity && entity->GetEntityPool() == Entity::LivePool)
        {
            return entity;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Find an entity by its GUID. If there is no live entity with the given GUID,
    the method will try to create a sleeping entity from the database. If
    that fails as well, the method will return 0.
*/
Entity*
EntityManager::FindEntityByGuid(const nString& guid, bool liveOnly)
{
    Entity* entity = 0;
    int entityIndex;
    int numEntities = this->GetNumEntities();
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        entity = this->GetEntityAt(entityIndex);
        if (entity->HasAttr(Attr::GUID) && (entity->GetString(Attr::GUID) == guid))
        {
            return entity;
        }
    }
    if (!liveOnly)
    {
        // not found, try to create a new sleeping entity
        entity = this->CreateSleepingEntity(Db::Attribute(Attr::GUID, guid));
    }
    return entity;
}

//------------------------------------------------------------------------------
/**
    Find entity by name. If there is no live entity with the given name,
    the method will try to create a sleeping entity from the database. If
    that fails as well, the method will return 0.
*/
Entity*
EntityManager::FindEntityByName(const nString& name, bool liveOnly)
{
    Entity* entity = 0;
    int i;
    int num = this->entities.Size();
    for (i = 0; i < num; i++)
    {
        entity = this->entities[i];
        if (entity->HasAttr(Attr::Name))
        {
            if (entity->GetString(Attr::Name) == name)
            {
                return entity;
            }
        }
    }
    if (!liveOnly)
    {
        // not found, try to create a new sleeping entity
        entity = this->CreateSleepingEntity(Db::Attribute(Attr::Name, name));
    }
    return entity;
}

//------------------------------------------------------------------------------
/**
    Find all entities which have a specific attribute set to a specific value.
    This method will never return sleeping entities!
*/
nArray<Ptr<Entity> >
EntityManager::FindEntitiesByAttr(const Db::Attribute& attr)
{
    nArray<Ptr<Entity> > entities;
    int entityIndex;
    int numEntities = this->GetNumEntities();
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        Entity* entity = this->GetEntityAt(entityIndex);
        if (entity->HasAttr(attr.GetAttributeID()) && 
            (entity->GetAttr(attr.GetAttributeID()) == attr) &&
            entity->GetEntityPool() == Entity::LivePool)
        {
            entities.Append(entity);
        }
    }
    return entities;
}

//------------------------------------------------------------------------------
/**
    This method creates a new sleeping entity from the database using the
    provided key attribute (usually Attr::Name or Attr::GUID) and attaches
    it to the world. Please note that the method may return 0 if the entity
    is not found in the database!
*/
Entity*
EntityManager::CreateSleepingEntity(const Db::Attribute& keyAttr)
{
    Entity* entity = FactoryManager::Instance()->CreateEntityByKeyAttr(keyAttr, Entity::SleepingPool);
    if (entity)
    {
        this->AttachEntity(entity);
    }
    return entity;
}

//------------------------------------------------------------------------------
/**
    This method is called once per frame by the game server and implements
    the Game subsystem's game loop.
*/
void
EntityManager::OnFrame()
{
    // we work from a copy of the entity pointer array, that way,
    // entity can be savely removed from the entity pool at any
    // time (also during our trigger loops) without corrupting
    // the for loops
    // *************************************************************************
    // NOTE: We NEED to re-assign the entity-array after each
    // trigger run, in case entities have been removed
    // from the world during the trigger run. This is necessary
    // so that removed entities won't be triggered any more!!!
    // *************************************************************************

    // get stats
    #ifdef __NEBULA_STATS__
    int numEntities = this->entities.Size();
    int numLiveEntities = 0;
    int numSleepingEntities = 0;
    int i;
    for (i = 0; i < numEntities; i++)
    {
        if (this->entities[i]->GetEntityPool() == Entity::LivePool)
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
    nArray<Ptr<Game::Entity> > entityArray = this->entities;
    int entityIndex;
    for (entityIndex = 0; entityIndex < entityArray.Size(); entityIndex++)
    {
        if (entityArray[entityIndex]->GetEntityPool() == Entity::LivePool)
        {
            entityArray[entityIndex]->OnBeginFrame();
        }
    }

    // invoke OnMoveBefore() on all entities
    entityArray = this->entities;
    for (entityIndex = 0; entityIndex < entityArray.Size(); entityIndex++)
    {
        if (entityArray[entityIndex]->GetEntityPool() == Entity::LivePool)
        {
            entityArray[entityIndex]->OnMoveBefore();
        }
    }

    // trigger the physics subsystem
    Physics::Server::Instance()->Trigger();

    // invoke OnMoveAfter() on all entities
    entityArray = this->entities;
    for (entityIndex = 0; entityIndex < entityArray.Size(); entityIndex++)
    {
        if (entityArray[entityIndex]->GetEntityPool() == Entity::LivePool)
        {
            entityArray[entityIndex]->OnMoveAfter();
        }
    }

    // invoke OnRender() on all entities
    entityArray = this->entities;
    for (entityIndex = 0; entityIndex < entityArray.Size(); entityIndex++)
    {
        if (entityArray[entityIndex]->GetEntityPool() == Entity::LivePool)
        {
            entityArray[entityIndex]->OnRender();
        }
    }
}

} // namespace Managers
