//------------------------------------------------------------------------------
//  physics/charentity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/charentity.h"
#include "physics/server.h"
#include "physics/rigidbody.h"
#include "physics/shape.h"
#include "physics/boxshape.h"
#include "physics/composite.h"
#include "physics/level.h"
#include "physics/physicsutil.h"
#include "physics/amotor.h"
#include "physics/ragdoll.h"
#include "physics/compositeloader.h"

using namespace Foundation;

namespace Physics
{
ImplementRtti(Physics::CharEntity, Physics::Entity);
ImplementFactory(Physics::CharEntity);

//------------------------------------------------------------------------------
/**
*/
CharEntity::CharEntity() :
    angularVelocityDirty(true),
    radius(0.3f),
    height(1.75f),
    hover(0.2f),
    nebCharacter(0),
    ragdollActive(false),
    groundMaterial(InvalidMaterial)
{
}

//------------------------------------------------------------------------------
/**
*/
CharEntity::~CharEntity()
{
    n_assert(this->baseBody == 0);
}

//------------------------------------------------------------------------------
/**
    Create the abstract composite for the character entity when the
    entity is "alive" (not in its ragdoll state).
*/
void
CharEntity::CreateDefaultComposite()
{
    Server* physicsServer = Physics::Server::Instance();

    // create a composite with a spherical rigid body and an angular motor
    this->defaultComposite = physicsServer->CreateComposite();

    // create a base rigid body object
    this->baseBody = physicsServer->CreateRigidBody();
    this->baseBody->SetName("CharEntityBody");

    float capsuleLength = this->height - 2.0f * this->radius - this->hover;
    matrix44 upRight;
    upRight.rotate_x(n_deg2rad(90.0f));
    upRight.translate(vector3(0.0f, capsuleLength * 0.5f, 0.0f));
    Ptr<Shape> shape = (Physics::Shape*) physicsServer->CreateCapsuleShape(upRight, MaterialTable::StringToMaterialType("Character"), this->radius, capsuleLength);
    this->baseBody->BeginShapes(1);
    this->baseBody->AddShape(shape);
    this->baseBody->EndShapes();
    this->defaultComposite->BeginBodies(1);
    this->defaultComposite->AddBody(this->baseBody);
    this->defaultComposite->EndBodies();

    // create an amotor connected to the base shape
    this->aMotor = physicsServer->CreateAMotor();
    this->aMotor->SetBodies(this->baseBody, 0);
    this->aMotor->SetNumAxes(2);
    this->aMotor->AxisParams(0).SetAxis(vector3(1.0f, 0.0f, 0.0f));
    this->aMotor->AxisParams(1).SetAxis(vector3(0.0f, 0.0f, 1.0f));
    this->aMotor->AxisParams(0).SetFMax(100000.0f);
    this->aMotor->AxisParams(1).SetFMax(100000.0f);
    this->defaultComposite->BeginJoints(1);
    this->defaultComposite->AddJoint(this->aMotor);
    this->defaultComposite->EndJoints();
}

//------------------------------------------------------------------------------
/**
    Create the optional ragdoll composite for the character entity.
*/
void
CharEntity::CreateRagdollComposite()
{
    // load and setup ragdoll
    if (this->compositeName.IsValid())
    {
        n_assert(this->compositeName.IsValid());
        CompositeLoader compLoader;
        this->ragdollComposite = (Physics::Ragdoll*) compLoader.Load(this->compositeName);
        n_assert(this->ragdollComposite->IsA(Ragdoll::RTTI));
        this->ragdollComposite->SetTransform(this->transform);
        this->ragdollComposite->SetCharacter(this->GetCharacter());
        this->ragdollComposite->Bind();
    }
}

//------------------------------------------------------------------------------
/**
    Called when the entity is attached to the game entity. This will
    initialize the embedded composite object.
*/
void
CharEntity::OnActivate()
{
    n_assert(this->baseBody == 0);
    Server* physicsServer = Physics::Server::Instance();
    this->active = true;

    // create the default composite object (when the character is alive)
    this->CreateDefaultComposite();

    // create optional ragdoll composite object
    this->CreateRagdollComposite();

    this->SetComposite(this->defaultComposite);
    this->defaultComposite->Attach(physicsServer->GetOdeWorldId(), physicsServer->GetOdeDynamicSpaceId(), physicsServer->GetOdeStaticSpaceId());
    this->active = true;

    // NOTE: do NOT call parent class
}

//------------------------------------------------------------------------------
/**
    Called when the Physics entity is removed from the Game entity.
*/
void
CharEntity::OnDeactivate()
{
    n_assert(this->baseBody != 0);
    Level* level = Physics::Server::Instance()->GetLevel();
    n_assert(level);

    Entity::OnDeactivate();

    this->baseBody = 0;
    this->aMotor = 0;
    this->defaultComposite = 0;
    this->ragdollComposite = 0;
    this->ragdollImpulse = 0;
    this->SetCharacter(0);
}

//------------------------------------------------------------------------------
/**
    Set world space transform. The actual physics transform will be
    offset by the ball radius in the Y direction.

    @param  m   a 4x4 transformation matrix in world space
*/
void
CharEntity::SetTransform(const matrix44& m)
{
    matrix44 offsetMatrix(m);
    offsetMatrix.M42 += this->radius + this->hover;
    Entity::SetTransform(offsetMatrix);
    this->lookatDirection = -m.z_component();
}

//------------------------------------------------------------------------------
/**
    Get world space transform. The returned matrix will always be
    in the upright position (local y axis aligned with global y axis), 
    and the -z axis will look into the velocity direction.

    @return     a 4x4 transformation matrix in world space
*/
matrix44
CharEntity::GetTransform() const
{
    if (this->IsRagdollActive())
    {
        return Entity::GetTransform();
    }
    else
    {
        matrix44 tmp = Entity::GetTransform();
        static vector3 upVector(0.0f, 1.0f, 0.0f);
        matrix44 fixedTransform;
        fixedTransform.lookatRh(this->lookatDirection, upVector);
        fixedTransform.translate(tmp.pos_component());
        fixedTransform.M42 -= this->radius + this->hover;
        return fixedTransform;
    }
}

//------------------------------------------------------------------------------
/**
    Always return the desired velocity instead of the real velocity.
*/
vector3
CharEntity::GetVelocity() const
{
    return this->desiredVelocity;
}

//------------------------------------------------------------------------------
/**
    This function checks if the character is currently touching the ground
    by doing a vertical ray check.
*/
bool
CharEntity::CheckGround(float& dist)
{
    Physics::Server* physicsServer = Physics::Server::Instance();

    vector3 pos = this->GetTransform().pos_component();
    vector3 from(pos.x, pos.y + this->radius * 2.0f, pos.z);
    vector3 dir(0.0f, -this->radius * 4.0f, 0.0f);

    FilterSet excludeSet = this->groundExcludeSet;
    excludeSet.AddEntityId(this->GetUniqueId());
    const ContactPoint* contact = Physics::Server::Instance()->GetClosestContactAlongRay(from, dir, excludeSet);    
    if (contact)
    {
        dist = vector3(contact->GetPosition() - from).len() - this->radius * 2.0f;
        this->groundMaterial = contact->GetMaterial();
        return true;
    }
    else
    {
        dist = 2.0f * this->radius;
        this->groundMaterial = InvalidMaterial;
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Called before a simulation step is taken. This will convert the
    linear desired velocity into an angular velocity.
*/
void
CharEntity::OnStepBefore()
{
    if (this->IsCollisionEnabled() && !this->IsRagdollActive())
    {
        Physics::Server* physicsServer = Physics::Server::Instance();
        RigidBody* body = this->baseBody;
        dBodyID odeBodyId = body->GetOdeBodyId();

        // get "touching ground status" and determine ground material
        float distToGround;
        bool nearGround = this->CheckGround(distToGround);
 
        // enable/disable the body based on desired velocity
        vector3 desVel(this->desiredVelocity.x, -distToGround * 50.0f, this->desiredVelocity.z);
        this->SetEnabled(true);

        // get current velocity and mass of body
        float m = body->GetMass();
        vector3 curVel = body->GetLinearVelocity();

        // compute resulting impulse
        vector3 p = -(curVel - desVel) * m;

        // convert impulse to force
        dVector3 odeForce;
        dWorldImpulseToForce(physicsServer->GetOdeWorldId(), dReal(this->level->GetStepSize()), p.x, p.y, p.z, odeForce);

        // set new force
        dBodyAddForce(odeBodyId, odeForce[0], odeForce[1], odeForce[2]);
    }
    
    // call parent class
    Entity::OnStepBefore();
}

//------------------------------------------------------------------------------
/**
    Activate the optional ragdoll.
*/
void
CharEntity::ActivateRagdoll()
{
    n_assert(this->nebCharacter);
    n_assert(!this->IsRagdollActive());

    this->ragdollActive = true;

    if (this->ragdollComposite.isvalid())
    {
        // get transform before detaching current composite
        matrix44 curTransform = Entity::GetTransform();

        // switch current composite, this will reset the
        // composite's transform to the entity's initial transform
        this->DisableCollision();
        this->SetComposite(this->ragdollComposite);
        this->EnableCollision();

        // fix transform (all the 180 voodoo is necessary because Nebula2's
        // characters are rotated by 180 degree around the Y axis
        matrix44 rot180;
        rot180.rotate_y(n_deg2rad(180.0f));
        static vector3 upVector(0.0f, 1.0f, 0.0f);
        matrix44 m;
        m.lookatRh(this->lookatDirection, upVector);
        m.translate(curTransform.pos_component());
        m.M42 -= this->radius + this->hover;
        rot180.mult_simple(m);
        this->ragdollComposite->SetTransform(rot180);

        // sync physics joint angles with bind pose model
        this->ragdollComposite->ReadJoints();

        // apply a the stored impulse to the ragdoll
        if (this->ragdollImpulse.isvalid())
        {
            Server::Instance()->ApplyAreaImpulse(this->ragdollImpulse);
        }    
        this->ragdollImpulse = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Deactivate the optional ragdoll.
*/
void
CharEntity::DeactivateRagdoll()
{
    n_assert(this->IsRagdollActive());

    this->ragdollActive = false;

    if (this->ragdollComposite.isvalid())
    {
        this->DisableCollision();
        this->SetComposite(this->defaultComposite);
        this->EnableCollision();
    }
}

//------------------------------------------------------------------------------
/**
    Overwrite this in a subclass
*/
bool
CharEntity::OnCollide(Shape* collidee)
{
    bool validCollision = Entity::OnCollide(collidee);
    validCollision &= !this->groundExcludeSet.CheckShape(collidee);
    return validCollision;
}

} // namespace Physics