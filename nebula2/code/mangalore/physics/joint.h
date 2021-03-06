#ifndef PHYSICS_JOINT_H
#define PHYSICS_JOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::Joint

    A joint (also known as constraint) connects to rigid bodies. Subclasses
    of joint implement specific joint types.

    (C) 2005 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "ode/ode.h"

//------------------------------------------------------------------------------
namespace Physics
{
class RigidBody;

class Joint : public Foundation::RefCounted
{
    DeclareRtti;

public:
    /// joint types
    enum Type
    {
        BallJoint = 0,
        HingeJoint,
        UniversalJoint,
        SliderJoint,
        Hinge2Joint,
        AMotor,

        NumJointTypes,
        InvalidType,
    };

    /// constructor
    Joint(Type t);
    /// destructor
    virtual ~Joint() = 0;
    /// initialize the joint (attach to world)
    virtual void Attach(dWorldID worldID, dJointGroupID groupID, const matrix44& m);
    /// uninitialize the joint (detach from world)
    virtual void Detach();
    /// currently attached to world?
    bool IsAttached() const;
    /// get the joint type
    Type GetType() const;
    /// update position and orientation
    virtual void UpdateTransform(const matrix44& m);
    /// render debug visualization
    virtual void RenderDebug();
    /// set the 2 bodies connected by the joint (0 pointers are valid)
    void SetBodies(RigidBody* body1, RigidBody* body2);
    /// get the first body
    const RigidBody* GetBody1() const;
    /// get the second body
    const RigidBody* GetBody2() const;
    /// set optional link name (for linking to a character joint)
    void SetLinkName(const nString& n);
    /// get optional link name
    const nString& GetLinkName() const;
    /// return true if a link name has been set
    bool IsLinkValid() const;
    /// set link index
    void SetLinkIndex(int i);
    /// get link index
    int GetLinkIndex() const;
    /// get ode joint id
    dJointID GetJointId() const;

protected:
    /// get a debug visualization color
    vector4 GetDebugVisualizationColor() const;

    Type type;
    dJointID odeJointId;
    Ptr<RigidBody> rigidBody1;
    Ptr<RigidBody> rigidBody2;
    nString linkName;
    int linkIndex;
    bool isAttached;
};

//------------------------------------------------------------------------------
/**
    Get the joint type.

    @return     the joint type
*/
inline
Joint::Type
Joint::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
    Set optional link name. This is usually the name of a Nebula2
    character joint (for implementing ragdolls).
*/
inline
void
Joint::SetLinkName(const nString& n)
{
    this->linkName = n;
}

//------------------------------------------------------------------------------
/**
    Get optional link name.
*/
inline
const nString&
Joint::GetLinkName() const
{
    return this->linkName;
}

//------------------------------------------------------------------------------
/**
    Set link index. This is the link name converted to some index, for
    instance a joint index.
*/
inline
void
Joint::SetLinkIndex(int i)
{
    this->linkIndex = i;
}

//------------------------------------------------------------------------------
/**
    Get link index.
*/
inline
int
Joint::GetLinkIndex() const
{
    return this->linkIndex;
}

//------------------------------------------------------------------------------
/**
    Return true if this joint has a link name set.
*/
inline
bool
Joint::IsLinkValid() const
{
    return this->linkName.IsValid();
}

//------------------------------------------------------------------------------
/**
    Return true if joint is currently attached to world.
*/
inline
bool
Joint::IsAttached() const
{
    return (0 != this->odeJointId);
}

//------------------------------------------------------------------------------
/**
    Return a debug visualization color.
*/
inline
vector4
Joint::GetDebugVisualizationColor() const
{
    return vector4(1.0f, 0.0f, 1.0f, 1.0f);
}
//------------------------------------------------------------------------------
/**
*/
inline
dJointID
Joint::GetJointId() const
{
    return this->odeJointId;
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
