//------------------------------------------------------------------------------
//  physics/boxshape.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/boxshape.h"
#include "gfx2/ngfxserver2.h"
#include "physics/rigidbody.h"

namespace Physics
{
ImplementRtti(Physics::BoxShape, Physics::Shape);
ImplementFactory(Physics::BoxShape);

//------------------------------------------------------------------------------
/**
*/
BoxShape::BoxShape() :
    Shape(Box),
    size(1.0f, 1.0f, 1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
BoxShape::~BoxShape()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create a box object, add it to ODE's collision space, and initialize
    the mass member.
*/
bool
BoxShape::Attach(dSpaceID spaceId)
{
    if (Shape::Attach(spaceId))
    {
        dGeomID box = dCreateBox(0, this->size.x, this->size.y, this->size.z);
        this->AttachGeom(box, spaceId);
        dMassSetBox(&(this->odeMass), Physics::MaterialTable::GetDensity(this->materialType), 
                    this->size.x, this->size.y, this->size.z);
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
BoxShape::RenderDebug(const matrix44& parentTransform)
{
    if (this->IsAttached())
    {
        // compute resulting model matrix
        matrix44 m;
        m.scale(this->size);
        m *= this->GetTransform();
        m *= parentTransform;
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Box, m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics