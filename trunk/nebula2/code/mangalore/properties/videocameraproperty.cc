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
*/
void
VideoCameraProperty::OnActivate()
{
    CameraProperty::OnActivate();

    // initialize the Maya camera object
    const matrix44 m = GetEntity()->GetMatrix44(Attr::Transform);
    this->mayaCamera.SetDefaultEyePos(m.pos_component());
    this->mayaCamera.SetDefaultUpVec(GetEntity()->GetVector3(Attr::VideoCameraDefaultUpVec));
    this->mayaCamera.SetDefaultCenterOfInterest(GetEntity()->GetVector3(Attr::VideoCameraCenterOfInterest));
    this->mayaCamera.Initialize();
}

//------------------------------------------------------------------------------
/**
*/
void
VideoCameraProperty::OnObtainFocus()
{
    // update focus attribute
    GetEntity()->SetBool(Attr::CameraFocus, true);

    Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
    camera->GetCamera().SetPerspective(60.0f, 4.0f/3.0f, 0.01f, 5000.0f);

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
        // update the graphics subsystem's camera transform
        Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
        n_assert(camera != 0);
        camera->SetTransform(this->mayaCamera.GetViewMatrix());
    }

    // important: call parent class at the end to apply any shake effects
    CameraProperty::OnRender();
}
//------------------------------------------------------------------------------
/**
    This adds the default attributes to the property.
*/
void
VideoCameraProperty::SetupDefaultAttributes()
{
    CameraProperty::SetupDefaultAttributes();

    GetEntity()->SetVector3(Attr::VideoCameraCenterOfInterest, vector3::zero);
    GetEntity()->SetVector3(Attr::VideoCameraDefaultUpVec, vector3(0.0f, 1.0f, 0.0f));
}
} // namespace Properties






