#ifndef PHYSICS_BALLJOINT_H
#define PHYSICS_BALLJOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::BallJoint

    A ball-and-socket joint (see ODE docs for details).

    (C) 2003 RadonLabs GmbH
*/
#include "physics/joint.h"

//------------------------------------------------------------------------------
namespace Physics
{
class BallJoint : public Joint
{
    DeclareRtti;
	DeclareFactory(BallJoint);

public:
    /// constructor
    BallJoint();
    /// destructor
    virtual ~BallJoint();
    /// initialize the joint (attach to world)
    virtual void Attach(dWorldID worldID, dJointGroupID groupID, const matrix44& m);
    /// Update position and orientation.
    virtual void UpdateTransform(const matrix44& m);
    /// render debug visualization
    virtual void RenderDebug();
    /// Set local anchor position to `v'.
    void SetAnchor(const vector3& v);
    /// Local anchor position in world coordinates.
    const vector3& GetAnchor() const;

private:
    vector3 anchor;
};

RegisterFactory(BallJoint);

//------------------------------------------------------------------------------
/**
*/
inline
void 
BallJoint::SetAnchor(const vector3& v)
{
    this->anchor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3& 
BallJoint::GetAnchor() const
{
    return this->anchor;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
