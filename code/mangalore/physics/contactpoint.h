#ifndef PHYSICS_CONTACTPOINT_H
#define PHYSICS_CONTACTPOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::ContactPoint

    Contact points with position and Face-Up vector.

    (C) 2003 RadonLabs GmbH
*/
#include "mathlib/vector.h"
#include "physics/materialtable.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Entity;
class RigidBody;
class Composite;

class ContactPoint
{
public:
    /// Construct.
    ContactPoint();

    /// clear content
    void Clear();
    /// set position to `v'
    void SetPosition(const vector3& v);
    /// get position
    const vector3& GetPosition() const;
    /// set up vector to `v'
    void SetUpVector(const vector3& v);
    /// get up vector
    const vector3& GetUpVector() const;
    /// get penetration depth
    float GetDepth() const;
    /// set penetration depth
    void SetDepth(float d);
    /// set optional entity id
    void SetEntityId(uint id);
    /// get entity id
    uint GetEntityId() const;
    /// set optional rigid body index in composite of entity
    void SetRigidBodyId(uint i);
    /// get rigid body index
    uint GetRigidBodyId() const;
    /// Set material of contact
    void SetMaterial(MaterialType material);
    /// get material of contact
    MaterialType GetMaterial() const;
    /// find physics entity of contact, slow, may return 0
    Entity* GetEntity() const;
    /// find rigid body of contact, slow, may return 0
    RigidBody* GetRigidBody() const;

private:
    vector3 position;
    vector3 upVector;
    float depth;
    uint entityId;
    uint rigidBodyId;
    uint gameEntityId;
    MaterialType material;
};

//------------------------------------------------------------------------------
/**
*/
inline
ContactPoint::ContactPoint() :
    upVector(0.0f, 1.0f, 0.0f),
    depth(0.f),
    entityId(0),
    rigidBodyId(0),
    material(InvalidMaterial)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::Clear()
{
    this->position.set(0.0f, 0.0f, 0.0f);
    this->upVector.set(0.0f, 1.0f, 0.0f);
    this->entityId = 0;
    this->rigidBodyId = 0;
    this->material = InvalidMaterial;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
ContactPoint::SetPosition(const vector3& v)
{
    position = v;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const vector3& 
ContactPoint::GetPosition() const
{
    return position;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
ContactPoint::SetUpVector(const vector3& v)
{
    upVector = v;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const vector3& 
ContactPoint::GetUpVector() const
{
    return upVector;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ContactPoint::GetDepth() const
{
    return this->depth;    
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetDepth(float d)
{
    this->depth = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetEntityId(uint id)
{
    this->entityId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
ContactPoint::GetEntityId() const
{
    return this->entityId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetRigidBodyId(uint id)
{
    this->rigidBodyId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
ContactPoint::GetRigidBodyId() const
{
    return this->rigidBodyId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetMaterial(MaterialType mat)
{
    this->material = mat;
}

//------------------------------------------------------------------------------
/**
*/
inline
MaterialType
ContactPoint::GetMaterial() const
{
    return this->material;
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
