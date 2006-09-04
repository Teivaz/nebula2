//------------------------------------------------------------------------------
//  managers/envquerymanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#define MANGALORE_USE_CEGUI
#include "managers/envquerymanager.h"
#include "managers/entitymanager.h"
#include "physics/server.h"
#include "graphics/server.h"
#include "input/server.h"
#include "gui/nguiserver.h"
#include "ui/server.h"
#ifdef MANGALORE_USE_CEGUI
#include "ceui/server.h"
#endif

namespace Managers
{
ImplementRtti(Managers::EnvQueryManager, Game::Manager);
ImplementFactory(Managers::EnvQueryManager);

EnvQueryManager* EnvQueryManager::Singleton = 0;

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
EnvQueryManager::EnvQueryManager() :
    entityUnderMouse(0),
    mouseIntersection(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
EnvQueryManager::~EnvQueryManager()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    This returns a pointer to the entity under the mouse, or 0 if mouse is
    not over an entity.
*/
Entity*
EnvQueryManager::GetEntityUnderMouse() const
{
    return EntityManager::Instance()->GetEntityById(this->entityUnderMouse).get_unsafe();
}

//------------------------------------------------------------------------------
/**
   Get the entities under the mouse drag drop rectangle area
*/
void
EnvQueryManager::GetEntitiesUnderMouseDragDropRect(const rectangle& dragDropRect, nArray<Ptr<Game::Entity> >& entities)
{
    line3 ray = nGfxServer2::Instance()->ComputeWorldMouseRay(dragDropRect.midpoint(), 5000.0f);
    float angleOfView = nGfxServer2::Instance()->GetCamera().GetAngleOfView();

    nArray<Ptr<Graphics::Entity> > graphicsEntities;
    Graphics::Server::Instance()->DragDropSelect(ray.end(), angleOfView*dragDropRect.width(), dragDropRect.width()/dragDropRect.height(), graphicsEntities);

    EntityManager* entityMgr = EntityManager::Instance();
    for (int i = 0; i < graphicsEntities.Size(); i++)
    {
        int entityId = graphicsEntities[i]->GetUserData();
        if (entityId)
        {
            n_assert(entityMgr->ExistsEntityById(entityId));
            entities.PushBack(entityMgr->GetEntityById(entityId));
        }
    }
}

//------------------------------------------------------------------------------
/**
    This returns the position where a vector through the mouse position
    intersects the 3d world (or the nearest entity). If the mouse doesn't
    intersect, the result will be undefined, and the method
    HasMouseIntersection() returns false.
*/
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
bool
EnvQueryManager::HasMouseIntersection() const
{
    return this->mouseIntersection;
}

//------------------------------------------------------------------------------
/**
    Returns all game entities which intersect the given sphere. Uses the
    physics subsystem to do the query.
*/
nArray<Ptr<Game::Entity> >
EnvQueryManager::GetEntitiesInSphere(const vector3& midPoint, float radius)
{
    nArray<Ptr<Game::Entity> > gameEntities;
    Physics::FilterSet excludeSet;
    nArray<Ptr<Physics::Entity> > physicsEntities;
    Physics::Server::Instance()->GetEntitiesInSphere(midPoint, radius, excludeSet, physicsEntities);

    // convert physics entities back into game entities
    EntityManager* entityManager = EntityManager::Instance();
    int num = physicsEntities.Size();
    for (int i = 0; i < num; i++)
    {
        if (entityManager->ExistsEntityById(physicsEntities[i]->GetUserData()))
        {
            gameEntities.Append(entityManager->GetEntityById(physicsEntities[i]->GetUserData()));
        }
    }
    return gameEntities;
}
//------------------------------------------------------------------------------
/**
    Returns all game entities which intersect the given box. Uses the
    physics subsystem to do the query.
*/
nArray<Ptr<Game::Entity> >
EnvQueryManager::GetEntitiesInBox(const vector3& scale, const matrix44& m)
{
    nArray<Ptr<Game::Entity> > gameEntities;
    Physics::FilterSet excludeSet;
    nArray<Ptr<Physics::Entity> > physicsEntities;
    Physics::Server::Instance()->GetEntitiesInBox(scale, m, excludeSet, physicsEntities);

    // convert physics entities back into game entities
    EntityManager* entityManager = EntityManager::Instance();
    int num = physicsEntities.Size();
    for (int i = 0; i < num; i++)
    {
        if (entityManager->ExistsEntityById(physicsEntities[i]->GetUserData()))
        {
            gameEntities.Append(entityManager->GetEntityById(physicsEntities[i]->GetUserData()));
        }
    }
    return gameEntities;
}
//------------------------------------------------------------------------------
/**
    This method is called per-frame by the game server and updates the
    current values, like entity-under-mouse, 3d mouse position, etc...
*/
void
EnvQueryManager::OnFrame()
{
    Physics::Server* physicsServer = Physics::Server::Instance();
    Input::Server* inputServer = Input::Server::Instance();

    // reset values
    this->entityUnderMouse = 0;
    this->mousePos3d.set(0.0f, 0.0f, 0.0f);
    this->mouseIntersection = false;

    // get 3d contact under mouse
    if (!nGuiServer::Instance()->IsMouseOverGui() && !UI::Server::Instance()->IsMouseOverGui()
#ifdef MANGALORE_USE_CEGUI
        && !CEUI::Server::Instance()->IsMouseOverGui()
#endif
    ) {
        const vector2& mousePos = inputServer->GetMousePos();
        const float rayLength = 5000.0f;
        const Physics::ContactPoint* contact = 0;
        Physics::FilterSet excludeSet;
        contact = physicsServer->GetClosestContactUnderMouse(mousePos, rayLength, excludeSet);
        if (contact)
        {
            // store intersection position
            this->mousePos3d = contact->GetPosition();
            this->upVector = contact->GetUpVector();
            this->mouseIntersection = true;

            // new entity under mouse?
            Physics::Entity* physicsEntity = physicsServer->FindEntityByUniqueId(contact->GetEntityId());
            Game::Entity::EntityId gameEntityUnderMouse = 0;
            if (physicsEntity)
            {
                // user data of physics entity is unique id of game entity which owns the
                // physics entity
                gameEntityUnderMouse = physicsEntity->GetUserData();
            }

            if (gameEntityUnderMouse != this->entityUnderMouse)
            {
                this->entityUnderMouse = gameEntityUnderMouse;
            }
        }
    }
}

} // namespace Managers
