//------------------------------------------------------------------------------
//  physics/rigidbody.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/rigidbody.h"
#include "physics/shape.h"
#include "physics/composite.h"
#include "physics/level.h"

namespace Physics
{
ImplementRtti(Physics::RigidBody, Foundation::RefCounted);
ImplementFactory(Physics::RigidBody);

uint RigidBody::uniqueIdCounter = 1;

//------------------------------------------------------------------------------
/**
*/
RigidBody::RigidBody() :
    composite(0),
    curShapeIndex(0),
    entity(0),
    linkNames(NumLinkTypes),
    linkIndex(-1),
    connectedCollision(false),
    angularDamping(0.01f),
    linearDamping(0.005f),
    odeBodyId(0),
    stamp(0)
{
    // set new unique id
    this->uniqueId = uniqueIdCounter++;
}

//------------------------------------------------------------------------------
/**
*/
RigidBody::~RigidBody()
{
    if (this->IsAttached())
    {
        this->Detach();
    }
}

//------------------------------------------------------------------------------
/**
    Begin adding geometry shapes to the rigid body. A rigid body's shape
    can be described by any number of geometry shapes which are attached
    to the rigid body.

    @param  num     number of geometry shapes which will be attached
*/
void
RigidBody::BeginShapes(int num)
{
    n_assert(this->shapeArray.Size() == 0);
    this->shapeArray.SetSize(num);
    this->curShapeIndex = 0;
}

//------------------------------------------------------------------------------
/**
    Add a shape to the rigid body. The refcount of the shape object
    will be incremented. Shape::Open() will be invoked on the shape.

    @param  shape   pointer to a shape object
*/
void
RigidBody::AddShape(Physics::Shape* shape)
{
    n_assert(shape);
    shape->SetRigidBody(this);
    shape->SetEntity(this->GetEntity());
    this->shapeArray[this->curShapeIndex++] = shape;
}

//------------------------------------------------------------------------------
/**
    Finish adding geometry objects to the rigid body.
*/
void
RigidBody::EndShapes()
{
    n_assert(this->shapeArray.Size() == this->curShapeIndex);
}

//------------------------------------------------------------------------------
/**
    Attach the rigid body to the world and initialize its position. 
    This will create an ODE rigid body object and create all associated shapes.
*/
void
RigidBody::Attach(dWorldID worldID, dSpaceID spaceID, const matrix44& m)
{
    n_assert(!this->IsAttached());

    // create ODE rigid body object
    this->odeBodyId = dBodyCreate(worldID);
    dBodySetData(this->odeBodyId, this);

    // attach shapes
    dMassSetZero(&(this->mass));
    int numShapes = this->GetNumShapes();
    int shapeIndex;
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        Shape* curShape = this->GetShapeAt(shapeIndex);
        if (curShape->Attach(spaceID))
        {
            dMassAdd(&(this->mass), &(curShape->GetOdeMass()));
        }
        else
        {
            n_error("RigidBody::Attach(): Failed to open shape!");
        }
    }


    // set resulting mass in the rigid body object
    dBodySetMass(this->odeBodyId, &(this->mass));

    // update body's transformation
    this->SetTransform(m);
}

//------------------------------------------------------------------------------
/**
    Detach the rigid body to the world. This will detach all shapes and
    destroy the ODE rigid body object.
*/
void
RigidBody::Detach()
{
    n_assert(this->IsAttached());

    // detach all shapes
    int numShapes = this->GetNumShapes();
    int shapeIndex;
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        Shape* curShape = this->GetShapeAt(shapeIndex);
        curShape->Detach();
    }

    // destroy the rigid body object
    dBodyDestroy(this->odeBodyId);
    this->odeBodyId = 0;
}

