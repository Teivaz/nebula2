//------------------------------------------------------------------------------
//  properties/simplecameraproperty.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/simplecameraproperty.h"
#include "managers/focusmanager.h"
#include "game/entity.h"
#include "graphics/server.h"
#include "graphics/cameraentity.h"
#include "audio/server.h"

#include "msg/cameraorbit.h"
#include "msg/movedirection.h"

namespace Properties
{
ImplementRtti(Properties::SimpleCameraProperty, Properties::CameraProperty);
ImplementFactory(Properties::SimpleCameraProperty);

using namespace Game;
using namespace Managers;

//------------------------------------------------------------------------------
/**
*/
SimpleCameraProperty::SimpleCameraProperty() :
    relHorizontalRotation(0.f),
    relHorizontalRotationFactor(0.15f),
    relVerticalRotation(0.f),
    relVerticalRotationFactor(0.15f),
    relMove(0.f, 0.f, 0.f),
    relMoveFactor(20.f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
SimpleCameraProperty::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    
    if (FocusManager::Instance()->GetCameraFocusEntity() == this->GetEntity())
    {
        // camera look
        if (msg->CheckId(Message::CameraOrbit::Id)) return true;

        // camera move
        if (msg->CheckId(Message::MoveDirection::Id)) return true;
        
        // ?? handle move rotate? Message::MoveRotate
    }
    return CameraProperty::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleCameraProperty::HandleMessage(Message::Msg* msg)
{
    if (msg->CheckId(Message::CameraOrbit::Id))
    {
        this->OnCameraOrbit((Message::CameraOrbit*) msg);
    }
    else if (msg->CheckId(Message::MoveDirection::Id))
    {
        this->OnMoveDirection((Message::MoveDirection*) msg);
    }
    // ?? handle move rotate? Message::MoveRotate
    else
    {
        CameraProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    Called before camera is "rendered". The default camera properties
    applies shake effects to the camera.
*/
void
SimpleCameraProperty::OnRender()
{
    if (FocusManager::Instance()->GetCameraFocusEntity() == GetEntity())
    {
        // get graphics subsystem camera
        Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
        n_assert(camera != 0);

        // get current camera matrix
        matrix44 newTransform = camera->GetTransform();
        
        // save current translation, and set matrix44 to 0,0,0 origin for the rotation
        vector3 curPosition = newTransform.pos_component();
        newTransform.set_translation(vector3(0.f, 0.f, 0.f));
        
        // rotate around global y-axis
        newTransform.rotate(vector3(0.f, 1.f, 0.f), -this->relHorizontalRotation * this->relHorizontalRotationFactor);
        
        // create x-rotation axis
        vector3 xRotAxis(1.f, 0.f, 0.f);
        xRotAxis = newTransform * xRotAxis;
        xRotAxis.y = 0;
        xRotAxis.norm();

        // rotate around x-rotation axis
        newTransform.rotate(xRotAxis, this->relVerticalRotation * -this->relVerticalRotationFactor);

        // restore translation
        newTransform.set_translation(curPosition);    
    
        // translate along loacal z-axis
        newTransform.translate(newTransform.z_component() * this->relMove.z * this->relMoveFactor);
        // translate along loacal x-axis
        newTransform.translate(newTransform.x_component() * this->relMove.x * this->relMoveFactor);

        // clear relative values
        this->relHorizontalRotation = 0.f;
        this->relVerticalRotation = 0.f;
        this->relMove = vector3(0.f, 0.f, 0.f);
        
        camera->SetTransform(newTransform);

        // set the current camera position as audio listener position
        Audio::Listener* listener = Audio::Server::Instance()->GetListener();
        n_assert(listener);
        listener->SetTransform(camera->GetTransform());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleCameraProperty::OnMoveDirection(Message::MoveDirection* msg)
{
    // just add the direction
    this->relMove += msg->GetDirection();
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleCameraProperty::OnCameraOrbit(Message::CameraOrbit* msg)
{
    this->relHorizontalRotation += msg->GetHorizontalRotation();
    this->relVerticalRotation += msg->GetVerticalRotation();
}


}; // namespace Properties
