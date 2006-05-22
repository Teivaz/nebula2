//------------------------------------------------------------------------------
//  properties/pointnclickinputproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/pointnclickinputproperty.h"
#include "managers/focusmanager.h"
#include "msg/cameradistance.h"
#include "msg/camerareset.h"
#include "msg/cameraorbit.h"
#include "msg/movegoto.h"
#include "msg/movestop.h"
#include "managers/focusmanager.h"
#include "input/server.h"
#include "foundation/factory.h"
#include "game/entity.h"
#include "managers/envquerymanager.h"
#include "managers/timemanager.h"

namespace Properties
{
ImplementRtti(Properties::PointNClickInputProperty, Properties::InputProperty);
ImplementFactory(Properties::PointNClickInputProperty);

using namespace Game;
using namespace Managers;

//------------------------------------------------------------------------------
/**
*/
PointNClickInputProperty::PointNClickInputProperty() :
    moveGotoTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PointNClickInputProperty::~PointNClickInputProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
PointNClickInputProperty::OnBeginFrame()
{
    // only do something if we have the input focus
    if (FocusManager::Instance()->GetInputFocusEntity() == GetEntity())
    {
        Input::Server* inputServer = Input::Server::Instance();
        if (inputServer->GetButton("lmbDown"))
        {
            this->OnLmbDown();
        }
        else if (inputServer->GetButton("lmbPressed"))
        {
            this->OnLmbPressed();
        }
        if (inputServer->GetButton("mmbPressed"))
        {
            this->OnMmbPressed();
        }
        if (inputServer->GetButton("rmbDown"))
        {
            this->OnRmbDown();
        }
        if (inputServer->GetButton("vwrZoomIn"))
        {
            this->OnCameraZoomIn();
        }
        if (inputServer->GetButton("vwrZoomOut"))
        {
            this->OnCameraZoomOut();
        }
    }
    InputProperty::OnBeginFrame();
}

//------------------------------------------------------------------------------
/**
    This sends a move goto message to the intersection point with the world.
*/
void
PointNClickInputProperty::SendMoveGoto()
{
    EnvQueryManager* envQueryMgr = EnvQueryManager::Instance();
    if (envQueryMgr->HasMouseIntersection())
    {
        // send a MoveGoto to the mouse intersection point
        const vector3& mousePos = envQueryMgr->GetMousePos3d();
		Ptr<Message::MoveGoto> msg = Message::MoveGoto::Create();
        msg->SetPosition(mousePos);
        GetEntity()->SendAsync(msg);

        // record the current time for the HandleLMBPressed() method
        this->moveGotoTime = TimeManager::Instance()->GetTime();
    }
}

//------------------------------------------------------------------------------
/**
    Handle the default action. This sends usually a MoveGoto message
    to our entity.
*/
void
PointNClickInputProperty::OnLmbDown()
{
    this->SendMoveGoto();
}

//------------------------------------------------------------------------------
/**
    This handles the case when the left mouse button remains pressed.
    In this case, a MoveGoto message is sent every half a second or so, so
    that the entity follows the mouse.
*/
void
PointNClickInputProperty::OnLmbPressed()
{
    nTime curTime = TimeManager::Instance()->GetTime();
    if ((curTime - this->moveGotoTime) > 0.25f)
    {
        this->SendMoveGoto();
    }
}

//------------------------------------------------------------------------------
/**
    Handle the camera orbit movement.
*/
void
PointNClickInputProperty::OnMmbPressed()
{
    // get horizontal and vertical mouse movement
    Input::Server* inputServer = Input::Server::Instance();
    float x = inputServer->GetSlider("vwrLeft") - inputServer->GetSlider("vwrRight");
    float y = inputServer->GetSlider("vwrUp") - inputServer->GetSlider("vwrDown");
    
    // create CameraOrbit message
	Ptr<Message::CameraOrbit> msg = Message::CameraOrbit::Create();
    msg->SetHorizontalRotation(x);
    msg->SetVerticalRotation(y);
    GetEntity()->SendAsync(msg);
}

//------------------------------------------------------------------------------
/**
    Handle a right mouse button click. This cancels the current movement.
*/
void
PointNClickInputProperty::OnRmbDown()
{
    // create a MoveStop message
	Ptr<Message::MoveStop> msg = Message::MoveStop::Create();
    GetEntity()->SendAsync(msg);
}

//------------------------------------------------------------------------------
/**
    This sends a CameraDistance message if the user zooms in with the
    mouse wheel.
*/
void
PointNClickInputProperty::OnCameraZoomIn()
{
	Ptr<Message::CameraDistance> msg = Message::CameraDistance::Create();
    msg->SetRelativeDistanceChange(-1.0f);
	GetEntity()->SendAsync(msg);
}

//------------------------------------------------------------------------------
/**
    This sends a CameraDistance message if the user zooms out with the
    mouse wheel.
*/
void
PointNClickInputProperty::OnCameraZoomOut()
{
	Ptr<Message::CameraDistance> msg = Message::CameraDistance::Create();
    msg->SetRelativeDistanceChange(+1.0f);
    GetEntity()->SendAsync(msg);
}

}; // namespace Properties
