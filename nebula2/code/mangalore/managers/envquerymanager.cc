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
    return EntityManager::Instance()->FindEntityById(this->entityUnderMouse);
}

//------------------------------------------------------------------------------
/**
   Get the entities under the mouse drag drop rectangle area
*/
void
EnvQueryManager::GetEntitiesUnderMouseDragDropRect(vector2 dragPosition, vector2 dropPosition, nArray<Ptr<Game::Entity> >& entities)
{
    nArray<Ptr<Graphics::Entity> > graphicsEntities;
    float width, height;
    vector2 center;
    if (dragPosition.x < dropPosition.x)
    {
        width = dropPosition.x - dragPosition.x;
        center.x = dragPosition.x + width*0.5f;
    }
    else
    {
        width = dragPosition.x - dropPosition.x;
        center.x = dropPosition.x + width*0.5f;
    }
    if (dragPosition.y < dropPosition.y)
    {
        height = dropPosition.y - dragPosition.y;
        center.y = dragPosition.y + height*0.5f;
    }
    else
    {
        height = dragPosition.y - dropPosition.y;
        center.y = dropPosition.y + height*0.5f;
    }
    line3 ray = nGfxServer2::Instance()->ComputeWorldMouseRay(center, 5000.0f);
    float angleOfView = nGfxServer2::Instance()->GetCamera().GetAngleOfView();

    Graphics::Server::Instance()->DragDropSelect(ray.end(), angleOfView*width, width/height, graphicsEntities);

    EntityManager* entityMgr = EntityManager::Instance();
    for (int i = 0; i < graphicsEntities.Size(); i++)
    {
        int entityId = graphicsEntities[i]->GetUserData();
        if (entityId)
        {
            entities.PushBack(entityMgr->FindEntityById(entityId));
        }
    }
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
            Physics::Entity* physicsEntity = contact->GetEntity();
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
