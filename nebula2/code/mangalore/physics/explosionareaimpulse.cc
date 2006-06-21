//------------------------------------------------------------------------------
//  physics/explosionareaimpulse.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "physics/explosionareaimpulse.h"
#include "physics/server.h"
#include "physics/sphereshape.h"
#include "physics/materialtable.h"
#include "physics/rigidbody.h"

namespace Physics
{
ImplementRtti(Physics::ExplosionAreaImpulse, Physics::AreaImpulse);
ImplementFactory(Physics::ExplosionAreaImpulse);

nArray<ContactPoint> ExplosionAreaImpulse::CollideContacts(256, 512);

//------------------------------------------------------------------------------
/**
*/
ExplosionAreaImpulse::ExplosionAreaImpulse() :
    radius(1.0f),
    impulse(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ExplosionAreaImpulse::~ExplosionAreaImpulse()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ExplosionAreaImpulse::Apply()
{
    Server* physicsServer = Server::Instance();

    // create a sphere shape and collide it against the world
    matrix44 m;
    m.translate(this->pos);
    Ptr<SphereShape> sphereShape = physicsServer->CreateSphereShape(m, MaterialTable::StringToMaterialType("Wood"), this->radius);
    sphereShape->Attach(physicsServer->GetOdeDynamicSpaceId());
    FilterSet excludeSet;
    sphereShape->Collide(excludeSet, CollideContacts);
    sphereShape->Detach();

    // apply impulse to rigid bodies
    uint stamp = Server::GetUniqueStamp();
    int i;
    int numContacts = CollideContacts.Size();
    for (i = 0; i < numContacts; i++)
    {
        RigidBody* rigidBody = CollideContacts[i].GetRigidBody();
        if (rigidBody && (rigidBody->GetStamp() != stamp))
        {
            rigidBody->SetStamp(stamp);
            this->HandleRigidBody(rigidBody, CollideContacts[i].GetPosition());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Applies impulse on single rigid body. Does line of sight test on
    the center of the rigid body (FIXME: check all corners of the 
    bounding box??).
*/
bool
ExplosionAreaImpulse::HandleRigidBody(RigidBody* rigidBody, const vector3& rigidBodyPos)
{
    Server* physicsServer = Server::Instance();

    // do line of sight check to position of body
//    FilterSet excludeSet;
//    excludeSet.AddRigidBodyId(rigidBody->GetUniqueId());
    vector3 dirVec = rigidBodyPos - this->pos;
//    if (0 == physicsServer->GetClosestContactAlongRay(this->pos, dirVec, excludeSet))
//    {
        // free line of sight, apply impulse
        float dist = dirVec.len();
        dirVec.norm();

        // scale impulse by distance
        float attenuate = 1.0f - n_saturate(dist / this->radius);
        vector3 impulse = dirVec * this->impulse * attenuate;

        rigidBody->ApplyImpulseAtPos(impulse, rigidBodyPos);
        return true;
//    }
//    else
//    {
//        // no free line of sight
//        return false;
//    }
}

} // namespace Physics
