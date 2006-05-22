#ifndef PHYSICS_BOXSHAPE_H
#define PHYSICS_BOXSHAPE_H
//------------------------------------------------------------------------------
/**
    @class Physics::BoxShape

    A box shape in the Physics subsystem.
    
    (C) 2003 RadonLabs GmbH
*/
#include "physics/shape.h"

//------------------------------------------------------------------------------
namespace Physics
{
class BoxShape : public Shape
{
    DeclareRtti;
	DeclareFactory(BoxShape);

public:
    /// constructor
    BoxShape();
    /// destructor
    virtual ~BoxShape();
    /// render debug visualization
    virtual void RenderDebug(const matrix44& t);
    /// attach the shape to the world
    virtual bool Attach(dSpaceID spaceId);
    /// set the size of the box
    void SetSize(const vector3& s);
    /// get the size of the box
    const vector3& GetSize() const;

private:
    vector3 size;
};

RegisterFactory(BoxShape);

//------------------------------------------------------------------------------
/**
*/
inline
void
BoxShape::SetSize(const vector3& s)
{
    n_assert(!this->IsAttached());
    this->size = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
BoxShape::GetSize() const
{
    return this->size;
}

}; // namespace OdePhysics
//------------------------------------------------------------------------------
#endif    
