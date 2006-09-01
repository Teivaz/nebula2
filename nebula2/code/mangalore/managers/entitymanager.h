#ifndef MANAGERS_ENTITYMANAGER_H
#define MANAGERS_ENTITYMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Managers::EntityManager

    The entity manager object keeps track of all active game entities
    and calls their per-frame-update methods to keep them alive. It
    also contains methods to iterate through existing entities.
    Derive from this class if your application needs different or
    more advanced game entity management, but make sure that all
    methods which are defined in entity manager still do the expected thing
    in your derived class.

    (C) 2005 Radon Labs GmbH
*/
#include "game/manager.h"
#include "foundation/ptr.h"
#include "util/narray.h"
#include "util/nkeyarray.h"
#include "game/entity.h"
#include "misc/nwatched.h"
#include "kernel/nprofiler.h"

//------------------------------------------------------------------------------
namespace Managers
{
class EntityManager : public Game::Manager
{
    DeclareRtti;
	DeclareFactory(EntityManager);

public:
    /// constructor
    EntityManager();
    /// destructor
    virtual ~EntityManager();
    /// get instance pointer
    static EntityManager* Instance();

    /// called per-frame by game server
    virtual void OnFrame();
    /// called if a render debug visualization is requested
    virtual void OnRenderDebug();

    /// attach an entity to the world
    void AttachEntity(Game::Entity* entity);
    /// remove an entity from the world
    void RemoveEntity(Game::Entity* entity);
    /// delete an entity from the world (also deletes the entity from DB!)
    void DeleteEntity(Game::Entity* entity);
    /// remove all entities from the world
    void RemoveAllEntities();

    /// return number of attached entities (live entities), also counts entities which get deleted this frame
    int GetNumEntities() const;
    /// get pointer to entity at index (live entities), may be 0 (representing an entity that will be deleted this frame)
    Game::Entity* GetEntityAt(int index) const;
    /// access to entity array (live entities), may contain Ptr(0) elements - entities that get deleted this frame
    const nArray<Ptr<Game::Entity> >& GetEntities() const;

    /// does the entity with the given id exists (only works for live entities)
    bool ExistsEntityById(uint id) const;
    /// get the entity for the given id (only works for live entities)
    Ptr<Game::Entity> GetEntityById(uint id) const;

    /// does the entity with the given GUID exists (liveOnly: search only the live entities)
    bool ExistsEntityByGuid(const nString& guid, bool liveOnly = false) const;
    /// get the entity with the given GUID (liveOnly: search only the live entities)
    Ptr<Game::Entity> GetEntityByGuid(const nString& guid, bool liveOnly = false);

    /// does the entity with the given name exists (liveOnly: search only the live entities)
    bool ExistsEntityByName(const nString& name, bool liveOnly = false) const;
    /// get the entity with the given name (liveOnly: search only the live entities)
    Ptr<Game::Entity> GetEntityByName(const nString& name, bool liveOnly = false);

    /// does one or more entities exist for the given attribute (liveOnly: search only the live entities)
    bool ExistsEntitiesByAttr(const Db::Attribute& attr, bool liveOnly = false) const;
    /// does one or more entities exist for the given attribute array (liveOnly: search only the live entities)
    bool ExistsEntitiesByAttrs(const nArray<Db::Attribute>& attributes, bool liveOnly = false) const;

    /// get the entities for the given attribute (liveOnly: search only the live entities)
    nArray<Ptr<Game::Entity> > GetEntitiesByAttr(const Db::Attribute& attr, bool liveOnly = false, bool onlyFirstEntity = false, bool failOnDBError = true);
    /// get the entities for the given attribute array (liveOnly: search only the live entities)
    nArray<Ptr<Game::Entity> > GetEntitiesByAttrs(const nArray<Db::Attribute>& attributes, bool liveOnly = false, bool onlyFirstEntity = false, bool failOnDBError = true);

private:
    static EntityManager* Singleton;

    /// is inside OnFrame
    bool isInOnFrame;

protected:
    /// remove entity from registry (handles late dissmiss)
    void RemoveEntityFromRegistry(Game::Entity* entity);
    /// add entity to registry (handles late dismiss)
    void AddEntityToReqistry(Game::Entity* entity);
    /// update the registry (cleanup dissmissed entitys, attach new entitys)
    void UpdateRegistry();

    /// create an on-demand sleeping entities
    nArray<Game::Entity*> CreateSleepingEntities(const nArray<Db::Attribute>& keyAttributes, const nArray<Ptr<Game::Entity> >& filteredEntitys, bool failOnDBError = true);

    nArray<Ptr<Game::Entity> > entities;
    nKeyArray<Game::Entity*> entityRegistry;

    nArray<Ptr<Game::Entity> > newEntitys;
    nArray<Ptr<Game::Entity> > removedEntitys;

    #if __NEBULA_STATS__
    nWatched statsNumEntities;
    nWatched statsNumLiveEntities;
    nWatched statsNumSleepingEntities;
    #endif

    PROFILER_DECLARE(profOnBeginFrame);
    PROFILER_DECLARE(profOnMoveBefore);
    PROFILER_DECLARE(profPhysics);
    PROFILER_DECLARE(profOnMoveAfter);
    PROFILER_DECLARE(profOnRender);
    PROFILER_DECLARE(profFrame);
    PROFILER_DECLARE(profUpdateRegistry);
};

RegisterFactory(EntityManager);

//------------------------------------------------------------------------------
/**
*/
inline
EntityManager*
EntityManager::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

} // namespace Managers
//------------------------------------------------------------------------------
#endif
