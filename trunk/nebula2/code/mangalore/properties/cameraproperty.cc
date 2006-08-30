//------------------------------------------------------------------------------
//  properties/cameraproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/cameraproperty.h"
#include "managers/focusmanager.h"
#include "game/entity.h"
#include "graphics/server.h"
#include "graphics/cameraentity.h"
#include "audio/server.h"

// camera specific attributes
namespace Attr
{
    DefineStorableBool(CameraFocus);
    DefineFloat(FieldOfView);
};

namespace Properties
{
ImplementRtti(Properties::CameraProperty, Game::Property);
ImplementFactory(Properties::CameraProperty);

using namespace Game;
using namespace Managers;

//------------------------------------------------------------------------------
/**
*/
CameraProperty::CameraProperty()
{
    this->shakeEffectHelper.SetMaxDisplacement(vector3(0.5f, 0.5f, 0.25f));
    this->shakeEffectHelper.SetMaxTumble(vector3(5.0f, 5.0f, 5.0f));
}

//------------------------------------------------------------------------------
/**
*/
CameraProperty::~CameraProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This property makes only sense for live entities.
*/
int
CameraProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
*/
void
CameraProperty::SetupDefaultAttributes()
{
    Property::SetupDefaultAttributes();
    GetEntity()->SetBool(Attr::CameraFocus, true);
    GetEntity()->SetFloat(Attr::FieldOfView, 60.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
CameraProperty::OnActivate()
{
    Property::OnActivate();
    if (GetEntity()->GetBool(Attr::CameraFocus))
    {
        FocusManager::Instance()->SetCameraFocusEntity(GetEntity());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CameraProperty::OnDeactivate()
{
    // clear camera focus, if we are the camera focus object
    if (this->HasFocus())
    {
        FocusManager::Instance()->SetCameraFocusEntity(0);
    }
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    This method is called by the FocusManager when our entity gains the
    camera focus. Override this method if your subclass needs to do
    some initialization when gaining the camera focus.
*/
void
CameraProperty::OnObtainFocus()
{
    // update focus attribute
    GetEntity()->SetBool(Attr::CameraFocus, true);

    // update the field of view angle
    Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
    n_assert(camera != 0);
    camera->GetCamera().SetAngleOfView(GetEntity()->GetFloat(Attr::FieldOfView));
}

//------------------------------------------------------------------------------
/**
    This method is called by the FocusManager when our entity loses
    the camera focus. Override this method if your subclass needs to do any
    cleanup work here.
*/
void
CameraProperty::OnLoseFocus()
{
    // update focus attribute
    GetEntity()->SetBool(Attr::CameraFocus, false);
}

//------------------------------------------------------------------------------
/**
    This method returns true if our entity has the camera focus. This
    implementation makes sure that 2 properties cannot report that they
    have the camera focus by accident.
*/
bool
CameraProperty::HasFocus() const
{
    return GetEntity()->GetBool(Attr::CameraFocus);
}

//------------------------------------------------------------------------------
/**
    Called before camera is "rendered". The default camera properties
    applies shake effects to the camera.
*/
void
CameraProperty::OnRender()
{
    if (FocusManager::Instance()->GetCameraFocusEntity() == GetEntity())
    {
        // get graphics subsystem camera
        Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
        n_assert(camera != 0);
        this->shakeEffectHelper.SetCameraTransform(camera->GetTransform());
        this->shakeEffectHelper.Update();
        camera->SetTransform(this->shakeEffectHelper.GetShakeCameraTransform());

        // set the current camera position as audio listener position
        Audio::Listener* listener = Audio::Server::Instance()->GetListener();
        n_assert(listener);
        listener->SetTransform(camera->GetTransform());
    }
}

} // namespace Properties
