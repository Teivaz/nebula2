//------------------------------------------------------------------------------
//  physics/entity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/composite.h"
#include "physics/entity.h"
#include "physics/level.h"
#include "physics/rigidbody.h"
#include "physics/shape.h"
#include "physics/compositeloader.h"

namespace Physics
{
ImplementRtti(Physics::Entity, Foundation::RefCounted);
ImplementFactory(Physics::Entity);

uint Entity::uniqueIdCounter = 1;

//------------------------------------------------------------------------------
/**
*/
Entity::Entity() :
    active(false),
    level(0),
    composite(0),
    userData(0),
    locked(false),
    stamp(0),
    collisionEnabled(true)
{
    this->uniqueId = uniqueIdCounter++;
    this->collidedShapes.SetFlags(nArray<Ptr<Shape> >::DoubleGrowSize);
    Server::Instance()->RegisterEntity(this);
}

//------------------------------------------------------------------------------
/**
*/
Entity::~Entity()
{
    n_assert(this->level == 0);
    if (this->active)
    {
        this->OnDeactivate();
    }
    Server::Instance()->UnregisterEntity(this);
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity becomes active
    (i.e. when it is attached to a game entity).
*/
void
Entity::OnActivate()
{
    n_assert(!this->active);
    Physics::Server* physicsServer = Physics::Server::Instance();
    dSpaceID dynamicSpaceId = physicsServer->GetOdeDynamicSpaceId();
    dSpaceID staticSpaceId  = physicsServer->GetOdeStaticSpaceId();
    if (this->composite != 0)
    {
        // composite directly set
        this->composite->SetTransform(this->transform);        
        this->composite->Attach(physicsServer->GetOdeWorldId(), dynamicSpaceId, staticSpaceId);
    }
    else if (!this->compositeName.IsEmpty())
    {
        n_assert(this->composite == 0);

        // load and initialize composite object
        CompositeLoader compLoader;
        Ptr<Composite> newComposite = compLoader.Load(this->compositeName);
        newComposite->SetTransform(this->transform);        
        newComposite->Attach(physicsServer->GetOdeWorldId(), dynamicSpaceId, staticSpaceId);
        n_assert2( newComposite != 0, "Composite was created for PhysicsProperty" );
        this->SetComposite(newComposite);
    }
    else
    {
        n_error("Physics::Entity: no valid physics composite name given!");
    }

    this->active = true;
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity becomes inactive,
    (i.e. when it is detached from its game entity).
*/
void
Entity::OnDeactivate()
{
    n_assert(this->active);
    Physics::Server* physicsServer = Physics::Server::Instance();
    if (this->composite != 0)
    {
        if (this->collisionEnabled)
        {
            this->composite->Detach();
        }
        // DO NOT RELEASE THE COMPOSITE, KEEP THE ENTITY INTACT!
        // this->composite = 0;
    }
    this->active = false;
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity is attached to a
    physics level object.

    @param  levl   pointer to a level object
*/
void
Entity::OnAttachedToLevel(Level* levl)
{
    n_assert(levl);
    n_assert(this->level == 0);
    this->level = levl;
    this->SetEnabled(false);
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity is removed from
    its physics level object.
*/
void
Entity::OnRemovedFromLevel()
{
    n_assert(this->level != 0);
    this->level = 0;
}

//------------------------------------------------------------------------------
/**
    Overwrite this in a subclass
*/
bool
Entity::OnCollide(Shape* collidee)
{
    if (!this->collidedShapes.Find(collidee))
    {
        this->collidedShapes.Append(collidee);
    }

    // collision is valid on default
    return true; 
}

//------------------------------------------------------------------------------
/**
    Set the current transformation in world space. This method should
    only be called once at initialization time, since the main job
    of a physics object is to COMPUTE the transformation for a game entity.

    @param  m   a matrix44 defining the world space transformation
*/
void
Entity::SetTransform(const matrix44& m)
{
    if (this->composite != 0)
    {
        this->composite->SetTransform(m);
    }
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
    Get the current transformation in world space. The transformation
    is updated during Physics::Server::Trigger().

    @return     transformation of physics entity in world space
*/
matrix44
Entity::GetTransform() const
{
    if (this->composite != 0)
    {
        return this->composite->GetTransform();
    }
    else
    {
        return this->transform;
    }
}

//------------------------------------------------------------------------------
/**
    Return true if the transformation has changed during the frame.
*/
bool
Entity::HasTransformChanged() const
{
    if (this->composite != 0)
    {
        return this->composite->HasTransformChanged();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Get the current world space velocity.
*/
vector3
Entity::GetVelocity() const
{
    if ((this->composite != 0) && (this->composite->GetMasterBody()))
    {
        return this->composite->GetMasterBody()->GetLinearVelocity();
    }
    else
    {
        return vector3(0.0f, 0.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------
/**
    This method is invoked before a simulation step is taken.
*/
void
Entity::OnStepBefore()
{
    if (this->composite != 0)
    {
        this->composite->OnStepBefore();
    }
}

//------------------------------------------------------------------------------
/**
    This method is invoked after a simulation step is taken.
*/
void
Entity::OnStepAfter()
{
    if (this->composite != 0)
    {
        this->composite->OnStepAfter();
    }

    // if we are locked, and have become enabled automatically,
    // disable again, and restore original position
    if (this->IsLocked())
    {
        if (this->IsEnabled())
        {
            this->SetTransform(this->lockedTransform);
            this->SetEnabled(false);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is invoked before a physics frame starts (consisting of 
    several physics steps).
*/
void
Entity::OnFrameBefore()
{
    this->collidedShapes.Clear();
    if (this->composite != 0)
    {
        this->composite->OnFrameBefore();
    }
}

//------------------------------------------------------------------------------
/**
    This method is invoked after a physics frame is taken (consisting of 
    several physics steps).
*/
void
Entity::OnFrameAfter()
{
    if (this->composite != 0)
    {
        this->composite->OnFrameAfter();
    }
}

//------------------------------------------------------------------------------
/**
    Reset the entity's velocity and force accumulators.
*/
void
Entity::Reset()
{
    if (this->composite != 0)
    {
        this->composite->Reset();
    }
}

//------------------------------------------------------------------------------
/**
*/
int
Entity::GetNumCollisions() const
{
    if (this->composite != 0)
    {
        return composite->GetNumCollisions();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
Entity::IsHorizontalCollided() const
{
    if (!this->composite != 0)
    {
        return composite->IsHorizontalCollided();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Enable/disable the physics entity. A disabled entity will enable itself
    automatically on contact with other enabled entities.
*/
void
Entity::SetEnabled(bool b)
{
    if (this->composite != 0)
    {
        this->composite->SetEnabled(b);
    }
}

//------------------------------------------------------------------------------
/**
    Get the enabled state of the physics entity.
*/
bool
Entity::IsEnabled() const
{
    if (this->composite != 0)
    {
        return this->composite->IsEnabled();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Lock the entity. A locked entity acts like a disabled entity,
    but will never re-enable itself on contact with another entity.
*/
void
Entity::Lock()
{
    n_assert(!this->locked);
    this->locked = true;
    this->lockedTransform = this->GetTransform();
    if (this->GetComposite())
    {
        this->SetEnabled(false);
    }
}

//------------------------------------------------------------------------------
/**
    Unlock the entity. This will reset the entity (set velocity and forces 
    to 0), and place it on the position where it was when the entity was
    locked. Note that the entity will NOT be enabled. This will happen
    automatically when necessary (for instance on contact with another
    active entity).
*/
void
Entity::Unlock()
{
    n_assert(this->locked);
    this->locked = false;
    this->SetTransform(this->lockedTransform);
    this->Reset();
}

//------------------------------------------------------------------------------
/**
    Set direct composite pointer. This is optional to setting a name.
*/
void
Entity::SetComposite(Composite* c)
{
    this->composite = c;
    this->composite->SetEntity(this);
    this->composite->SetTransform(this->transform);
    if (this->locked)
    {
        this->composite->SetEnabled(false);
    }
}

//------------------------------------------------------------------------------
/**
    Get pointer to composite object of this physics entity.
*/
Composite*
Entity::GetComposite() const
{
    return this->composite.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of this entity.
*/
void
Entity::RenderDebug()
{
    if (this->composite != 0)
    {
        this->composite->RenderDebug();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Entity::EnableCollision()
{
    if (this->collisionEnabled || !this->composite.isvalid())
        return;

    Server* server = Server::Instance();
    this->composite->Attach(server->GetOdeWorldId(), server->GetOdeDynamicSpaceId(), server->GetOdeStaticSpaceId());

    this->collisionEnabled = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
Entity::DisableCollision()
{
    if (!this->collisionEnabled || !this->composite.isvalid())
        return;

    this->composite->Detach();

    this->collisionEnabled = false;
}

} // namespace Physics
