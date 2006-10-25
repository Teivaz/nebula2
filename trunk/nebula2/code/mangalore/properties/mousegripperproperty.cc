//------------------------------------------------------------------------------
//  properties/mousegripper.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "properties/mousegripperproperty.h"
#include "physics/server.h"
#include "physics/rigidbody.h"
#include "physics/composite.h"
#include "gfx2/ngfxserver2.h"
#include "input/server.h"

// Mouse Gripper property specific attributes
namespace Attr
{
    DefineFloat(MaxDistance);
    DefineBool(Enabled);
};
namespace Properties
{
ImplementRtti(Properties::MouseGripperProperty, Game::Property);
ImplementFactory(Properties::MouseGripperProperty);

const float MouseGripperProperty::positionGain = -2.0f;
const float MouseGripperProperty::positionStepSize = 0.001f;

//------------------------------------------------------------------------------
/**
*/
MouseGripperProperty::MouseGripperProperty() :
    gripOpen(true),
    dummyBodyId(0),
    dummyJointId(0),
    dummyAMotorId(0)
{
    memset(&this->jointFeedback, 0, sizeof(this->jointFeedback));
}

//------------------------------------------------------------------------------
/**
*/
MouseGripperProperty::~MouseGripperProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Makes only sense for live entities.
*/
int
MouseGripperProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
    This method is called on begin of frame
*/
void
MouseGripperProperty::OnBeginFrame()
{
    // check if grabbed rigid body has gone away for some reason
    if (!this->gripOpen && (this->contactPoint.GetRigidBodyId() != 0))
    {
        Physics::RigidBody* rigidBody = this->GetRigidBodyFromContact();
        if (!rigidBody)
        {
            this->gripOpen = true;
            this->contactPoint.Clear();
        }
    }
    this->UpdateGripPosition();
    this->UpdateGripForce();
}

//------------------------------------------------------------------------------
/**
    This method is called before movement happens
*/
void
MouseGripperProperty::OnMoveBefore()
{
    Input::Server* inputServer = Input::Server::Instance();
    //get Mouse position
    SetMousePos(inputServer->GetMousePos());

    //if left mouse button is pressed, pick up physic entity
    if (!inputServer->GetButtonPressed("ctrl") && inputServer->GetButtonDown("mouseLMB"))
    {
        CloseGrip();
    }
    //if right button is pressed, force release of physics entity
    if (inputServer->GetButtonDown("mouseRMB") || inputServer->GetButtonUp("mouseLMB"))
    {
        OpenGrip();
    }
}

//------------------------------------------------------------------------------
/**
    This method is called after movement has happened
*/
void
MouseGripperProperty::OnMoveAfter()
{
    // decide if joint should break
    if (!this->gripOpen)
    {
        n_assert(this->dummyJointId);
        dJointFeedback* jf = dJointGetFeedback(this->dummyJointId);
        n_assert(jf);
        vector3 f1, f2;
        Physics::Server::OdeToVector3(jf->f1, f1);
        Physics::Server::OdeToVector3(jf->f2, f2);
        float maxForce = n_max(f1.len(), f2.len());
        if (maxForce > 100.0f)
        {
            // break the joint (open the grip)
            this->OpenGrip();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Returns pointer to rigid body from id's stored in contact point object.
    This may return 0 if the ids are invalid, or the body has gone away.
*/
Physics::RigidBody*
MouseGripperProperty::GetRigidBodyFromContact() const
{
    return this->contactPoint.GetRigidBody();
}

//------------------------------------------------------------------------------
/**
    Open the grip, this will release the currently grabbed physics entity
    (if any).
*/
void
MouseGripperProperty::OpenGrip()
{
    this->contactPoint.Clear();
    this->gripOpen = true;
    if (this->dummyJointId != 0)
    {
        dJointDestroy(this->dummyJointId);
        this->dummyJointId = 0;
    }
    if (this->dummyAMotorId != 0)
    {
        dJointDestroy(this->dummyAMotorId);
        this->dummyAMotorId = 0;
    }
    if (this->dummyBodyId != 0)
    {
        dBodyDestroy(this->dummyBodyId);
        this->dummyBodyId = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Close the grip, if a physics entity is under the mouse, it will be grabbed.
*/
void
MouseGripperProperty::CloseGrip()
{
    if (!this->gripOpen)
    {
        // already closed
        return;
    }

    // check if we are over a physics entity
    if (this->contactPoint.GetEntityId() != 0)
    {
        Physics::Server* physicsServer = Physics::Server::Instance();

        // get entity pointer (if it still exists)
        Physics::RigidBody* rigidBody = this->GetRigidBodyFromContact();
        if (rigidBody)
        {
            // compute the distance from the camera to the contact point
            line3 worldMouseRay = nGfxServer2::Instance()->ComputeWorldMouseRay(this->mousePos, 1.0f);
            vector3 diffVec = worldMouseRay.start() - this->contactPoint.GetPosition();
            this->curDistance = diffVec.len();

            // store the body-relative contact point
            const vector3& globalGripPos = this->contactPoint.GetPosition();
            this->bodyGripPosition = rigidBody->GlobalToLocalPoint(globalGripPos);

            // initialize the global grip position feedback loop
            nTime time = physicsServer->GetTime();
            this->gripPosition.Reset(time, positionStepSize, positionGain, globalGripPos);

            // create a dummy rigid body
            this->dummyBodyId = dBodyCreate(physicsServer->GetOdeWorldId());
            dBodySetGravityMode(this->dummyBodyId, 0);
            dBodySetPosition(this->dummyBodyId, globalGripPos.x, globalGripPos.y, globalGripPos.z);
            dMass mass;
            dMassSetSphereTotal(&mass, 1000.0f, 0.1f);
            dBodySetMass(this->dummyBodyId, &mass);

            // create a ball joint and attach it to the bodies
            this->dummyJointId = dJointCreateBall(physicsServer->GetOdeWorldId(), 0);
            dJointSetFeedback(this->dummyJointId, &this->jointFeedback);
            dJointAttach(this->dummyJointId, rigidBody->GetOdeBodyId(), this->dummyBodyId);
            dJointSetBallAnchor(this->dummyJointId, globalGripPos.x, globalGripPos.y, globalGripPos.z);

            // create an angular motor and attach it to the bodies
            this->dummyAMotorId = dJointCreateAMotor(physicsServer->GetOdeWorldId(), 0);
            dJointAttach(this->dummyAMotorId, rigidBody->GetOdeBodyId(), this->dummyBodyId);

            dJointSetAMotorMode(this->dummyAMotorId, dAMotorEuler);
            dJointSetAMotorAxis(this->dummyAMotorId, 0, 1, 1.0f, 0.0f, 0.0f);
            dJointSetAMotorAxis(this->dummyAMotorId, 2, 2, 0.0f, 0.0f, 1.0f);

            dJointSetAMotorParam(this->dummyAMotorId, dParamVel,   0.0f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamVel2,  0.0f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamVel3,  0.0f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamFMax,  0.5f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamFMax2, 0.5f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamFMax3, 0.5f);

            this->gripOpen = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
    This updates the grip position depending on the current mouse position.
*/
void
MouseGripperProperty::UpdateGripPosition()
{
    Physics::Server* physicsServer = Physics::Server::Instance();

    // do a ray check into the environment, using maximum or current distance
    float rayLen;
    if (this->gripOpen)
    {
        rayLen = GetEntity()->GetFloat(Attr::MaxDistance);
    }
    else
    {
        rayLen = this->curDistance;
    }
    nTime time = physicsServer->GetTime();
    const Physics::ContactPoint* contactPtr = 0;
    Physics::FilterSet excludeSet;
    excludeSet.AddMaterialType(Physics::MaterialTable::StringToMaterialType("Character"));
    contactPtr = physicsServer->GetClosestContactUnderMouse(this->mousePos, rayLen, excludeSet);
    if (this->gripOpen)
    {
        if (contactPtr)
        {
            this->contactPoint = *contactPtr;
            this->gripPosition.Reset(time, positionStepSize, positionGain, contactPtr->GetPosition());
        }
        else
        {
            line3 worldMouseRay = nGfxServer2::Instance()->ComputeWorldMouseRay(this->mousePos, rayLen);
            this->gripPosition.Reset(time, positionStepSize, positionGain, worldMouseRay.end());
            this->contactPoint.Clear();
        }
    }
    else
    {
        line3 worldMouseRay = nGfxServer2::Instance()->ComputeWorldMouseRay(this->mousePos, rayLen);
        this->gripPosition.SetGoal(worldMouseRay.end());
    }
    this->gripPosition.Update(time);
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the gripper.
*/
void
MouseGripperProperty::RenderDebug()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    matrix44 gripTransform;
    vector4 gripColor;
    matrix44 bodyTransform;
    vector4 bodyColor(1.0f, 0.0f, 0.0f, 1.0f);

    gripTransform.scale(vector3(0.1f, 0.1f, 0.1f));
    gripTransform.set_translation(this->gripPosition.GetState());
    gfxServer->BeginShapes();
    if (this->gripOpen)
    {
        gripColor.set(1.0f, 1.0f, 0.0f, 1.0f);
    }
    else
    {
        Physics::RigidBody* rigidBody = this->GetRigidBodyFromContact();
        if (rigidBody)
        {
            bodyTransform.scale(vector3(0.1f, 0.1f, 0.1f));
            bodyTransform.set_translation(rigidBody->LocalToGlobalPoint(this->bodyGripPosition));
            gfxServer->DrawShape(nGfxServer2::Sphere, bodyTransform, bodyColor);
        }
        gripColor.set(1.0f, 0.0f, 1.0f, 1.0f);
    }
    gfxServer->DrawShape(nGfxServer2::Sphere, gripTransform, gripColor);
    gfxServer->EndShapes();
}

//------------------------------------------------------------------------------
/**
    If grip closed around a valid rigid body, apply a force to the
    rigid body which moves the body into the direction of the
    mouse.
*/
void
MouseGripperProperty::UpdateGripForce()
{

    if (!this->gripOpen && (0 != this->contactPoint.GetRigidBodyId()))
    {
        Physics::RigidBody* rigidBody = this->GetRigidBodyFromContact();
        if (rigidBody)
        {
            // update the dummy body's position to correspond with the mouse
            n_assert(this->dummyBodyId != 0);
            rigidBody->SetEnabled(true);
            dBodyEnable(this->dummyBodyId);
            const vector3& pos = this->gripPosition.GetState();
            dBodySetPosition(this->dummyBodyId, pos.x, pos.y, pos.z);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Returns the entity id of the currently grabbed entity, or 0 if nothing
    grabbed.
*/
uint
MouseGripperProperty::GetGrabbedEntityId() const
{
    if (!this->gripOpen && (this->contactPoint.GetEntityId() != 0))
    {
        return this->contactPoint.GetEntityId();
    }
    else
    {
        return 0;
    }
}
//------------------------------------------------------------------------------
/**
*/
void
MouseGripperProperty::OnRender()
{
    //empty
}
//------------------------------------------------------------------------------
/**
*/
void
MouseGripperProperty::SetupDefaultAttributes()
{
    GetEntity()->SetFloat(Attr::MaxDistance, 20.0f);
    GetEntity()->SetBool(Attr::Enabled, true);
}
} // namespace Properties
