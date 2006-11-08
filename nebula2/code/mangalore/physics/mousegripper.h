#ifndef PHYSICS_MOUSEGRIPPER_H
#define PHYSICS_MOUSEGRIPPER_H
//------------------------------------------------------------------------------
/**
    @class Physics::MouseGripper

    A MouseGripper lets the user manipulate object positions in the physics
    simulation by grabbing, dragging and releasing them. It's usually
    attached to the mouse, so that the user can intuitively manipulate
    the physics objects around him.

    (C) 2004 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "physics/contactpoint.h"
#include "util/npfeedbackloop.h"
#include "ode/ode.h"

//------------------------------------------------------------------------------
namespace Physics
{
class MouseGripper : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(MouseGripper);

public:
    /// constructor
    MouseGripper();
    /// destructor
    virtual ~MouseGripper();
    /// called before physics frame
    void OnFrameBefore();
    /// called after physics frame
    void OnFrameAfter();
    /// called before one physics step
    void OnStepBefore();
    /// called after one physics step
    void OnStepAfter();
    /// set current mouse pos
    void SetMousePos(const vector2& p);
    /// get stored mouse pos
    const vector2& GetMousePos() const;
    /// set the maximum grabble distance
    void SetMaxDistance(float d);
    /// get the maximum grabble distance
    float GetMaxDistance() const;
    /// get the current physics entity under the mouse
    uint GetEntityIdUnderMouse() const;
    /// get the id of the currently grabbed entity (0 if none)
    uint GetGrabbedEntityId() const;
    /// enable/disable the gripper
    void SetEnabled(bool b);
    /// currently enabled?
    bool IsEnabled() const;
    /// toggle the grip
    void ToggleGrip();
    /// open the grip
    void OpenGrip();
    /// close the grip
    void CloseGrip();
    /// is grip opened
    bool IsGripOpen() const;
    /// render a debug visualization of the mouse gripper
    void RenderDebug();

private:
    /// update the closed gripper position
    void UpdateGripPosition();
    /// update force applied to grabbed rigid body
    void UpdateGripForce();
    /// get rigid body from contact
    RigidBody* GetRigidBodyFromContact() const;

    static const float positionGain;
    static const float positionStepSize;

    vector2 mousePos;
    float maxDistance;
    float curDistance;
    bool enabled;
    bool gripOpen;
    ContactPoint contactPoint;
    vector3 bodyGripPosition;
    dBodyID dummyBodyId;
    dJointID dummyJointId;
    dJointID dummyAMotorId;
    dJointFeedback jointFeedback;
    nPFeedbackLoop<vector3> gripPosition;
};

RegisterFactory(MouseGripper);

//------------------------------------------------------------------------------
/**
*/
inline
void
MouseGripper::SetMousePos(const vector2& p)
{
    this->mousePos = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
MouseGripper::GetMousePos() const
{
    return this->mousePos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MouseGripper::SetMaxDistance(float d)
{
    this->maxDistance = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
MouseGripper::GetMaxDistance() const
{
    return this->maxDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MouseGripper::SetEnabled(bool b)
{
    this->enabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
MouseGripper::IsEnabled() const
{
    return this->enabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MouseGripper::ToggleGrip()
{
    if (this->gripOpen)
    {
        this->CloseGrip();
    }
    else
    {
        this->OpenGrip();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
MouseGripper::IsGripOpen() const
{
    return this->gripOpen;
}

//------------------------------------------------------------------------------
/**
    Returns pointer to physics entity under mouse, if any.
*/
inline
uint
MouseGripper::GetEntityIdUnderMouse() const
{
    return this->contactPoint.GetEntityId();
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
