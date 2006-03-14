#ifndef PHYSICS_ENTITY_H
#define PHYSICS_ENTITY_H
//------------------------------------------------------------------------------
/**
    @class Physics::Entity

    A physics entity is the frontend to a physics simulation object.
    
    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Level;
class Composite;
class Shape;

class Entity : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Entity);

public:
    /// an entity id
    typedef unsigned int Id;
    /// constructor
    Entity();
    /// destructor
    virtual ~Entity();
    /// render the debug visualization of the entity
    virtual void RenderDebug();
    /// get the entity's unique id
    Id GetUniqueId() const;
    /// set the composite name for this object
    void SetCompositeName(const nString& name);
    /// get the composite name for this object
    const nString& GetCompositeName() const;
    /// set direct composite pointer
    void SetComposite(Composite* c);
    /// get composite pointer of this physics entity
    Composite* GetComposite() const;
    /// called when attached to game entity
    virtual void OnActivate();
    /// called when removed from game entity
    virtual void OnDeactivate();
    /// return true if active 
    bool IsActive() const;
    /// called when attached to physics level
    virtual void OnAttachedToLevel(Level* level);
    /// called when removed from physics level
    virtual void OnRemovedFromLevel();
    /// return true if attached to level
    bool IsAttachedToLevel() const;
    /// invoked before a simulation step is taken
    virtual void OnStepBefore();
    /// invoked after a simulation step is taken
    virtual void OnStepAfter();
    /// invoked before stepping the simulation
    virtual void OnFrameBefore();
    /// invoked after a stepping the simulation
    virtual void OnFrameAfter();
    /// called on collision; decide if it is valid
    virtual bool OnCollide(Shape* collidee);
    /// get level currently attached to
    Level* GetLevel() const;
    /// set the current world space transformation
    virtual void SetTransform(const matrix44& m);
    /// get the current world space transformation
    virtual matrix44 GetTransform() const;
    /// get the current world space velocity
    virtual vector3 GetVelocity() const;
    /// enable/disable the physics entity, re-enables itself on contact
    void SetEnabled(bool b);
    /// get enabled state of the physics entity
    bool IsEnabled() const;
    /// lock this entity in place, will never re-enable itself automatically
    void Lock();
    /// unlock the entity again
    void Unlock();
    /// is this entity locked?
    bool IsLocked() const;
    /// Number of collisions in last simulation step.
    int GetNumCollisions() const;
    /// Is entity horizontal collided?
    bool IsHorizontalCollided() const;
    /// Get all entities collided with this in last simulation step.
    const nArray<Ptr<Shape> >& GetCollidedShapes() const;
    /// set user data, this is a dirty way to get this physics entity's game entity
    void SetUserData(int d);
    /// get user data
    int GetUserData() const;
    /// reset the velocity and force accumulators of the entity
    void Reset();
    /// set temporary stamp value
    void SetStamp(uint s);
    /// get temporary stamp value
    uint GetStamp() const;

    /// enable collision
    void EnableCollision();
    /// disable collision
    void DisableCollision();
    /// return true if collision is currently enabled
    bool IsCollisionEnabled() const;


protected:
    nString compositeName;
    Ptr<Composite> composite;           // the composite of this entity
    bool active;                        // currently between Activate()/Deactivate()
    bool locked;                        // entity currently locked?
    matrix44 lockedTransform;           // backup transform matrix when locked
    Level* level;                       // currently attached to this level
    matrix44 transform;                 // the backup transform matrix
    Id uniqueId;
    static Id uniqueIdCounter;
    int userData;
    uint stamp;
    nArray<Ptr<Shape> > collidedShapes;
    bool collisionEnabled;
};

RegisterFactory(Entity);

//------------------------------------------------------------------------------
/**
*/
inline
bool
Entity::IsActive() const
{
	return this->active;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Entity::IsAttachedToLevel() const
{
    return (0 != this->level);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetStamp(uint s)
{
    this->stamp = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
Entity::GetStamp() const
{
    return this->stamp;
}

//------------------------------------------------------------------------------
/**
    Set the composite name of the entity which will be loaded in
    OnActivate().
*/
inline
void
Entity::SetCompositeName(const nString& name)
{
    this->compositeName = name;
}

//------------------------------------------------------------------------------
/**
    Get the composite name of the entity.
*/
inline
const nString&
Entity::GetCompositeName() const
{
    return this->compositeName;
}

//------------------------------------------------------------------------------
/**
    Get the entity's unique id. Use the physics server's FindEntityByUniqueId()
    method to lookup an entity by its unique id.
*/
inline
Entity::Id
Entity::GetUniqueId() const
{
    return this->uniqueId;
}

//------------------------------------------------------------------------------
/**
    Set the user data of the entity. This is a not-quite-so-nice way to 
    store the game entity's unique id in the physics entity.
*/
inline
void
Entity::SetUserData(int d)
{
    this->userData = d;
}

//------------------------------------------------------------------------------
/**
    Get the user data associated with this physics entity.
*/
inline
int
Entity::GetUserData() const
{
    return this->userData;
}

//------------------------------------------------------------------------------
/**
    Get locked state of entity.
*/
inline
bool
Entity::IsLocked() const
{
    return this->locked;
}

//------------------------------------------------------------------------------
/**
    Return pointer to level object which the physics entity is currently
    attached to.

    @return     pointer to Physics::Level object
*/
inline
Level*
Entity::GetLevel() const
{
    return this->level;
}

//------------------------------------------------------------------------------
/**
    Get all entities collided with this in last simulation step.
*/
inline
const nArray<Ptr<Shape> >&
Entity::GetCollidedShapes() const
{
    return this->collidedShapes;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Entity::IsCollisionEnabled() const
{
    return this->collisionEnabled;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
