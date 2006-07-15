//------------------------------------------------------------------------------
//  properties/abstractphysicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/abstractphysicsproperty.h"
#include "game/entity.h"

namespace Properties
{
ImplementRtti(Properties::AbstractPhysicsProperty, TransformableProperty);
ImplementFactory(Properties::AbstractPhysicsProperty);

//------------------------------------------------------------------------------
/**
*/
AbstractPhysicsProperty::AbstractPhysicsProperty() :
    enabled(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AbstractPhysicsProperty::~AbstractPhysicsProperty()
{
    if (this->IsActive())
    {
        this->DisablePhysics();
    }
}

//------------------------------------------------------------------------------
/** 
    Physics is only active for live entities, sleeping entities don't
    have physics.
*/
int
AbstractPhysicsProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/** 
    Called when property is attached to a game entity. This will create and setup
    the required physics entities.
*/
void
AbstractPhysicsProperty::OnActivate()
{
    TransformableProperty::OnActivate();
    
    // activate physics by default
    this->EnablePhysics();
}

//------------------------------------------------------------------------------
/** 
    Called when property is going to be removed from its game entity.
    This will release the physics entity owned by the game entity.
*/
void
AbstractPhysicsProperty::OnDeactivate()
{
    if (this->IsEnabled())
    {
        this->DisablePhysics();
    }
    
    TransformableProperty::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
AbstractPhysicsProperty::SetEnabled(bool setEnabled)
{
    if (this->enabled != setEnabled)
    {
        if (setEnabled)
        {
            this->EnablePhysics();
        }
        else
        {
            this->DisablePhysics();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AbstractPhysicsProperty::EnablePhysics()
{
    n_assert(!this->IsEnabled());
    this->enabled = true;
}

//------------------------------------------------------------------------------
/**
*/
void
AbstractPhysicsProperty::DisablePhysics()
{
    n_assert(this->IsEnabled());
    this->enabled = false;
}

//------------------------------------------------------------------------------
/**
*/
Physics::Entity*
AbstractPhysicsProperty::GetPhysicsEntity() const
{
    return 0;
}

} // namespace Properties
