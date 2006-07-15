#ifndef PHYSICS_EXPLOSIONAREAIMPULSE_H
#define PHYSICS_EXPLOSIONAREAIMPULSE_H
//------------------------------------------------------------------------------
/**
    @class Physics::ExplosionAreaImpulse

    Implements an area impulse for a typical explosion. Applies an impulse
    with exponentail falloff to all rigid bodies within the range of the
    explosion witch satisfy a line-of-sight test. After Apply() is called,
    the object can be asked about all physics entities which have been
    affected.
    
    (C) 2005 Radon Labs GmbH
*/
#include "physics/areaimpulse.h"
#include "util/narray.h"
#include "physics/entity.h"

//------------------------------------------------------------------------------
namespace Physics
{
class ContactPoint;
class RigidBody;

class ExplosionAreaImpulse : public AreaImpulse
{
    DeclareRtti;
	DeclareFactory(ExplosionAreaImpulse);

public:
    /// constructor
    ExplosionAreaImpulse();
    /// destructor
    virtual ~ExplosionAreaImpulse();
    /// apply impulse to the world
    void Apply();
    /// set position
    void SetPosition(const vector3& p);
    /// get position
    const vector3& GetPosition() const;
    /// set radius
    void SetRadius(float r);
    /// get radius
    float GetRadius() const;
    /// set max impulse
    void SetImpulse(float i);
    /// get max impulse
    float GetImpulse() const;

private:
    /// apply impulse on single rigid body
    bool HandleRigidBody(RigidBody* rigidBody, const vector3& pos);

    static nArray<ContactPoint> CollideContacts;
    vector3 pos;
    float radius;
    float impulse;
};

RegisterFactory(ExplosionAreaImpulse);

//------------------------------------------------------------------------------
/**
*/
inline
void
ExplosionAreaImpulse::SetPosition(const vector3& p)
{
    this->pos = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
ExplosionAreaImpulse::GetPosition() const
{
    return this->pos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ExplosionAreaImpulse::SetRadius(float r)
{
    this->radius = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ExplosionAreaImpulse::GetRadius() const
{
    return this->radius;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ExplosionAreaImpulse::SetImpulse(float i)
{
    this->impulse = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ExplosionAreaImpulse::GetImpulse() const
{
    return this->impulse;
}

};
//------------------------------------------------------------------------------
#endif
