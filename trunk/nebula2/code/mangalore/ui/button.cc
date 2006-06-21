//------------------------------------------------------------------------------
//  ui/button.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/button.h"

namespace UI
{
ImplementRtti(UI::Button, UI::Control);
ImplementFactory(UI::Button);

//------------------------------------------------------------------------------
/**
*/
Button::Button() :
    mouseOver(false),
    pressed(false),
    visuals(NumVisualStates)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Button::~Button()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Called when the button is created. This will lookup the visuals for
    the diverse visual states of the button from the Nebula2 node hierarchy.
*/
void
Button::OnCreate(Element* parent)
{
    // first hand to parent
    Control::OnCreate(parent);

    // parse attributes
    this->SetText(this->gfxNode->GetStringAttr("rlGuiText"));
    this->SetEventName(this->gfxNode->GetStringAttr("rlGuiEvent"));

    // lookup visuals (NOTE: visuals are optionals)
    this->visuals[Normal]    = (nTransformNode*) this->gfxNode->Find("normal");
    this->visuals[Pressed]   = (nTransformNode*) this->gfxNode->Find("pressed");
    this->visuals[MouseOver] = (nTransformNode*) this->gfxNode->Find("mouseover");
    this->visuals[Disabled]  = (nTransformNode*) this->gfxNode->Find("disabled");
}

//------------------------------------------------------------------------------
/**
    Called on mouse move. This sets the mouse over state of the button,
    and may cancel the pressed state if the mouse moves outside of the button.
*/
void
Button::OnMouseMove(const vector2& mousePos)
{
    if (this->IsEnabled() && this->Inside(mousePos))
    {
        this->mouseOver = true;
    }
    else
    {
        this->mouseOver = false;
    }
    Control::OnMouseMove(mousePos);
}

//------------------------------------------------------------------------------
/**
    Called on left mouse button down. This sets the pressed state of the
    button.
*/
void
Button::OnLeftButtonDown(const vector2& mousePos)
{
    if (this->IsEnabled() && this->Inside(mousePos))
    {
        this->pressed = true;
    }
    else
    {
        this->pressed = false;
    }
    Control::OnLeftButtonDown(mousePos);
}

//------------------------------------------------------------------------------
/**
    Called on left mouse button up. This sends the button event.
*/
void
Button::OnLeftButtonUp(const vector2& mousePos)
{
    if (this->IsEnabled() && this->Inside(mousePos) && this->IsPressed())
    {
        this->PutEvent(this->GetEventName());
    }
    this->pressed = false;
    Control::OnLeftButtonUp(mousePos);
}

//------------------------------------------------------------------------------
/**
    Called on right button down. This may cancel the pressed state.
*/
void
Button::OnRightButtonDown(const vector2& mousePos)
{
    this->pressed = false;
    Control::OnRightButtonDown(mousePos);
}

//------------------------------------------------------------------------------
/**
    Called before rendering happens. This updates the visibility of our
    Nebula2 visual nodes.
*/
void
Button::OnRender()
{
    // first, hide all visuals
    int i;
    for (i = 0; i < NumVisualStates; i++)
    {
        if (this->visuals[i].isvalid())
        {
            this->visuals[i]->SetActive(false);
        }
    }

    // now decide which one is visible
    if (this->IsEnabled())
    {
        if (this->IsPressed() && this->IsMouseOver())
        {
            if (this->visuals[Pressed].isvalid())
            {
                this->visuals[Pressed]->SetActive(true);
            }
        }
        else if (this->IsMouseOver())
        {
            if (this->visuals[MouseOver].isvalid())
            {
                this->visuals[MouseOver]->SetActive(true);
            }
        }
        else
        {
            if (this->visuals[Normal].isvalid())
            {
                this->visuals[Normal]->SetActive(true);
            }
        }
    }
    else
    {
        if (this->visuals[Disabled].isvalid())
        {
            this->visuals[Disabled]->SetActive(true);
        }
    }
}

} // namespace UI

