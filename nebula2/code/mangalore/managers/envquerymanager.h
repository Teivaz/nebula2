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
    /// get the entities under the mouse drag drop rectangle area
    virtual void GetEntitiesUnderMouseDragDropRect(vector2 dragPosition, vector2 dropPosition, nArray<Ptr<Game::Entity> >& entities);
    /// get the mouse position in the 3d world
    virtual const vector3& GetMousePos3d() const;
    /// get the upVector of the face under the mouse cursor
    virtual const vector3& GetUpVector() const;
    /// return true if mouse is over "something"
    virtual bool HasMouseIntersection() const;
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

//------------------------------------------------------------------------------
/**
    This returns the position where a vector through the mouse position
    intersects the 3d world (or the nearest entity). If the mouse doesn't
    intersect, the result will be undefined, and the method
    HasMouseIntersection() returns false.
*/
inline
const vector3&
EnvQueryManager::GetMousePos3d() const
{
    return this->mousePos3d;
}

//------------------------------------------------------------------------------
/**
    This returns the upvector of the face under the mousecursor.
    If the mouse doesn't intersect, the result will be undefined,
    and the method HasMouseIntersection() returns false.
*/
inline
const vector3&
EnvQueryManager::GetUpVector() const
{
    return this->upVector;
}

//------------------------------------------------------------------------------
/**
    Returns true if the vector through the current mouse position intersects
    the world, or an entity, false if no intersection exists.
*/
inline
bool
EnvQueryManager::HasMouseIntersection() const
{
    return this->mouseIntersection;
}

} // namespace Managers
//------------------------------------------------------------------------------
#endif
