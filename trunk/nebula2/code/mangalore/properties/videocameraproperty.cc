//------------------------------------------------------------------------------
//  properties/videocameraproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/videocameraproperty.h"
#include "managers/focusmanager.h"
#include "input/server.h"
#include "graphics/server.h"
#include "graphics/cameraentity.h"
#include "game/entity.h"
#include "msg/updatetransform.h"
#include "properties/pathanimproperty.h"

// video camera specific attributes
namespace Attr
{
    DefineVector3(VideoCameraCenterOfInterest);
    DefineVector3(VideoCameraDefaultUpVec);
};
namespace Properties
{
ImplementRtti(Properties::VideoCameraProperty, Properties::CameraProperty);
ImplementFactory(Properties::VideoCameraProperty);

using namespace Game;
using namespace Managers;
using namespace Message;

//------------------------------------------------------------------------------
/**
*/
VideoCameraProperty::VideoCameraProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
VideoCameraProperty::~VideoCameraProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This adds the default attributes to the property.
*/
void
VideoCameraProperty::SetupDefaultAttributes()
{
    CameraProperty::SetupDefaultAttributes();

    vector3 identity;
    GetEntity()->SetVector3(Attr::VideoCameraCenterOfInterest, identity);
    GetEntity()->SetVector3(Attr::VideoCameraDefaultUpVec, vector3(0.0f, 1.0f, 0.0f));
}

//------------------------------------------------------------------------------
/**
*/
bool
VideoCameraProperty::Accepts(Msg* msg)
{
    if (msg->CheckId(UpdateTransform::Id)) return true;
    return CameraProperty::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
VideoCameraProperty::HandleMessage(Msg* msg)
{
    if (msg->CheckId(UpdateTransform::Id))
    {
        // this is coming usually from the AnimPath property
        Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
        n_assert(camera != 0);
        camera->SetTransform(((UpdateTransform*)msg)->GetMatrix());
    }
    else
    {
        CameraProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
VideoCameraProperty::OnActivate()
{
    CameraProperty::OnActivate();

    // initialize the Maya camera object
    const matrix44& m = GetEntity()->GetMatrix44(Attr::Transform);
    this->mayaCamera.SetDefaultEyePos(m.pos_component());
    //this->mayaCamera.SetDefaultUpVec(m.y_component());
    //this->mayaCamera.SetDefaultCenterOfInterest(m.pos_component() - m.z_component() * 10.0f);
    this->mayaCamera.SetDefaultCenterOfInterest(GetEntity()->GetAttr(Attr::VideoCameraCenterOfInterest).GetVector3());
    this->mayaCamera.SetDefaultUpVec(GetEntity()->GetAttr(Attr::VideoCameraDefaultUpVec).GetVector3());
    this->mayaCamera.Initialize();
}

//------------------------------------------------------------------------------
/**
*/
void
VideoCameraProperty::OnObtainFocus()
{
    CameraProperty::OnObtainFocus();
}

//------------------------------------------------------------------------------
/**
*/
void
VideoCameraProperty::OnRender()
{
    if (FocusManager::Instance()->GetInputFocusEntity() == GetEntity())
    {
        // feed the Maya camera object with input
        nInputServer* inputServer = nInputServer::Instance();

        if (inputServer->GetButton("ctrlPressed"))
        {
            this->mayaCamera.SetResetButton(inputServer->GetButton("vwrReset"));
            this->mayaCamera.SetLookButton(inputServer->GetButton("lmbPressed"));
            this->mayaCamera.SetPanButton(inputServer->GetButton("mmbPressed"));
            this->mayaCamera.SetZoomButton(inputServer->GetButton("rmbPressed"));
            this->mayaCamera.SetSliderLeft(inputServer->GetSlider("vwrLeft"));
            this->mayaCamera.SetSliderRight(inputServer->GetSlider("vwrRight"));
            this->mayaCamera.SetSliderUp(inputServer->GetSlider("vwrUp"));
            this->mayaCamera.SetSliderDown(inputServer->GetSlider("vwrDown"));
        }

        // update the Maya camera
        this->mayaCamera.Update();
    }

    if (FocusManager::Instance()->GetCameraFocusEntity() == GetEntity())
    {
        if (!(GetEntity()->HasAttr(Attr::AnimPath) && GetEntity()->GetString(Attr::AnimPath).IsValid()))
        {
            // only use the internal matrix if we are not animated
            Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
            n_assert(camera != 0);
            float fov = this->GetEntity()->GetFloat(Attr::FieldOfView);
            camera->GetCamera().SetPerspective(fov, 4.0f/3.0f, 0.1f, 2500.0f);
            camera->SetTransform(this->mayaCamera.GetViewMatrix());
        }
    }

    // important: call parent class at the end to apply any shake effects
    CameraProperty::OnRender();
}

} // namespace Properties






