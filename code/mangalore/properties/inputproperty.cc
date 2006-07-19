//------------------------------------------------------------------------------
//  properties/inputproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/inputproperty.h"
#include "managers/focusmanager.h"
#include "game/entity.h"
#include "input/server.h"

namespace Attr
{
    DefineStorableBool(InputFocus);
};

namespace Properties
{
ImplementRtti(Properties::InputProperty, Game::Property);
ImplementFactory(Properties::InputProperty);

using namespace Game;
using namespace Managers;

//------------------------------------------------------------------------------
/**
*/
InputProperty::InputProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
InputProperty::~InputProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/** 
    Makes only sense for live entities.
*/
int
InputProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::SetupDefaultAttributes()
{
    Property::SetupDefaultAttributes();
    GetEntity()->SetBool(Attr::InputFocus, false);
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::OnActivate()
{
    Property::OnActivate();
    if (GetEntity()->GetBool(Attr::InputFocus))
    {
        FocusManager::Instance()->SetInputFocusEntity(GetEntity());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::OnDeactivate()
{
    // clear input focus, if we are the input focus object
    if (this->HasFocus())
    {
        FocusManager::Instance()->SetInputFocusEntity(0);
    }
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    This method is called by the FocusManager when our entity gains the
    input focus. Override this method if your subclass needs to do 
    some initialization when gaining the input focus.
*/
void
InputProperty::OnObtainFocus()
{
    GetEntity()->SetBool(Attr::InputFocus, true);
}

//------------------------------------------------------------------------------
/**
    This method is called by the FocusManager when our entity loses
    the input focus. Override this method if your subclass needs to do any
    cleanup work here.
*/
void
InputProperty::OnLoseFocus()
{
    GetEntity()->SetBool(Attr::InputFocus, false);
}

//------------------------------------------------------------------------------
/**
    This method returns true if our entity has the input focus. This 
    implementation makes sure that 2 properties cannot report that they
    have the input focus by accident.
*/
bool
InputProperty::HasFocus() const
{
    return GetEntity()->GetBool(Attr::InputFocus);
}

//------------------------------------------------------------------------------
/**
    Handle general input.
*/
void
InputProperty::OnBeginFrame()
{
    // only do something if we have the input focus
    if (FocusManager::Instance()->GetInputFocusEntity() == GetEntity())
    {
        Input::Server* inputServer = Input::Server::Instance();
        
        // move focus to next entity when Tab is pressed,
        // only cycle camera focus on Ctrl + Tab
        if (inputServer->GetButtonDown("tab"))
        {
            if (inputServer->GetButtonPressed("ctrl"))
            {
                FocusManager::Instance()->SetCameraFocusToNextEntity();
            }
            else
            {
                FocusManager::Instance()->SetFocusToNextEntity();
            }
        }
    }
}

} // namespace Properties
