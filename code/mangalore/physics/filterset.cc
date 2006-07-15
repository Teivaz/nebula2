//------------------------------------------------------------------------------
//  physics/filterset.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "physics/filterset.h"
#include "physics/shape.h"
#include "physics/entity.h"
#include "physics/rigidbody.h"

namespace Physics
{
//------------------------------------------------------------------------------
/**
*/
bool
FilterSet::CheckShape(Shape* shape) const
{
    n_assert(shape);
    if (this->CheckMaterialType(shape->GetMaterialType()))
    {
        return true;
    }
    Entity* entity = shape->GetEntity();
    if (entity && this->CheckEntityId(entity->GetUniqueId()))
    {
        return true;
    }
    RigidBody* rigidBody = shape->GetRigidBody();
    if (rigidBody && this->CheckRigidBodyId(rigidBody->GetUniqueId()))
    {
        return true;
    }
    return false;
}

};
