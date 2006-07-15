//------------------------------------------------------------------------------
//  physics/capsuleshape.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/capsuleshape.h"
#include "gfx2/ngfxserver2.h"

namespace Physics
{
ImplementRtti(Physics::CapsuleShape, Physics::Shape);
ImplementFactory(Physics::CapsuleShape);

//------------------------------------------------------------------------------
/**
*/
CapsuleShape::CapsuleShape() :
    Shape(Capsule),
    radius(1.0f),
    length(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CapsuleShape::~CapsuleShape()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create a capsule object, add it to ODE's collide space, and initialize
    the mass member.
*/
bool
CapsuleShape::Attach(dSpaceID spaceId)
{
    if (Shape::Attach(spaceId))
    {
        dGeomID capsule = dCreateCapsule(0, this->radius, this->length);
        this->AttachGeom(capsule, spaceId);
        dMassSetCapsule(&(this->odeMass), Physics::MaterialTable::GetDensity(this->materialType), 3, this->radius, this->length);
        this->TransformMass();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the capsule shape.
*/
void
CapsuleShape::RenderDebug(const matrix44& parentTransform)
{
    if (this->IsAttached())
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        vector3 capScale(this->radius, this->radius, this->radius);
        vector4 color = this->GetDebugVisualizationColor();

        matrix44 cap0Transform;
        cap0Transform.scale(capScale);
        cap0Transform.translate(vector3(0.0f, 0.0f, this->length * 0.5f));
        cap0Transform *= this->GetTransform();
        cap0Transform *= parentTransform;
        gfxServer->DrawShape(nGfxServer2::Sphere, cap0Transform, color);

        matrix44 cap1Transform;
        cap1Transform.scale(capScale);
        cap1Transform.translate(vector3(0.0f, 0.0f, -this->length * 0.5f));
        cap1Transform *= this->GetTransform();
        cap1Transform *= parentTransform;
        gfxServer->DrawShape(nGfxServer2::Sphere, cap1Transform, color);

        matrix44 cylTransform;
        cylTransform.scale(vector3(this->radius, this->radius, this->length));
        cylTransform *= this->GetTransform();
        cylTransform *= parentTransform;
        gfxServer->DrawShape(nGfxServer2::Cylinder, cylTransform, color);
    }
}

} // namespace Physics
