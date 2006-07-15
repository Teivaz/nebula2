//------------------------------------------------------------------------------
//  properties/environmentgraphicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/environmentgraphicsproperty.h"
#include "graphics/server.h"
#include "game/entity.h"

namespace Properties
{
ImplementRtti(Properties::EnvironmentGraphicsProperty, Game::Property);
ImplementFactory(Properties::EnvironmentGraphicsProperty);

//------------------------------------------------------------------------------
/**
*/
EnvironmentGraphicsProperty::EnvironmentGraphicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EnvironmentGraphicsProperty::~EnvironmentGraphicsProperty()
{
    n_assert(this->graphicsEntities.Size() == 0);
}

//------------------------------------------------------------------------------
/** 
    Makes only sense for live entities.
*/
int
EnvironmentGraphicsProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentGraphicsProperty::OnActivate()
{
    Game::Property::OnActivate();

    // attach graphics properties to graphics level
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    int i;
    int num = this->graphicsEntities.Size();
    for (i = 0; i < num; i++)
    {
        graphicsLevel->AttachEntity(this->graphicsEntities[i]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentGraphicsProperty::OnDeactivate()
{
    // remove graphics properties from level, and release the graphics entities
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    int i;
    int num = this->graphicsEntities.Size();
    for (i = 0; i < num; i++)
    {
        graphicsLevel->RemoveEntity(this->graphicsEntities[i]);
    }
    this->graphicsEntities.Clear();

    Game::Property::OnDeactivate();
}

} // namespace Properties
