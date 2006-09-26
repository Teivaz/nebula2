//------------------------------------------------------------------------------
//  game/property.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "game/property.h"
#include "game/entity.h"

namespace Game
{
ImplementRtti(Game::Property, Message::Port);

//------------------------------------------------------------------------------
/**
*/
Property::Property() : active(false), entity(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Property::~Property()
{
    // n_assert(!this->entity.isvalid());
}

//------------------------------------------------------------------------------
/**
    This method returns an OR'ed mask of entity pools where this
    property is active. By default, the property is active in all
    entity pools (Live and Sleeping), if your subclassed property is
    only active in the Live pool, override this method and only return
    Entity::LivePool.

    During AttachProperty() the entity will ask the property with this
    method whether it is active in the entity's current pool, if not,
    the property will not even be added.
*/
int
Property::GetActiveEntityPools() const
{
    return (Entity::LivePool | Entity::SleepingPool);
}

//------------------------------------------------------------------------------
/**
    Returns whether this property should be attached to entities in the given
    state.

    The deault implementation returns true, i.e. this property is always
    attached to entites, regardless of the given state.

    Properties that do not want this behavior should override this method and
    return the correct flag.
*/
bool
Property::IsActiveInState(const nString& state) const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    If a property adds attributes to an entity, override this method to setup
    their default state. This method is called before the entity is even
    initialized from the database. After this method, entity attributes may
    be overwritten from the database, and after that from a stream.
*/
void
Property::SetupDefaultAttributes()
{
    // empty, override in subclass as necessary
}

//------------------------------------------------------------------------------
/**
    This method is called by Game::Entity::ActivateProperties().
    Use this method for one-time initializations of the property.
*/
void
Property::OnActivate()
{
    n_assert(!this->IsActive());
    this->active = true;
}

//------------------------------------------------------------------------------
/**
    This method is called by Game::Entity::DeactivateProperties(). Use this
    method to cleanup stuff which has been initialized in OnActivate().
*/
void
Property::OnDeactivate()
{
    n_assert(this->IsActive());
    this->active = false;
}

//------------------------------------------------------------------------------
/**
    This method is called from within Game::Entity::Load() after the
    entity attributes have been loaded from the database. You can
    override this method in a subclass if further initialization is needed
    for the property after attributes have been loaded from the database, but
    please be aware that this method may not be called if the entity is created
    directly.
*/
void
Property::OnLoad()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from within Game::Entity::OnStart(). This is the moment
    when the world is complete and the entity can establish connections to other
    entitys.
*/
void
Property::OnStart()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from within Game::Entity::Save() before the
    entity attributes will be saved to the database. You can override
    this method in a subclass if actions are needed before a save happens
    (this is usually the case if entity attributes need to be updated
    by the property before saving).
*/
void
Property::OnSave()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnBeginFrame() on all
    properties attached to an entity in the order of attachment. Override this
    method if your property has to do any work at the beginning of the frame.
*/
void
Property::OnBeginFrame()
{
    // default message handling
    this->HandlePendingMessages();
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnMoveBefore() on all
    properties attached to an entity in the order of attachment. Override this
    method if your property has any work to do before the physics subsystem
    is triggered.
*/
void
Property::OnMoveBefore()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnMoveAfter() on all
    properties attached to an entity in the order of attachment. Override this
    method if your property has any work to do after the physics subsystem
    has been triggered.
*/
void
Property::OnMoveAfter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnRender() on all
    properties attached to an entity in the order of attachment. Override
    this method if your property has any work to do before rendering happens.
*/
void
Property::OnRender()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnRenderDebug() on all
    properties attached to an entity in the order of attachment. It's meant for debug
    issues. It will be called when debug mode is enabled.
*/
void
Property::OnRenderDebug()
{
    // empty
}
//------------------------------------------------------------------------------
/**
    This method is inherited from the Port class. If your property acts as
    a message handler you must implement the Accepts() method to return
    true for each message that is accepted. By default no messages are accepted.
*/
bool
Property::Accepts(Message::Msg* msg)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    This method is inherited from the Port class. If your property acts as
    a message handler you must implement the HandleMessage() method to
    process the incoming messages. Please note that HandleMessage() will
    not be called "automagically" because the Property doesn't know at which
    point in the frame you want to handle pending messages.

    Thus, you must call the HandlePendingMessages() yourself from either OnBeginFrame(),
    OnMoveBefore(), OnMoveAfter() or OnRender().

    The simple rule is: if you override the Accepts() method, you must also call
    HandlePendingMessages() either in OnBeginFrame(), OnMoveBefore(), OnMoveAfter()
    or OnRender()
*/
void
Property::HandleMessage(Message::Msg* msg)
{
    n_assert(Accepts(msg));
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Property::SetEntity(Entity* v)
{
    n_assert(v != 0);
    n_assert(!HasEntity());
    this->entity = v;
}

//------------------------------------------------------------------------------
/**
*/
void
Property::ClearEntity()
{
    this->entity = 0;
}

}; // namespace Game
