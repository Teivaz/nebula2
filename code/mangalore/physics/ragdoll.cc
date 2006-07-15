//------------------------------------------------------------------------------
//  physics/ragdoll.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "physics/ragdoll.h"
#include "physics/balljoint.h"
#include "physics/hingejoint.h"
#include "physics/universaljoint.h"
#include "physics/sliderjoint.h"
#include "physics/hinge2joint.h"
#include "character/ncharskeleton.h"

namespace Physics
{
ImplementRtti(Physics::Ragdoll, Physics::Composite);
ImplementFactory(Physics::Ragdoll);

//------------------------------------------------------------------------------
/**
*/
Ragdoll::Ragdoll() :
    nebCharacter(0),
    bindPoseInfo(24, 16)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Ragdoll::~Ragdoll()
{
    this->SetCharacter(0);
}

//------------------------------------------------------------------------------
/**
    This attaches the ragdoll to the world and activates it.
*/
void
Ragdoll::Attach(dWorldID worldID, dSpaceID dynamicSpaceID, dSpaceID staticSpaceID)
{
    Composite::Attach(worldID, dynamicSpaceID, staticSpaceID);

    // set new auto disable flags for rigid bodies
    int numBodies = this->GetNumBodies();
    int bodyIndex;
    for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
    {
        RigidBody* body = this->GetBodyAt(bodyIndex);
        dBodyID bodyId = body->GetOdeBodyId();
        dBodySetAutoDisableFlag(bodyId, 1);
        dBodySetAutoDisableSteps(bodyId, 2);
        dBodySetAutoDisableLinearThreshold(bodyId, 0.2f);
        dBodySetAutoDisableAngularThreshold(bodyId, 1.0f);
    }
}

//------------------------------------------------------------------------------
/**
    This detaches the ragdoll from the world.
*/
void
Ragdoll::Detach()
{
    n_assert(this->nebCharacter);
    Composite::Detach();
    this->nebCharacter->SetAnimEnabled(true);
}

//------------------------------------------------------------------------------
/**
    Computes the current angle of 2 bodies around a given axis.
*/
float
Ragdoll::ComputeAxisAngle(const vector3& anchor, const vector3& axis, const vector3& p0, const vector3& p1)
{
    line3 axisLine(anchor, anchor + axis);

    // measure the angle on a 2d plane normal to the axis
    float t0 = axisLine.closestpoint(p0);
    float t1 = axisLine.closestpoint(p1);
    vector3 vec0 = p0 - axisLine.point(t0);
    vector3 vec1 = p1 - axisLine.point(t1);
    float a = vector3::angle(vec0, vec1);
    return a;
}

//------------------------------------------------------------------------------
/**
    Returns a JointInfo object which contains information about how the
    joint is positioned relativ to its 2 bodies in the bind pose. The ragdoll
    must currently be in the bindpose for this method to work.
*/
Ragdoll::JointInfo
Ragdoll::ComputeBindPoseInfoForJoint(Joint* joint)
{
    n_assert(joint);
    JointInfo jointInfo;
    jointInfo.axisAngle1 = 0.0f;
    jointInfo.axisAngle2 = 0.0f;
    const RigidBody* body1 = joint->GetBody1();
    const RigidBody* body2 = joint->GetBody2();
    vector3 posBody1;
    vector3 posBody2;
    if (body1)
    {
        posBody1 = body1->GetInitialTransform().pos_component();
    }
    if (body2)
    {
        posBody2 = body2->GetInitialTransform().pos_component();
    }

    // build a matrix which describes the joints position and orientation in the world
    matrix44 jointMatrix;
    if (joint->IsA(BallJoint::RTTI))
    {
        BallJoint* ballJoint = (BallJoint*) joint;
        jointMatrix.pos_component() = ballJoint->GetAnchor();
    }
    else if (joint->IsA(Hinge2Joint::RTTI))
    {
        Hinge2Joint* hinge2Joint = (Hinge2Joint*) joint;
        const vector3& axis1 = hinge2Joint->AxisParams(0).GetAxis();
        const vector3& axis2 = hinge2Joint->AxisParams(1).GetAxis();
        const vector3& anchor = hinge2Joint->GetAnchor();
        const vector3& y = axis1;
        const vector3& z = axis2;
        jointMatrix.y_component() = y;
        jointMatrix.z_component() = z;
        jointMatrix.x_component() = y * z;
        jointMatrix.pos_component() = anchor;
        jointInfo.axisAngle1 = this->ComputeAxisAngle(anchor, axis1, posBody1, posBody2);
        jointInfo.axisAngle2 = this->ComputeAxisAngle(anchor, axis2, posBody1, posBody2);
        jointInfo.axisParams1 = hinge2Joint->AxisParams(0);
        jointInfo.axisParams2 = hinge2Joint->AxisParams(1);
    }
    else if (joint->IsA(HingeJoint::RTTI))
    {
        HingeJoint* hingeJoint = (HingeJoint*) joint;
        const vector3& axis   = hingeJoint->AxisParams().GetAxis();
        const vector3& anchor = hingeJoint->GetAnchor();
        const vector3& y = axis;
        jointMatrix.y_component() = y;
        jointMatrix.x_component() = y * vector3(y.z, y.x, y.y);
        jointMatrix.z_component() = y * jointMatrix.x_component();
        jointMatrix.pos_component() = anchor;
        jointInfo.axisAngle1 = this->ComputeAxisAngle(anchor, axis, posBody1, posBody2);
        jointInfo.axisParams1 = hingeJoint->AxisParams();
    }
    else if (joint->IsA(SliderJoint::RTTI))
    {
        SliderJoint* sliderJoint = (SliderJoint*) joint;
        const vector3& axis = sliderJoint->AxisParams().GetAxis();
        const vector3& x = axis;
        jointMatrix.x_component() = x;
        jointMatrix.y_component() = x * vector3(x.z, x.x, x.y);
        jointMatrix.z_component() = x * jointMatrix.y_component();
        jointInfo.axisAngle1 = this->ComputeAxisAngle(vector3(0.0f, 0.0f, 0.0f), axis, posBody1, posBody2);
        jointInfo.axisParams1 = sliderJoint->AxisParams();
    }
    else if (joint->IsA(UniversalJoint::RTTI))
    {
        UniversalJoint* uniJoint = (UniversalJoint*) joint;
        const vector3& axis1 = uniJoint->AxisParams(0).GetAxis();
        const vector3& axis2 = uniJoint->AxisParams(1).GetAxis();
        const vector3& anchor = uniJoint->GetAnchor();
        const vector3& z = axis1;
        const vector3& y = axis2;
        jointMatrix.z_component() = z;
        jointMatrix.y_component() = y;
        jointMatrix.x_component() = y * z;
        jointMatrix.pos_component() = anchor;
        jointInfo.axisAngle1 = this->ComputeAxisAngle(anchor, axis1, posBody1, posBody2);
        jointInfo.axisAngle2 = this->ComputeAxisAngle(anchor, axis2, posBody1, posBody2);
        jointInfo.axisParams1 = uniJoint->AxisParams(0);
        jointInfo.axisParams2 = uniJoint->AxisParams(1);
    }

    // compute the matrix which transforms the joint from model space into body1's space
    if (body1)
    {
        // compute joint position in body1's space
        jointInfo.body1Matrix = jointMatrix * body1->GetInverseInitialTransform();
    }

    // compute the matrix which transforms the joint from model space into body2's space
    if (body2)
    {
        // compute joint position in body2's space
        jointInfo.body2Matrix = jointMatrix * body2->GetInverseInitialTransform();
    }
    return jointInfo;
}

//------------------------------------------------------------------------------
/**
    This method binds the physics joints to the character joints by resolving
    the joint link name in the rigidbodies and physics joints into
    Nebula2 character joint indices. It also stores the difference matrices
    between the Nebula2 character bind pose and the physics bind pose.
*/
void
Ragdoll::Bind()
{
    n_assert(this->nebCharacter);
    const nCharSkeleton& skeleton = this->nebCharacter->GetSkeleton();

    // convert joint names into joint indices
    int numBodies = this->GetNumBodies();
    int bodyIndex;
    for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
    {
        RigidBody* body = this->GetBodyAt(bodyIndex);
        n_assert(body->IsLinkValid(RigidBody::JointNode));
        const nString& jointName = body->GetLinkName(RigidBody::JointNode);
        int jointIndex = skeleton.GetJointIndexByName(jointName);
        if (-1 == jointIndex)
        {
            n_error("Ragdoll::Bind(): invalid joint name '%s'!", jointName.Get());
        }
        body->SetLinkIndex(jointIndex);
    }
    int numJoints = this->GetNumJoints();
    int jointIndex;
    for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
    {
        // record bind pose info the the joint
        Joint* joint = this->GetJointAt(jointIndex);
        this->bindPoseInfo.Append(this->ComputeBindPoseInfoForJoint(joint));
    }
}

//------------------------------------------------------------------------------
/**
    This method converts the current rigid body transformations into
    character joint transformations and writes them to the Nebula2 character.
    This method should be called after Composite::OnStepAfter() has been executed
    to ensure that all transformations are uptodate.
*/
void
Ragdoll::WriteJoints()
{
    n_assert(this->nebCharacter);
    this->nebCharacter->SetAnimEnabled(false);
    const nCharSkeleton& skeleton = this->nebCharacter->GetSkeleton();

    // get our inverse world space matrix
    matrix44 invCompositeWorldSpace = this->GetTransform();
    invCompositeWorldSpace.invert_simple();

    int numBodies = this->GetNumBodies();
    int bodyIndex;
    for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
    {
        RigidBody* body = this->GetBodyAt(bodyIndex);
        if (body->IsLinkValid(RigidBody::JointNode))
        {
            nCharJoint& charJoint = skeleton.GetJointAt(body->GetLinkIndex());

            // move current global rigid body transform into model space
            const matrix44& bodyWorldSpace = body->GetTransform();
            matrix44 bodyModelSpace = bodyWorldSpace * invCompositeWorldSpace;

            // Nebula2 FIX: rotate by 180 degree
            bodyModelSpace.rotate_y(n_deg2rad(180.0f));

            // move the body's model space transform by the difference between the
            // rigid body's initial pose, and the joint's bind pose
            // NOTE: this difference matrix is constant and should only be
            // computed once during setup
            const matrix44& invBodyPose = body->GetInverseInitialTransform();
            const matrix44& jointPose = charJoint.GetPoseMatrix();
            matrix44 diffMatrix = jointPose * invBodyPose;
            bodyModelSpace = diffMatrix * bodyModelSpace;

            // set the joint's new position directly in model space
            charJoint.SetMatrix(bodyModelSpace);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Fix the joint stops for a joint axis that's going to be re-attached
    to the world.
*/
void
Ragdoll::FixJointStops(JointAxis& curJointAxis,
                       const vector3& anchor,
                       const vector3& body1Pos,
                       const vector3& body2Pos,
                       float bindAngle,
                       const JointAxis& bindJointAxis)
{
/*
    // compute difference between current angle, and bind pose angle
    float curAngle = this->ComputeAxisAngle(anchor, curJointAxis.GetAxis(), body1Pos, body2Pos);
    float diff = curAngle - bindAngle;

    // add the difference to the joint stops
    if (curJointAxis.IsLoStopEnabled())
    {
        float loStop = bindJointAxis.GetLoStop() + diff;
        if (curAngle < loStop)
        {
            loStop = curAngle;
        }
        curJointAxis.SetLoStop(loStop);
    }
    if (curJointAxis.IsHiStopEnabled())
    {
        float hiStop = bindJointAxis.GetHiStop() + diff;
        if (curAngle > hiStop)
        {
            hiStop = curAngle;
        }
        curJointAxis.SetHiStop(hiStop);
    }
*/
}

//------------------------------------------------------------------------------
/**
    Read joint positions from Nebula2 character and initialize the physics
    rigid body and joint positions.

    FIXME: would be nice to get linear and angular velocity info from joint,
    this could be extracted from the animation by the joint while it was alive.
*/
void
Ragdoll::ReadJoints()
{
/*
    n_assert(this->nebCharacter);
    Physics::Server* physicsServer = Physics::Server::Instance();
    const nCharSkeleton& charSkeleton = this->nebCharacter->GetSkeleton();

    // get ode world and collide space id's
    dWorldID odeWorldId = physicsServer->GetOdeWorldId();
    dSpaceID odeSpaceId = physicsServer->GetOdeSpaceId();

    // update rigid body positions
    int numBodies = this->GetNumBodies();
    int bodyIndex;
    for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
    {
        RigidBody* body = this->GetBodyAt(bodyIndex);
        n_assert(body->IsLinkValid(RigidBody::JointNode));
        const nCharJoint& charJoint = charSkeleton.GetJointAt(body->GetLinkIndex());

        // detach body from world
        body->Detach();

        // compute the difference matrix between the joint's bind pose
        // and the rigid body's bind pose position
        const matrix44& invBodyPose = body->GetInverseInitialTransform();
        const matrix44& jointPose = charJoint.GetPoseMatrix();
        matrix44 diffMatrix = jointPose * invBodyPose;
        diffMatrix.invert_simple();

        // get model space position of character joint
        matrix44 charJointMatrix = charJoint.GetMatrix();

        // compute the new body's model space position from the current character joint matrix
        matrix44 bodyModelSpace = diffMatrix * charJointMatrix;

        // move body position into world space
        matrix44 bodyWorldSpace = bodyModelSpace * this->GetTransform();

        // re-attach the body to the world
        body->Attach(odeWorldId, odeSpaceId, bodyWorldSpace);
    }

    // update joint positions and orientations
    int numJoints = this->GetNumJoints();
    int jointIndex;
    for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
    {
        const JointInfo& jointInfo = this->bindPoseInfo[jointIndex];
        Joint* joint = this->GetJointAt(jointIndex);
        const RigidBody* body1 = joint->GetBody1();
        const RigidBody* body2 = joint->GetBody2();
        vector3 body1Pos, body2Pos;
        if (body1)
        {
            body1Pos = body1->GetTransform().pos_component();
        }
        if (body2)
        {
            body2Pos = body2->GetTransform().pos_component();
        }

        // detach joint from the world
        joint->Detach();

        // compute the difference between first body's bind pose position
        // and its current position
        const matrix44& bodyMatrix = joint->GetBody1()->GetTransform();

        // compute new joint position in world space
        matrix44 jointMatrix = jointInfo.body1Matrix * bodyMatrix;

        // setup new anchor, axis and stops for the joint
        if (joint->IsA(BallJoint::RTTI))
        {
            BallJoint* ballJoint = (BallJoint*) joint;
            ballJoint->SetAnchor(jointMatrix.pos_component());
        }
        else if (joint->IsA(Hinge2Joint::RTTI))
        {
            Hinge2Joint* hinge2Joint = (Hinge2Joint*) joint;
            hinge2Joint->AxisParams(0).SetAxis(jointMatrix.y_component());
            hinge2Joint->AxisParams(1).SetAxis(jointMatrix.z_component());
            hinge2Joint->SetAnchor(jointMatrix.pos_component());
            this->FixJointStops(hinge2Joint->AxisParams(0), hinge2Joint->GetAnchor(), body1Pos, body2Pos, jointInfo.axisAngle1, jointInfo.axisParams1);
            this->FixJointStops(hinge2Joint->AxisParams(1), hinge2Joint->GetAnchor(), body1Pos, body2Pos, jointInfo.axisAngle2, jointInfo.axisParams2);
        }
        else if (joint->IsA(HingeJoint::RTTI))
        {
            HingeJoint* hingeJoint = (HingeJoint*) joint;
            hingeJoint->AxisParams().SetAxis(jointMatrix.y_component());
            hingeJoint->SetAnchor(jointMatrix.pos_component());
            this->FixJointStops(hingeJoint->AxisParams(), hingeJoint->GetAnchor(), body1Pos, body2Pos, jointInfo.axisAngle1, jointInfo.axisParams1);
        }
        else if (joint->IsA(SliderJoint::RTTI))
        {
            SliderJoint* sliderJoint = (SliderJoint*) joint;
            sliderJoint->AxisParams().SetAxis(jointMatrix.x_component());
            this->FixJointStops(sliderJoint->AxisParams(), vector3(0.0f, 0.0f, 0.0f), body1Pos, body2Pos, jointInfo.axisAngle1, jointInfo.axisParams1);
        }
        else if (joint->IsA(UniversalJoint::RTTI))
        {
            UniversalJoint* uniJoint = (UniversalJoint*) joint;
            uniJoint->AxisParams(0).SetAxis(jointMatrix.z_component());
            uniJoint->SetAnchor(jointMatrix.pos_component());
            this->FixJointStops(uniJoint->AxisParams(0), uniJoint->GetAnchor(), body1Pos, body2Pos, jointInfo.axisAngle1, jointInfo.axisParams1);
            this->FixJointStops(uniJoint->AxisParams(1), uniJoint->GetAnchor(), body1Pos, body2Pos, jointInfo.axisAngle2, jointInfo.axisParams2);
        }

        // re-attach joint to the world
        joint->Attach(odeWorldId, 0, matrix44::identity);
    }
*/
}


} // namespace Physics
