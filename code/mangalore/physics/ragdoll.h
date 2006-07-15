#ifndef PHYSICS_RAGDOLL_H
#define PHYSICS_RAGDOLL_H
//------------------------------------------------------------------------------
/**
    @class Physics::Ragdoll

    A specialized physics composite, which binds physics joints contained
    in the composite to the character joints of a Nebula2 character.

    (C) 2005 Radon Labs GmbH
*/
#include "physics/composite.h"
#include "character/ncharacter2.h"
#include "physics/jointaxis.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Ragdoll : public Composite
{
    DeclareRtti;
	DeclareFactory(Ragdoll);

public:
    /// constructor
    Ragdoll();
    /// destructor
    virtual ~Ragdoll();
    /// attach the composite to the world
    virtual void Attach(dWorldID worldID, dSpaceID dynamicSpaceId, dSpaceID staticSpaceId);
    /// detach the composite from the world
    virtual void Detach();
    /// set pointer to associated Nebula2 character object
    void SetCharacter(nCharacter2* chr);
    /// get pointer to associated Nebula2 character object
    nCharacter2* GetCharacter() const;
    /// bind the ragdoll to the Nebula2 character object
    void Bind();
    /// write physics joint orientations to character joints
    void WriteJoints();
    /// read physics joint orientations from character joints
    void ReadJoints();

private:
    // store backup info of physics joint bind poses
    struct JointInfo
    {
        matrix44 body1Matrix;       // transforms joint from model space to body1's space
        matrix44 body2Matrix;       // transforms the joint from model space to body2's space
        float axisAngle1;           // angle around joint axis 1 between body1 and body2
        float axisAngle2;           // angle around joint axis 2 between body1 and body2
        JointAxis axisParams1;      // axis1 parameters at bind time
        JointAxis axisParams2;      // axis2 parameters at bind time
    };
    /// return bind pose info for a joint
    JointInfo ComputeBindPoseInfoForJoint(Joint* joint);
    /// compute the angle of 2 points around given axis
    float ComputeAxisAngle(const vector3& anchor, const vector3& axis, const vector3& p0, const vector3& p1);
    /// fixed the joint stops for a joint that's going to be re-attached to the world at a new position
    void FixJointStops(JointAxis& jointAxis, const vector3& anchor, const vector3& body1Pos, const vector3& body2Pos, float bindAngle, const JointAxis& bindJointAxis);

    nCharacter2* nebCharacter;
    nArray<JointInfo> bindPoseInfo;
};

RegisterFactory(Ragdoll);

//------------------------------------------------------------------------------
/**
*/
inline
void
Ragdoll::SetCharacter(nCharacter2* chr)
{
    if (this->nebCharacter)
    {
        this->nebCharacter->Release();
        this->nebCharacter = 0;
    }
    if (chr)
    {
        this->nebCharacter = chr;
        this->nebCharacter->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharacter2*
Ragdoll::GetCharacter() const
{
    return this->nebCharacter;
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
