#ifndef MANAGERS_ENVQUERYMANAGER_H
#define MANAGERS_ENVQUERYMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Game::EnvQueryManager

    The EnvQueryManager implements environment queries into the game world,
    like stabbing queries, line-of-sight checks, etc...

    (C) 2005 Radon Labs GmbH
*/
#include "game/manager.h"
#include "game/entity.h"
#include "mathlib/rectangle.h"

//------------------------------------------------------------------------------
namespace Managers
{
class EnvQueryManager : public Game::Manager
{
    DeclareRtti;
	DeclareFactory(EnvQueryManager);

public:
    /// constructor
    EnvQueryManager();
    /// destructor
    virtual ~EnvQueryManager();
    /// get instance pointer
    static EnvQueryManager* Instance();

    /// get the entity under the mouse cursor
    virtual Game::Entity* GetEntityUnderMouse() const;
    /// get the mouse position in the 3d world
    virtual const vector3& GetMousePos3d() const;
    /// get the upVector of the face under the mouse cursor
    virtual const vector3& GetUpVector() const;
    /// return true if mouse is over "something"
    virtual bool HasMouseIntersection() const;
    /// get all entities in a given spherical area
    virtual nArray<Ptr<Game::Entity> > GetEntitiesInSphere(const vector3& midPoint, float radius);
    /// get all entities in a given box shaped area
    virtual nArray<Ptr<Game::Entity> > GetEntitiesInBox(const vector3& scale, const matrix44& m); 
    /// called per-frame by game server
    virtual void OnFrame();

private:
    static EnvQueryManager* Singleton;

protected:
    Game::Entity::EntityId entityUnderMouse;
    vector3 mousePos3d;
    vector3 upVector;
    bool mouseIntersection;
};

RegisterFactory(EnvQueryManager);

//------------------------------------------------------------------------------
/**
*/
inline
EnvQueryManager*
EnvQueryManager::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

} // namespace Managers
//------------------------------------------------------------------------------
#endif
