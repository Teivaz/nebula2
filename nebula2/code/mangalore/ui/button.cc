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
bool
Button::OnMouseMove(const vector2& mousePos)
{
    if (this->IsVisible() && this->IsEnabled() && this->Inside(mousePos))
    {
        this->mouseOver = true;
    }
    else
    {
        this->mouseOver = false;
    }
    return Control::OnMouseMove(mousePos);
}

//------------------------------------------------------------------------------
/**
    Called on left mouse button down. This sets the pressed state of the
    button.
*/
bool
Button::OnLeftButtonDown(const vector2& mousePos)
{
    if (this->IsVisible() && this->IsEnabled())
    {
        // just update the pressed state, this does not handle the event
        if (this->Inside(mousePos))
        {
            this->pressed = true;
        }
        else
        {
            this->pressed = false;
        }
    }
    return Control::OnLeftButtonDown(mousePos);
}

//------------------------------------------------------------------------------
/**
    Called on left mouse button up. This sends the button event.
*/
bool
Button::OnLeftButtonUp(const vector2& mousePos)
{
    bool handled = false;
    if (this->IsVisible() && this->IsEnabled() && this->Inside(mousePos) && this->IsPressed())
    {
        this->PutEvent(this->GetEventName());
        handled = true;
    }
    this->pressed = false;
    return (handled || Control::OnLeftButtonUp(mousePos));
}

//------------------------------------------------------------------------------
/**
    Called on right button down. This may cancel the pressed state.
*/
bool
Button::OnRightButtonDown(const vector2& mousePos)
{
    bool handled = false;
    if (this->pressed)
    {
        this->pressed = false;
        handled = true;
    }
    return (handled || Control::OnRightButtonDown(mousePos));
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
    if (this->IsVisible())
    {
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
}

} // namespace UI

