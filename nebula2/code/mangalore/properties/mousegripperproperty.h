#ifndef PROPERTIES_MOUSEGRIPPERPROPERTY_H
#define PROPERTIES_MOUSEGRIPPERPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::MouseGripperProperty

    A MouseGripperProperty lets the user manipulate object positions in the physics 
    simulation by grabbing, dragging and releasing them. It's usually
    attached to a Entity, using input from the mouse, so that the user can intuitively 
    manipulate the physics objects around him.

    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "physics/entity.h"
#include "physics/contactpoint.h"
#include "ode/ode.h"
#include "util/npfeedbackloop.h"
#include "physics/rigidbody.h"
#include "attr/attributes.h"
#include "game/entity.h"

//------------------------------------------------------------------------------
namespace Attr
{
    // Mouse Gripper specific attributes

    //the maximum grabble distance
    DeclareFloat(MaxDistance);
    DeclareBool(Enabled);
};
//------------------------------------------------------------------------------
namespace Properties
{
class MouseGripperProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(MouseGripperProperty);

public:
    /// constructor
    MouseGripperProperty();
    /// destructor
    virtual ~MouseGripperProperty();
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called on begin of frame
    virtual void OnBeginFrame();
    /// called before movement happens
    virtual void OnMoveBefore();
    /// called after movement has happened
    virtual void OnMoveAfter();
    /// called before rendering happens
    virtual void OnRender();

private:
    /// set current mouse pos
    void SetMousePos(const vector2& p);
    /// get stored mouse pos
    const vector2& GetMousePos() const;
    /// get the current physics entity under the mouse
    uint GetEntityIdUnderMouse() const;
    /// get the id of the currently grabbed entity (0 if none)
    uint GetGrabbedEntityId() const;
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
    /// update the closed gripper position
    void UpdateGripPosition();
    /// update force applied to grabbed rigid body
    void UpdateGripForce();
    /// get rigid body from contact
    Physics::RigidBody* GetRigidBodyFromContact() const;

    static const float positionGain;
    static const float positionStepSize;

    vector2 mousePos;
    float curDistance;
    bool gripOpen;
    Physics::ContactPoint contactPoint;
    vector3 bodyGripPosition;
    dBodyID dummyBodyId;
    dJointID dummyJointId;
    dJointID dummyAMotorId;
    dJointFeedback jointFeedback;
    nPFeedbackLoop<vector3> gripPosition;
};

RegisterFactory(MouseGripperProperty);

//------------------------------------------------------------------------------
/**
*/
inline
void
MouseGripperProperty::SetMousePos(const vector2& p)
{
    this->mousePos = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
MouseGripperProperty::GetMousePos() const
{
    return this->mousePos;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
MouseGripperProperty::IsEnabled() const
{
    return GetEntity()->GetBool(Attr::Enabled);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MouseGripperProperty::ToggleGrip()
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
MouseGripperProperty::IsGripOpen() const
{
    return this->gripOpen;
}

//------------------------------------------------------------------------------
/**
    Returns pointer to physics entity under mouse, if any.
*/
inline
uint
MouseGripperProperty::GetEntityIdUnderMouse() const
{
    return this->contactPoint.GetEntityId();
}

}; // namespace Properties
//------------------------------------------------------------------------------
#endif
