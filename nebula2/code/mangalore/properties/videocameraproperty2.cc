//------------------------------------------------------------------------------
//  properties/videocameraproperty2.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/videocameraproperty2.h"
#include "managers/focusmanager.h"
#include "input/server.h"
#include "graphics/server.h"
#include "graphics/cameraentity.h"
#include "game/entity.h"
#include "attr/attributes.h"
#include "gfx2/ncamera2.h"
#include "db/query.h"
#include "db/server.h"

namespace Attr
{
    DefineFloat(FilmAspectRatio);
    DefineFloat(NearClipPlane);
    DefineFloat(FarClipPlane);
    DefineMatrix44(ProjectionMatrix);
};


namespace Properties
{
ImplementRtti(Properties::VideoCameraProperty2, Properties::CameraProperty);
ImplementFactory(Properties::VideoCameraProperty2);

using namespace Game;
using namespace Managers;

//------------------------------------------------------------------------------
/**
*/
VideoCameraProperty2::VideoCameraProperty2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
VideoCameraProperty2::~VideoCameraProperty2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
VideoCameraProperty2::SetupDefaultAttributes()
{
    CameraProperty::SetupDefaultAttributes();
    GetEntity()->SetFloat(Attr::FilmAspectRatio, 1.33f);
    GetEntity()->SetFloat(Attr::NearClipPlane, 0.1f);
    GetEntity()->SetFloat(Attr::FarClipPlane, 1000.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
VideoCameraProperty2::OnActivate()
{
    CameraProperty::OnActivate();

    Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
    camera->SetTransform(GetEntity()->GetMatrix44(Attr::Transform));

    // initialize the Maya camera object
    matrix44 m = GetEntity()->GetMatrix44(Attr::Transform);

    this->mayaCamera.SetDefaultEyePos(m.pos_component());
    this->mayaCamera.SetDefaultUpVec(vector3(0.0f, 1.0f, 0.0f));
    this->mayaCamera.SetDefaultCenterOfInterest(m.pos_component() - m.z_component() * 10.0f);
    this->mayaCamera.Initialize();
}

//------------------------------------------------------------------------------
/**
*/
void
VideoCameraProperty2::OnObtainFocus()
{
    // update focus attribute
    GetEntity()->SetBool(Attr::CameraFocus, true);

    Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
    camera->GetCamera().SetAngleOfView(GetEntity()->GetFloat(Attr::FieldOfView));
    camera->GetCamera().SetAspectRatio(GetEntity()->GetFloat(Attr::FilmAspectRatio));
    camera->GetCamera().SetNearPlane(GetEntity()->GetFloat(Attr::NearClipPlane));
    camera->GetCamera().SetFarPlane(GetEntity()->GetFloat(Attr::FarClipPlane)); 
   
    const matrix44 m = GetEntity()->GetMatrix44(Attr::ProjectionMatrix);
    camera->GetCamera().SetProjectionMatrix(m);

}

//------------------------------------------------------------------------------
/**
*/
void
VideoCameraProperty2::OnRender()
{
   if (FocusManager::Instance()->GetInputFocusEntity() == GetEntity())
    {
        // feed the Maya camera object with input
           Input::Server* inputServer = Input::Server::Instance();

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
     
        if (GetEntity()->HasAttr(Attr::AnimPath) && GetEntity()->GetString(Attr::AnimPath).IsValid())
        {
            // entity is animated, use the entity's transform
            camera->SetTransform(GetEntity()->GetMatrix44(Attr::Transform));
        }
        else
        {
            // entity is not animated, use the user input transform
            camera->SetTransform(this->mayaCamera.GetViewMatrix());
        }
    }

    // important: call parent class at the end to apply any shake effects
    CameraProperty::OnRender();
}
}; // namespace Properties