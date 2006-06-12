//------------------------------------------------------------------------------
//  physics/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/server.h"
#include "physics/level.h"
#include "physics/composite.h"
#include "physics/ragdoll.h"
#include "physics/rigidbody.h"
#include "physics/boxshape.h"
#include "physics/sphereshape.h"
#include "physics/capsuleshape.h"
#include "physics/meshshape.h"
#include "physics/ray.h"
#include "physics/hingejoint.h"
#include "physics/universaljoint.h"
#include "physics/sliderjoint.h"
#include "physics/balljoint.h"
#include "physics/hinge2joint.h"
#include "physics/amotor.h"
#include "kernel/nfileserver2.h"
#include "util/nstring.h"
#include "gfx2/ngfxserver2.h"
#include "physics/areaimpulse.h"
#include "foundation/factory.h"

namespace Physics
{
ImplementRtti(Physics::Server, Foundation::RefCounted);
ImplementFactory(Physics::Server);

Server* Server::Singleton = 0;
uint Server::UniqueStamp = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    time(0.0),
    contactPoints(256, 256),
    entityRegistry(1024, 1024)
{
    n_assert(0 == Singleton);
    Singleton = this;
    this->contactPoints.SetFlags(nArray<ContactPoint>::DoubleGrowSize);
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);
    n_assert(this->curLevel == 0);
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
dWorldID
Server::GetOdeWorldId() const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetOdeWorldId();
}

//------------------------------------------------------------------------------
/**
*/
dSpaceID
Server::GetOdeStaticSpaceId() const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetOdeStaticSpaceId();
}

//------------------------------------------------------------------------------
/**
*/
dSpaceID
Server::GetOdeDynamicSpaceId() const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetOdeDynamicSpaceId();
}

//------------------------------------------------------------------------------
/**
*/
dSpaceID
Server::GetOdeCommonSpaceId() const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetOdeCommonSpaceId();
}

//------------------------------------------------------------------------------
/**
    Set the current physics level. The refcount of the level will be
    incremented, the refcount of the previous level will be decremented
    (if exists). A 0 pointer is valid and will just release the
    previous level.

    @param  level   pointer to a Physics::Level object
*/
void
Server::SetLevel(Level* level)
{
    n_assert(this->isOpen);
    if (this->curLevel != 0)
    {
        this->curLevel->OnDeactivate();
        this->curLevel = 0;
    }
    if (level)
    {
        this->curLevel = level;
        this->curLevel->OnActivate();
    }
}

