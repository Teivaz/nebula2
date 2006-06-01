//------------------------------------------------------------------------------
//  managers/envquerymanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "managers/envquerymanager.h"
#include "managers/entitymanager.h"
#include "physics/server.h"
#include "input/server.h"
#include "gui/nguiserver.h"
#include "ui/server.h"

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
    return EntityManager::Instance()->FindEntityById(this->entityUnderMouse);
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
    if (!nGuiServer::Instance()->IsMouseOverGui() && !UI::Server::Instance()->IsMouseOverGui())
    {
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

}; // namespace Game