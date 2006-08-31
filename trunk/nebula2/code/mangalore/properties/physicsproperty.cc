//------------------------------------------------------------------------------
//  properties/physicsproperty.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "properties/physicsproperty.h"
#include "game/entity.h"
#include "physics/server.h"
#include "physics/level.h"
#include "attr/attributes.h"
#include "foundation/factory.h"
#include "msg/settransform.h"

namespace Properties
{
ImplementRtti(Properties::PhysicsProperty, AbstractPhysicsProperty);
ImplementFactory(Properties::PhysicsProperty);

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
PhysicsProperty::PhysicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PhysicsProperty::~PhysicsProperty()
{
    n_assert(!this->physicsEntity.isvalid());
}

//------------------------------------------------------------------------------
/**
    Get pointer to physics entity. Note that this method may return 0!
*/
Physics::Entity*
PhysicsProperty::GetPhysicsEntity() const
{
    return this->physicsEntity.get_unsafe();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::SetupDefaultAttributes()
{
    AbstractPhysicsProperty::SetupDefaultAttributes();
    GetEntity()->SetVector3(Attr::VelocityVector, vector3(0.0f, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------
/**
    Called after the physics subsystem has been triggered. This will transfer
    the physics entity's new transform back into the game entity.
*/
void
PhysicsProperty::OnMoveAfter()
{
    if (this->IsEnabled() && this->physicsEntity->HasTransformChanged())
    {
        Ptr<Message::UpdateTransform> msg = Message::UpdateTransform::Create();
        msg->SetMatrix(this->GetPhysicsEntity()->GetTransform());
        this->GetEntity()->SendSync(msg);
        GetEntity()->SetVector3(Attr::VelocityVector, this->physicsEntity->GetVelocity());
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
PhysicsProperty::Accepts(Message::Msg* msg)
{
    n_assert(msg != 0);
    if (msg->CheckId(Message::SetTransform::Id)) return true;
    return AbstractPhysicsProperty::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::HandleMessage(Message::Msg* msg)
{
    n_assert(msg != 0);

    if (msg->CheckId(Message::SetTransform::Id) && this->IsEnabled())
    {
        // set transform of physics entity
        Message::SetTransform* transformMsg = (Message::SetTransform*) msg;
        this->GetPhysicsEntity()->SetTransform(transformMsg->GetMatrix());
    }

	// forward settransform msg upwards to set Attr::Transform directly in Transformable::Property
	AbstractPhysicsProperty::HandleMessage(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::EnablePhysics()
{
    n_assert(!this->IsEnabled());

    if (GetEntity()->HasAttr(Attr::Physics))
    {
        if (this->physicsEntity == 0)
        {
            // create and setup physics entity
            this->physicsEntity = CreatePhysicsEntity();
            n_assert(this->physicsEntity != 0);
            this->physicsEntity->SetCompositeName(GetEntity()->GetString(Attr::Physics));
            this->physicsEntity->SetUserData(GetEntity()->GetUniqueId());
        }

        // attach physics entity to physics level
        this->physicsEntity->SetTransform(GetEntity()->GetMatrix44(Attr::Transform));
        Physics::Level* physicsLevel = Physics::Server::Instance()->GetLevel();
        n_assert(physicsLevel);
        physicsLevel->AttachEntity(this->physicsEntity);

        // call parent to do the real enable
        AbstractPhysicsProperty::EnablePhysics();
    }
}

//------------------------------------------------------------------------------
/**
*/
Physics::Entity*
PhysicsProperty::CreatePhysicsEntity()
{
    return Physics::Entity::Create();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::DisablePhysics()
{
    n_assert(this->IsEnabled());

    // release the physics entity
    Physics::Level* physicsLevel = Physics::Server::Instance()->GetLevel();
    n_assert(physicsLevel);
    physicsLevel->RemoveEntity(this->GetPhysicsEntity());

    // call parent
    AbstractPhysicsProperty::DisablePhysics();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::OnDeactivate()
{
    // call parent to cleanup
    AbstractPhysicsProperty::OnDeactivate();

    // release physics entity
    if (this->physicsEntity != 0)
    {
        this->physicsEntity = 0;
    }
}

} // namespace Properties
