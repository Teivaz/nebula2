//------------------------------------------------------------------------------
//  physics/sphereshape.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/sphereshape.h"
#include "physics/materialtable.h"
#include "gfx2/ngfxserver2.h"

namespace Physics
{
ImplementRtti(Physics::SphereShape, Physics::Shape);
ImplementFactory(Physics::SphereShape);

//------------------------------------------------------------------------------
/**
*/
SphereShape::SphereShape() :
    Shape(Sphere),
    radius(1.0f)
{
}

//------------------------------------------------------------------------------
/**
*/
SphereShape::~SphereShape()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create a sphere object, add it to ODE's collision space, and initialize
    the mass member.
*/
bool
SphereShape::Attach(dSpaceID spaceId)
{
    if (Shape::Attach(spaceId))
    {
        dGeomID sphere = dCreateSphere(0, radius);
        this->AttachGeom(sphere, spaceId);
        dMassSetSphere(&(this->odeMass), Physics::MaterialTable::GetDensity(this->materialType), this->radius);
        this->TransformMass();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the sphere shape.

    @param  t   transform matrix of my parent rigid body
*/
void
SphereShape::RenderDebug(const matrix44& parentTransform)
{
    if (this->IsAttached())
    {
        // compute resulting model matrix
        matrix44 m;
        m.scale(vector3(this->radius, this->radius, this->radius));
        m *= this->GetTransform();
        m *= parentTransform;
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Sphere, m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics