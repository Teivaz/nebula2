#ifndef PHYSICS_PHYSICSUTIL_H
#define PHYSICS_PHYSICSUTIL_H
//------------------------------------------------------------------------------
/**
    @class Physics::PhysicsUtil

    Implements some static physics utility methods.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/vector.h"
#include "physics/filterset.h"

//------------------------------------------------------------------------------
namespace Physics
{
class ContactPoint;

class PhysicsUtil
{
public:
    /// do a normal stabbing test and return closest contact
    static bool RayCheck(const vector3& from, const vector3& to, const FilterSet& excludeSet, ContactPoint& outContact);
    /// do a stabbing test into the world with a ray bundle, return distance to intersection
    static bool RayBundleCheck(const vector3& from, const vector3& to, const vector3& upVec, const vector3& leftVec, float bundleRadius, const FilterSet& excludeSet, float& outContactDist);
};

} // namespace Physics
//------------------------------------------------------------------------------
#endif
