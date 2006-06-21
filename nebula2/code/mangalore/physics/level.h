#ifndef PHYSICS_LEVEL_H
#define PHYSICS_LEVEL_H
//------------------------------------------------------------------------------
/**
    @class Physics::Level

    The Physics level contains all the physics entities.

    Has a "point of interest" property which should be set to the point
    where the action happens (for instance where the player controlled
    character is at the moment). This is useful for huge levels where
    physics should only happen in an area around the player.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "util/narray.h"
#include "physics/joint.h"
#include "physics/materialtable.h"
#include "util/nhashmap2.h"
#include "kernel/nprofiler.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Entity;
class Shape;
class Server;
class Ray;
class MouseGripper;

class Level : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Level);

public:
    /// constructor
    Level();
    /// destructor
    virtual ~Level();
    /// called when the level is attached to the physics server
    virtual void OnActivate();
    /// called when the level is detached from the physics server
    virtual void OnDeactivate();
    /// perform one (or more) simulation steps depending on current time
    virtual void Trigger();
    /// access to global embedded mouse gripper
    MouseGripper* GetMouseGripper() const;
    /// attach a physics entity to the level
    void AttachEntity(Entity* entity);
    /// remove a physics entity from the level
    void RemoveEntity(Entity* entity);
    /// get number of entities
    int GetNumEntities() const;
    /// get pointer to entity at index
    Entity* GetEntityAt(int index) const;
    /// directly attach a shape to the level
    void AttachShape(Shape* shape);
    /// remove a shape from the level
    void RemoveShape(Shape* shape);
    /// get number of shapes attached to level
    int GetNumShapes() const;
    /// get shape at index
    Shape* GetShapeAt(int index) const;
    /// set the current simulation time
    void SetTime(nTime t);
    /// get the current simulation time
    nTime GetTime() const;
    /// set step size
    void SetStepSize(nTime t);
    /// get step size
    nTime GetStepSize() const;
    /// set point of interest
    void SetPointOfInterest(const vector3& v);
    /// get current point of interest
    const vector3& GetPointOfInterest() const;
    /// render debug visualization
    void RenderDebug();
    /// get the ODE world id
    dWorldID GetOdeWorldId() const;
    /// get the ODE collision space id
    dSpaceID GetOdeStaticSpaceId() const;
    /// get the ODE dynamic space id
    dSpaceID GetOdeDynamicSpaceId() const;
    /// get the ODE common space id
    dSpaceID GetOdeCommonSpaceId() const;
    /// set gravity vector
    void SetGravity(const vector3& v);
    /// get gravity vector
    const vector3& GetGravity() const;

protected:
    /// ODE collision callback
    static void OdeNearCallback(void* data, dGeomID o1, dGeomID o2);

    nTime time;
    nTime stepSize;
    nArray<Ptr<Entity> > entityArray;
    nArray<Ptr<Shape> > shapeArray;
    Ptr<Shape> collideShape;
    vector3 pointOfInterest;
    vector3 gravity;

    dWorldID odeWorldId;
    dSpaceID odeCommonSpaceId;          // contains both the static and dynamic space
    dSpaceID odeStaticSpaceId;          // collide space for static geoms
    dSpaceID odeDynamicSpaceId;         // collide space for dynamic geoms

    enum
    {
        MaxContacts = 16,
    };
    nTime simTimeStamp;
    dJointGroupID contactJointGroup;
    Ptr<MouseGripper> mouseGripper;

    nHashMap2<nTime> collisionSounds;

    nProfiler profFrameBefore;
    nProfiler profFrameAfter;
    nProfiler profStepBefore;
    nProfiler profStepAfter;
    nProfiler profCollide;
    nProfiler profStep;
    nProfiler profJointGroupEmpty;
    int statsNumSpaceCollideCalled;              // number of times dSpaceCollide has been invoked
    int statsNumNearCallbackCalled;              // number of times the near callback has been invoked
    int statsNumCollideCalled;                   // number of times the collide function has been invoked
    int statsNumCollided;                        // number of times two shapes have collided
    int statsNumSpaces;
    int statsNumShapes;
    int statsNumSteps;
};

RegisterFactory(Level);

//------------------------------------------------------------------------------
/**
    Get the current gravity.

    @return     current gravity
*/
inline
const vector3&
Level::GetGravity() const
{
    return this->gravity;
}

//------------------------------------------------------------------------------
/**
*/
inline
dWorldID
Level::GetOdeWorldId() const
{
    return this->odeWorldId;
}

//------------------------------------------------------------------------------
/**
*/
inline
dSpaceID
Level::GetOdeStaticSpaceId() const
{
    return this->odeStaticSpaceId;
}

//------------------------------------------------------------------------------
/**
*/
inline
dSpaceID
Level::GetOdeDynamicSpaceId() const
{
    return this->odeDynamicSpaceId;
}

//------------------------------------------------------------------------------
/**
*/
inline
dSpaceID
Level::GetOdeCommonSpaceId() const
{
    return this->odeCommonSpaceId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Level::SetPointOfInterest(const vector3& v)
{
    this->pointOfInterest = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
Level::GetPointOfInterest() const
{
    return this->pointOfInterest;
}

//------------------------------------------------------------------------------
/**
    Get number of entities attached to level.
*/
inline
int
Level::GetNumEntities() const
{
    return this->entityArray.Size();
}

//------------------------------------------------------------------------------
/**
    Sets the current simulation time stamp.

    @param  t   the current simulation time stamp
*/
inline
void
Level::SetTime(nTime t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
    Returns current simlulation time stamp.

    @return     the current simulation time stamp
*/
inline
nTime
Level::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
    Set the simulation step size.

    @param  t   the step size in seconds
*/
inline
void
Level::SetStepSize(nTime t)
{
    this->stepSize = t;
}

//------------------------------------------------------------------------------
/**
    Get the simulation step size.

    @return     step size in seconds
*/
inline
nTime
Level::GetStepSize() const
{
    return this->stepSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
MouseGripper*
Level::GetMouseGripper() const
{
    return this->mouseGripper;
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
