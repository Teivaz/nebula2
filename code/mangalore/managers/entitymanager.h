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
#ifdef __NEBULA_STATS__
#include "misc/nwatched.h"
#endif

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

    /// attach an entity to the world
    virtual void AttachEntity(Game::Entity* entity);
    /// remove an entity from the world
    virtual void RemoveEntity(Game::Entity* entity);
    /// remove all entities from the world
    virtual void RemoveAllEntities();
    /// return number of attached entities
    virtual int GetNumEntities() const;
    /// get pointer to entity at index
    virtual Game::Entity* GetEntityAt(int index) const;
    /// access to entity array
    const nArray<Ptr<Game::Entity> >& GetEntities() const;
    /// find a game entity by its id, will only return live entities
    virtual Game::Entity* FindEntityById(uint id);
    /// FIXME API! find an entity by its GUID, may return an entity in the sleeping pool
    virtual Game::Entity* FindEntityByGuid(const nString& guid, bool liveOnly=false);
    /// FIXME API! find a game entity by its name, may return sleeping entity
    virtual Game::Entity* FindEntityByName(const nString& n, bool liveOnly=false);
    /// return all entities which have an attribute set to a specific value, will only return live entities
    virtual nArray<Ptr<Game::Entity> > FindEntitiesByAttr(const Db::Attribute& attr);
    /// called per-frame by game server
    virtual void OnFrame();

private:
    static EntityManager* Singleton;

protected:
    /// create an on-demand sleeping entity
    Game::Entity* CreateSleepingEntity(const Db::Attribute& keyAttr);

    nArray<Ptr<Game::Entity> > entities;
    nKeyArray<Game::Entity*> entityRegistry;

    #if __NEBULA_STATS__
    nWatched statsNumEntities;
    nWatched statsNumLiveEntities;
    nWatched statsNumSleepingEntities;
    #endif
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
