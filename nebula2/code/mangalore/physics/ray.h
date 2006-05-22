#ifndef PHYSICS_RAY_H
#define PHYSICS_RAY_H
//------------------------------------------------------------------------------
/**
    @class Physics::Ray

    Ray objects are used to perform ray checks on the physics world
    representation.
    
    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "physics/contactpoint.h"
#include "physics/filterset.h"
#include "ode/ode.h"
#define BAN_OPCODE_AUTOLINK
#include "opcode/Opcode.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Ray : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Ray);

public:
    /// constructor
    Ray();
    /// destructor
    virtual ~Ray();
    /// set ray origin
    void SetOrigin(const vector3& v);
    /// get ray origin
    const vector3& GetOrigin() const;
    /// set direction and length vector
    void SetVector(const vector3& v);
    /// get direction and length vector
    const vector3& GetVector() const;
    /// get normalized vector
    const vector3& GetNormVector() const;
    /// set exclusion filter set
    void SetExcludeFilterSet(const FilterSet& f);
    /// get exclusion filter set
    const FilterSet& GetExcludeFilterSet() const;
    /// do a complete ray check, return all contact points unsorted in user provided contacts array
    virtual int DoRayCheckAllContacts(const matrix44& transform, nArray<ContactPoint>& intersectionArray);
    /// get ode geom id
    dGeomID GetGeomId() const;

private:
    /// ODE collision callback function
    static void OdeRayCallback(void* data, dGeomID o1, dGeomID o2);
    
    enum
    {
        MaxContacts = 16,
    };
    vector3 orig;
    vector3 vec;
    vector3 normVec;
    FilterSet excludeFilterSet;
    dGeomID odeRayId;
    static nArray<ContactPoint>* contactsArray;             // not owned!
};

RegisterFactory(Ray);

//------------------------------------------------------------------------------
/**
*/
inline
dGeomID
Ray::GetGeomId() const
{
    return this->odeRayId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Ray::SetOrigin(const vector3& v)
{
    this->orig = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
Ray::GetOrigin() const
{
    return this->orig;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Ray::SetVector(const vector3& v)
{
    this->vec = v;
    this->normVec = v;
    this->normVec.norm();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
Ray::GetVector() const
{
    return this->vec;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
Ray::GetNormVector() const
{
    return this->normVec;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Ray::SetExcludeFilterSet(const FilterSet& f)
{
    this->excludeFilterSet = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
const FilterSet&
Ray::GetExcludeFilterSet() const
{
    return this->excludeFilterSet;
}

}; // namespace Physics

//------------------------------------------------------------------------------
#endif

