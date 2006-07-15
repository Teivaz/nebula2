//------------------------------------------------------------------------------
//  physics/contactpoint.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "physics/contactpoint.h"
#include "physics/entity.h"
#include "physics/composite.h"

namespace Physics
{

//------------------------------------------------------------------------------
/**
    Returns pointer to entity of the contact point. This resolves an
    entity id into a pointer, so it may be slow.
*/
Entity*
ContactPoint::GetEntity() const
{
    return Server::Instance()->FindEntityByUniqueId(this->entityId);
}

//------------------------------------------------------------------------------
/**
    Returns pointer to rigid body of contact point. This resolves id's into
    pointer, so it may be slow.
*/
RigidBody*
ContactPoint::GetRigidBody() const
{
    // find entity
    Entity* entity = this->GetEntity();
    if (entity)
    {
        return entity->GetComposite()->FindBodyByUniqueId(this->rigidBodyId);
    }
    else
    {
        return 0;
    }
}

}
