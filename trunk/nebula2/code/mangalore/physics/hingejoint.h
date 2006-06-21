#ifndef PHYSICS_HINGEJOINT_H
#define PHYSICS_HINGEJOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::HingeJoint

    A hinge joint. See ODE docs for details.

    (C) 2003 RadonLabs GmbH
*/
#include "physics/joint.h"
#include "physics/jointaxis.h"

//------------------------------------------------------------------------------
namespace Physics
{
class HingeJoint : public Joint
{
    DeclareRtti;
	DeclareFactory(HingeJoint);

public:
    /// constructor
    HingeJoint();
    /// destructor
    virtual ~HingeJoint();
    /// initialize the joint (attach to world)
    virtual void Attach(dWorldID worldID, dJointGroupID groupID, const matrix44& m);
    /// transform anchor and axis position/orientation into worldspace and set it in ode
    virtual void UpdateTransform(const matrix44& m);
    /// render debug visualization
    virtual void RenderDebug();
    /// set local anchor position to `v'
    void SetAnchor(const vector3& v);
    /// local anchor position in world coordinates.
    const vector3& GetAnchor() const;
    /// read/write access to axis parameters
    JointAxis& AxisParams();

private:
    vector3 anchor;
    JointAxis axisParams;
};

RegisterFactory(HingeJoint);

//------------------------------------------------------------------------------
/**
*/
inline
void
HingeJoint::SetAnchor(const vector3& v)
{
    this->anchor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
HingeJoint::GetAnchor() const
{
    return this->anchor;
}

//------------------------------------------------------------------------------
/**
*/
inline
JointAxis&
HingeJoint::AxisParams()
{
    return this->axisParams;
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