//------------------------------------------------------------------------------
/**
    Return pointer to current level.

    @return     pointer to a Physics::Level object
*/
Level*
Server::GetLevel() const
{
    return this->curLevel.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Initialize the physics subsystem.

    - 25-May-05 jo   Don't create default physics level.

    @return     true if physics subsystem initialized successfully
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);

    // setup the material table
    MaterialTable::Setup();

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the physics subsystem.
*/
void
Server::Close()
{
    n_assert(this->isOpen);

    this->SetLevel(0);

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Perform one or more simulation steps. The number of simulation steps
    performed depends on the time of the last call to Trigger().
*/
void
Server::Trigger()
{
    n_assert(this->isOpen);
    if (this->curLevel != 0)
    {
        // trigger the level
        this->curLevel->SetTime(this->time);
        this->curLevel->Trigger();
    }
}

//------------------------------------------------------------------------------
/**
    Set the current point of interest for the physics subsystem. This can
    be for instance the position of the game entity which has the input focus.
    Only the area around this point of interest should be simulated.
*/
void
Server::SetPointOfInterest(const vector3& p)
{
    n_assert(this->curLevel != 0);
    this->curLevel->SetPointOfInterest(p);
}

//------------------------------------------------------------------------------
/**
    Get the current point of interest.
*/
const vector3&
Server::GetPointOfInterest() const
{
    n_assert(this->curLevel != 0);
    return this->curLevel->GetPointOfInterest();
}

//------------------------------------------------------------------------------
/**
    Do a ray check starting from position `pos' along direction `dir'.
    Make resulting intersection points available in `GetIntersectionPoints()'.
*/
bool
Server::RayCheck(const vector3& pos, const vector3& dir, const FilterSet& excludeSet)
{
    const static matrix44 identity;
    this->contactPoints.Clear();
    this->ray.SetOrigin(pos);
    this->ray.SetVector(dir);
    this->ray.SetExcludeFilterSet(excludeSet);
    this->ray.DoRayCheckAllContacts(identity, this->contactPoints);
    return this->contactPoints.Size() > 0;
}

//------------------------------------------------------------------------------
/**
    Create a new Composite object.

    @return     pointer to a new Composite object
*/
Composite*
Server::CreateComposite() const
{
    return Composite::Create();
}

//------------------------------------------------------------------------------
/**
    Create a new Ragdoll object. Ragdolls are a specialization of
    class Composite.

    @return     pointer to a new Composite object
*/
Ragdoll*
Server::CreateRagdoll() const
{
    return Ragdoll::Create();
}

//------------------------------------------------------------------------------
/**
    Create a new RigidBody object. This method is normally
    overwritten by API specific subclasses of Physics::Server.

    @return     pointer to a new RigidBody object
*/
RigidBody*
Server::CreateRigidBody() const
{
    return RigidBody::Create();
}

//------------------------------------------------------------------------------
/**
    Create a HingeJoint object.
*/
HingeJoint*
Server::CreateHingeJoint() const
{
    return HingeJoint::Create();
}

//------------------------------------------------------------------------------
/**
    Create a Hinge2Joint object.
*/
Hinge2Joint*
Server::CreateHinge2Joint() const
{
    return Hinge2Joint::Create();
}

//------------------------------------------------------------------------------
/**
    Create an UniversalJoint object.
*/
UniversalJoint*
Server::CreateUniversalJoint() const
{
    return UniversalJoint::Create();
}

//------------------------------------------------------------------------------
/**
    Create an SliderJoint object.
*/
SliderJoint*
Server::CreateSliderJoint() const
{
    return SliderJoint::Create();
}

//------------------------------------------------------------------------------
/**
    Create an BallJoint object.
*/
BallJoint*
Server::CreateBallJoint() const
{
    return BallJoint::Create();
}

//------------------------------------------------------------------------------
/**
    Create an AMotor object.
*/
AMotor*
Server::CreateAMotor() const
{
    return AMotor::Create();
}

//------------------------------------------------------------------------------
/**
    Create a new fully initialized box shape object.

    @param  m           locale transformation of shape
    @param  matType     MaterialType of the shape
    @param  size        size of the box
    @return             pointer to new box shape object
*/
BoxShape*
Server::CreateBoxShape(const matrix44& m, MaterialType matType, const vector3& size) const
{
    BoxShape* boxShape = BoxShape::Create();
    boxShape->SetTransform(m);
    boxShape->SetMaterialType(matType);
    boxShape->SetSize(size);
    return boxShape;
}

//------------------------------------------------------------------------------
/**
    Create a fully initialized sphere shape object.

    @param  m           locale transformation of shape
    @param  matType     MaterialType of the shape
    @param  radius      radius of sphere
    @return             pointer to new sphere shape object
*/
SphereShape*
Server::CreateSphereShape(const matrix44& m, MaterialType matType, float radius) const
{
    SphereShape* sphereShape = SphereShape::Create();
    sphereShape->SetTransform(m);
    sphereShape->SetMaterialType(matType);
    sphereShape->SetRadius(radius);
    return sphereShape;
}

//------------------------------------------------------------------------------
/**
    Create a fully initialized capsule shape object.

    @param  m           locale transform of shape
    @param  matType     MaterialType of shape
    @param  radius      radius of capsule
    @param  length      lenght of capsule (not counting the caps)
    @return             pointer to a new CapsuleShape object
*/
CapsuleShape*
Server::CreateCapsuleShape(const matrix44& m, MaterialType matType, float radius, float length) const
{
    CapsuleShape* capsuleShape = CapsuleShape::Create();
    capsuleShape->SetTransform(m);
    capsuleShape->SetMaterialType(matType);
    capsuleShape->SetRadius(radius);
    capsuleShape->SetLength(length);
    return capsuleShape;
}

//------------------------------------------------------------------------------
/**
    Create a fully initialized mesh shape object.

    @param  m               locale transformation of shape
    @param  matType         MaterialType of the shape
    @param  meshFilename    filename of mesh resource file
    @return                 pointer to new mesh shape object
*/
MeshShape*
Server::CreateMeshShape(const matrix44& m, MaterialType matType, const nString& meshFilename) const
{
    MeshShape* meshShape = MeshShape::Create();
    meshShape->SetTransform(m);
    meshShape->SetMaterialType(matType);
    meshShape->SetFilename(meshFilename);
    return meshShape;
}

//------------------------------------------------------------------------------
/**
    Create a ray object.

    @param  orig    origin of ray in local space
    @param  vec     direction/length vector in local space
    @return         pointer to new ray object
*/
Physics::Ray*
Server::CreateRay(const vector3& orig, const vector3& vec) const
{
    Ray* ray = Ray::Create();
    ray->SetOrigin(orig);
    ray->SetVector(vec);
    return ray;
}

//------------------------------------------------------------------------------
/**
    Shoots a 3d ray into the world and returns the closest contact.

    @param  pos         starting position of ray
    @param  dir         direction and length of ray
    @param  excludeSet  filter set defining objects to exclude
    @return             pointer to closest ContactPoint, or 0 if no contact detected
*/
const ContactPoint*
Server::GetClosestContactAlongRay(const vector3& pos, const vector3& dir, const FilterSet& excludeSet)
{
    // do the actual ray check (returns all contacts)
    this->RayCheck(pos, dir, excludeSet);

    // find closest contact
    const nArray<ContactPoint>& contacts = this->GetContactPoints();
    int closestContactIndex = -1;
    float closestDistance = dir.len();;
    int i;
    int numContacts = contacts.Size();
    vector3 distVec;
    for (i = 0; i < numContacts; i++)
    {
        const ContactPoint& curContact = contacts[i];
        distVec = curContact.GetPosition() - pos;
        float dist = distVec.len();
        if (dist < closestDistance)
        {
            closestContactIndex = i;
            closestDistance = dist;
        }
    }
    if (closestContactIndex != -1)
    {
        return &contacts[closestContactIndex];
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Shoots a 3d ray through the current mouse position and returns the
    closest contact, or a null pointer if no contact.
    NOTE: This gets the current view matrix from the Nebula gfx server.
    This means the check could be one frame off, if the "previous" view matrix
    is used.

    @param  mousePos        the current 2d mouse position (or any other 2d screen pos)
    @param  length          length of ray through mouse
    @param  excludeSet      filter set defining which objects to exclude
    @return                 pointer to closest ContactPoint or 0 if no contact detected
*/
const ContactPoint*
Server::GetClosestContactUnderMouse(const vector2& mousePos, float length, const FilterSet& excludeSet)
{
    line3 worldMouseRay = nGfxServer2::Instance()->ComputeWorldMouseRay(mousePos, length);
    return this->GetClosestContactAlongRay(worldMouseRay.start(), worldMouseRay.vec(), excludeSet);
}

//------------------------------------------------------------------------------
/**
    Apply an impulse on the first rigid body which lies along the defined ray.
*/
bool
Server::ApplyImpulseAlongRay(const vector3& pos, const vector3& dir, const FilterSet& excludeSet, float impulse)
{
    const ContactPoint* contactPoint = this->GetClosestContactAlongRay(pos, dir, excludeSet);
    if (contactPoint)
    {
        RigidBody* rigidBody = contactPoint->GetRigidBody();
        if (rigidBody)
        {
            vector3 normDir = dir;
            normDir.norm();
            rigidBody->ApplyImpulseAtPos(contactPoint->GetPosition(), normDir * impulse);
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Apply an area impulse to the world.
*/
void
Server::ApplyAreaImpulse(AreaImpulse* impulse)
{
    n_assert(impulse);
    impulse->Apply();
}

//------------------------------------------------------------------------------
/**
    Renders the debug visualization of the level.
*/
void
Server::RenderDebug()
{
    n_assert(this->curLevel);
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->BeginShapes();
    this->curLevel->RenderDebug();
    gfxServer->EndShapes();
}

//------------------------------------------------------------------------------
/**
*/
MouseGripper*
Server::GetMouseGripper() const
{
    return this->curLevel->GetMouseGripper();
}

//------------------------------------------------------------------------------
/**
    This method returns all physics entities touching the given spherical
    area. The method creates a sphere shape and calls its collide
    method, so it's quite fast. Note that entities will be appended to the
    array, so usually you should make sure to pass an empty array. This method
    will also overwrite the internal contactPoints array which can be
    queried after the method has returned, but note that there will only
    be one contact per physics shape.

    @param  pos         center of the sphere
    @param  radius      radius of the sphere
    @param  excludeSet  what contacts should be ignored?
    @param  result      array which will be filled with entity pointers
    @return             number of entities touching the sphere
*/
int
Server::GetEntitiesInSphere(const vector3& pos, float radius, const FilterSet& excludeSet, nArray<Ptr<Entity> >& result)
{
    n_assert(radius >= 0.0f);
    n_assert(this->GetLevel());
    int oldResultSize = result.Size();

    // create a sphere shape and perform collision check
    matrix44 m;
    m.translate(pos);
    Ptr<SphereShape> sphereShape = this->CreateSphereShape(m, MaterialTable::StringToMaterialType("Wood"), radius);
    sphereShape->Attach(this->GetLevel()->GetOdeDynamicSpaceId());
    this->contactPoints.Reset();
    sphereShape->Collide(excludeSet, this->contactPoints);
    sphereShape->Detach();

    // convert contacts to unique entities
    uint stamp = this->GetUniqueStamp();
    int i;
    int numContacts = this->contactPoints.Size();
    for (i = 0; i < numContacts; i++)
    {
        Entity* entity = this->contactPoints[i].GetEntity();
        if (entity && (entity->GetStamp() != stamp))
        {
            entity->SetStamp(stamp);
            result.Append(entity);
        }
    }
    return result.Size() - oldResultSize;
}

//------------------------------------------------------------------------------
/**
    This method returns all physics entities touching the given box
    area. The method creates a box shape and calls its collide
    method, so it's quite fast. Note that entities will be appended to the
    array, so usually you should make sure to pass an empty array. This method
    will also overwrite the internal contactPoints array which can be
    queried after the method has returned, but note that there will only
    be one contact per physics shape.

    @param  pos         center of the box
    @param  size        size of the box
    @param  excludeSet  what contacts should be ignored?
    @param  result      array which will be filled with entity pointers
    @return             number of entities touching the box
*/
int
Server::GetEntitiesInBox(const vector3& pos, const vector3& size, const FilterSet& excludeSet, nArray<Ptr<Entity> >& result)
{
    n_assert(size.x >= 0.0f && size.y >= 0.0f && size.z >= 0.0f);
    n_assert(this->GetLevel());
    int oldResultSize = result.Size();

    // create a sphere shape and perform collision check
    matrix44 m;
    m.translate(pos);
    Ptr<BoxShape> boxShape = this->CreateBoxShape(m, MaterialTable::StringToMaterialType("Wood"), size);
    boxShape->Attach(this->GetLevel()->GetOdeDynamicSpaceId());
    this->contactPoints.Reset();
    boxShape->Collide(excludeSet, this->contactPoints);
    boxShape->Detach();

    // convert contacts to unique entities
    uint stamp = this->GetUniqueStamp();
    int i;
    int numContacts = this->contactPoints.Size();
    for (i = 0; i < numContacts; i++)
    {
        Entity* entity = this->contactPoints[i].GetEntity();
        if (entity && (entity->GetStamp() != stamp))
        {
            entity->SetStamp(stamp);
            result.Append(entity);
        }
    }
    return result.Size() - oldResultSize;
}

} // namespace Physics
