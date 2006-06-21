//------------------------------------------------------------------------------
//  properties/environmentcollideproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/environmentcollideproperty.h"
#include "physics/server.h"
#include "physics/level.h"
#include "game/entity.h"

namespace Properties
{
ImplementRtti(Properties::EnvironmentCollideProperty, Game::Property);
ImplementFactory(Properties::EnvironmentCollideProperty);

//------------------------------------------------------------------------------
/**
*/
EnvironmentCollideProperty::EnvironmentCollideProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EnvironmentCollideProperty::~EnvironmentCollideProperty()
{
    n_assert(this->collideShapes.Size() == 0);
}

//------------------------------------------------------------------------------
/** 
    Makes only sense for live entities.
*/
int
EnvironmentCollideProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentCollideProperty::OnActivate()
{
    Game::Property::OnActivate();

    // attach collide shapes to physics level
    Physics::Level* physicsLevel = Physics::Server::Instance()->GetLevel();
    n_assert(physicsLevel);
    int i;
    int num = this->collideShapes.Size();
    for (i = 0; i < num; i++)
    {
        physicsLevel->AttachShape(this->collideShapes[i]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentCollideProperty::OnDeactivate()
{
    // remove collide shapes from physics level, and clear the collide shapes
    Physics::Level* physicsLevel = Physics::Server::Instance()->GetLevel();
    n_assert(physicsLevel);
    int i;
    int num = this->collideShapes.Size();
    for (i = 0; i < num; i++)
    {
        physicsLevel->RemoveShape(this->collideShapes[i]);
    }
    this->collideShapes.Clear();

    // hand to parent class
    Game::Property::OnDeactivate();
}

} // namespace Properties