//------------------------------------------------------------------------------
/**
    This method is called before a physics frame consisting of several
    steps takes place.
*/
void
RigidBody::OnFrameBefore()
{
    // reset the number of collision counters and horizontal collided flag
    // in all shapes
    int i;
    int num = this->shapeArray.Size();
    for (i = 0; i < num; i++)
    {
        shapeArray[i]->SetNumCollisions(0);
        shapeArray[i]->SetHorizontalCollided(false);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called after a physics frame consisting of several
    steps takes place.

    28-Nov-04   floh    removed autodisable check, we now use ODE's builtin
                        autodisable feature
*/
void
RigidBody::OnFrameAfter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Apply an impulse vector at a position in the global coordinate frame.
*/
void
RigidBody::ApplyImpulseAtPos(const vector3& impulse, const vector3& pos)
{
    n_assert(this->IsAttached());

    Server* server = Server::Instance();
    dWorldID odeWorldId = server->GetOdeWorldId();
    nTime stepSize = server->GetLevel()->GetStepSize();

    // important: enable body!
    this->SetEnabled(true);

    // convert impulse to force over one timestep
    dVector3 odeForce;
    dWorldImpulseToForce(odeWorldId, dReal(stepSize), impulse.x, impulse.y, impulse.z, odeForce);

    // apply force to body
    dBodyAddForceAtPos(this->odeBodyId, odeForce[0], odeForce[1], odeForce[2], pos.x, pos.y, pos.z);
}

//------------------------------------------------------------------------------
/**
    Apply linear and angular damping.
*/
void
RigidBody::ApplyDamping()
{
    n_assert(this->IsEnabled());

    // apply angular damping
    if (this->angularDamping > 0.0f)
    {
        const dReal* odeVec = dBodyGetAngularVel(this->odeBodyId);
        vector3 v(odeVec[0], odeVec[1], odeVec[2]);
        float damp = n_saturate(1.0f - this->angularDamping);
        v *= damp;

        // clamp at some upper value
        if (v.len() > 30.0f)
        {
            v.norm();
            v *= 30.0f;
        }
        dBodySetAngularVel(this->odeBodyId, v.x, v.y, v.z);
    }
    if (this->linearDamping > 0.0f)
    {
        const dReal* odeVec = dBodyGetLinearVel(this->odeBodyId);
        vector3 v(odeVec[0], odeVec[1], odeVec[2]);
        float damp = n_saturate(1.0f - this->linearDamping);
        v *= damp;

        // clamp at some upper value
        if (v.len() > 50.0f)
        {
            v.norm();
            v *= 50.0f;
        }
        dBodySetLinearVel(this->odeBodyId, v.x, v.y, v.z);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called before a simulation step is taken.
*/
void
RigidBody::OnStepBefore()
{
    Physics::Level* physicsLevel = Physics::Server::Instance()->GetLevel();
    n_assert(physicsLevel);
    dSpaceID dynamicSpace = physicsLevel->GetOdeDynamicSpaceId();
    dSpaceID staticSpace  = physicsLevel->GetOdeStaticSpaceId();
    if (this->IsEnabled())
    {
        // move to dynamic collide space if we have become freshly enabled
        int i;
        int num = this->GetNumShapes();
        for (i = 0; i < num; i++)
        {
            Shape* curShape = this->GetShapeAt(i);
            if (dynamicSpace != curShape->GetSpaceId())
            {
                curShape->RemoveFromSpace();
                curShape->AttachToSpace(dynamicSpace);
            }
        }

        // apply damping
        this->ApplyDamping();
    }
    else
    {
        // move to static collide space if we have become freshly disabled
        int i;
        int num = this->GetNumShapes();
        for (i = 0; i < num; i++)
        {
            Shape* curShape = this->GetShapeAt(i);
            if (staticSpace != curShape->GetSpaceId())
            {
                curShape->RemoveFromSpace();
                curShape->AttachToSpace(staticSpace);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is called after a simulation step is taken.
*/
void
RigidBody::OnStepAfter()
{
    n_assert(this->IsAttached());

    // update own position from rigid body
    const dReal* pos = dBodyGetPosition(this->odeBodyId);
    const dReal* rot = dBodyGetRotation(this->odeBodyId);
    vector3 p;
    Server::OdeToMatrix44(*(dMatrix3*)rot, this->transform);
    Server::OdeToVector3(*(dVector3*)pos, p);
    this->transform.M41 = p.x;
    this->transform.M42 = p.y;
    this->transform.M43 = p.z;
}

//------------------------------------------------------------------------------
/**
*/
int
RigidBody::GetNumCollisions() const
{
    int result = 0;
    for (int i = 0; i < this->shapeArray.Size(); i++)
    {
        result += this->shapeArray[i]->GetNumCollisions();
    }
    n_assert(result >= 0);
    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool
RigidBody::IsHorizontalCollided() const
{
    for (int i = 0; i < this->shapeArray.Size(); i++)
    {
        if (this->shapeArray[i]->IsHorizontalCollided())
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Set the body's current transformation in global space. Only translation
    and rotation is allowed (no scale or shear).

    @param  m   the translate/rotate matrix    
*/
void
RigidBody::SetTransform(const matrix44& m)
{
    this->transform = m;

    if (this->IsAttached())
    {
        dVector3 odePos;
        dMatrix3 odeRot;
        Server::Matrix44ToOde(m, odeRot);
        Server::Vector3ToOde(m.pos_component(), odePos);
        dBodySetPosition(this->odeBodyId, odePos[0], odePos[1], odePos[2]);
        dBodySetRotation(this->odeBodyId, odeRot);
    }
}

//------------------------------------------------------------------------------
/**
    Get the body's current transformation in global space.

    @return the transformation matrix
*/
const matrix44&
RigidBody::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
    Set the body's current linear velocity.

    @param  v   linear velocity
*/
void
RigidBody::SetLinearVelocity(const vector3& v)
{
    dVector3 odeVel;
    Server::Vector3ToOde(v, odeVel);
    dBodySetLinearVel(this->odeBodyId, odeVel[0], odeVel[1], odeVel[2]);
}

//------------------------------------------------------------------------------
/**
    Get the body's current linear velocity.

    @return linear velocity
*/
vector3
RigidBody::GetLinearVelocity() const
{
    vector3 v;
    const dReal* vel = dBodyGetLinearVel(this->odeBodyId);
    Server::OdeToVector3(*(dVector3*)vel, v);
    return v;
}

//------------------------------------------------------------------------------
/**
    Set the body's current angular velocity.

    @param  v   linear velocity
*/
void
RigidBody::SetAngularVelocity(const vector3& v)
{
    dVector3 odeVel;
    Server::Vector3ToOde(v, odeVel);
    dBodySetAngularVel(this->odeBodyId, odeVel[0], odeVel[1], odeVel[2]);
}

//------------------------------------------------------------------------------
/**
    Get the body's current angular velocity.

    @return linear velocity
*/
vector3
RigidBody::GetAngularVelocity() const
{
    vector3 v;
    const dReal* vel = dBodyGetAngularVel(this->odeBodyId);
    Server::OdeToVector3(*(dVector3*)vel, v);
    return v;
}

//------------------------------------------------------------------------------
/**
    Get the current accumulated force vector.

    @param  force   [out] filled with force vector
*/
void
RigidBody::GetLocalForce(vector3& force) const
{
    const dReal* f = dBodyGetForce(this->odeBodyId);
    Server::OdeToVector3(*(dVector3*)f, force);
}

//------------------------------------------------------------------------------
/**
    Get the current accumulated torque vector.

    @param  torque  [out] filled with torque vector
*/
void
RigidBody::GetLocalTorque(vector3& torque) const
{
    const dReal* t = dBodyGetTorque(this->odeBodyId);
    Server::OdeToVector3(*(dVector3*)t, torque);
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of the rigid body.
*/
void
RigidBody::RenderDebug()
{
    if (this->IsAttached())
    {
        matrix44 m = this->GetTransform();
        int i;
        int numShapes = this->shapeArray.Size();
        for (i = 0; i < numShapes; i++)
        {
            this->shapeArray[i]->RenderDebug(m);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
RigidBody::SetEntity(Entity* e)
{
    n_assert(e);
    this->entity = e;

    // distribute to shapes
    int i;
    int num = this->shapeArray.Size();
    for (i = 0; i < num; i++)
    {
        this->shapeArray[i]->SetEntity(e);
    }
}

//------------------------------------------------------------------------------
/**
    Transforms a point in global space into the local coordinate system
    of the body.
*/
vector3
RigidBody::GlobalToLocalPoint(const vector3& p) const
{
    dVector3 result;
    dBodyGetPosRelPoint(this->odeBodyId, p.x, p.y, p.z, result);
    return vector3(result[0], result[1], result[2]);
}

//------------------------------------------------------------------------------
/**
    Transforms a point in body-local space into global space.
*/
vector3
RigidBody::LocalToGlobalPoint(const vector3& p) const
{
    dVector3 result;
    dBodyGetRelPointPos(this->odeBodyId, p.x, p.y, p.z, result);
    return vector3(result[0], result[1], result[2]);
}

} // namespace Physics
