//------------------------------------------------------------------------------
//  nguicheckbutton_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicheckbutton.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiCheckButton, "nguitextlabel");

//------------------------------------------------------------------------------
/**
*/
nGuiCheckButton::nGuiCheckButton() :
    mouseOver(false),
    pressed(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiCheckButton::~nGuiCheckButton()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    If mouse is over sensitive area, set the mouse over flag.    
*/
bool
nGuiCheckButton::OnMouseMoved(const vector2& mousePos)
{
    if (this->Inside(mousePos))
    {
        this->mouseOver = true;
    }
    else
    {
        this->mouseOver = false;
    }
    return nGuiWidget::OnMouseMoved(mousePos);
}

//-----------------------------------------------------------------------------
/**
    If mouse currently over button, toggle the pressed flag. And call the
    script callback command.
*/
bool
nGuiCheckButton::OnButtonDown(const vector2& /*mousePos*/)
{
    if (this->mouseOver)
    {
        this->pressed = !this->pressed;
        this->triggerSound = true;
        this->OnAction();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Manually set the button state.
*/
void
nGuiCheckButton::SetState(bool b)
{
    this->pressed = b;
}

//-----------------------------------------------------------------------------
/**
    Get the current button state.
*/
bool
nGuiCheckButton::GetState() const
{
    return this->pressed;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiCheckButton::GetMouseOver() const
{
    return this->mouseOver;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiCheckButton::Render()
{
    if (this->IsShown())
    {
        // render the background image
        if (this->pressed)
        {
            this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), this->GetPressedBrush());
        }
        else
        {
            this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), this->GetDefaultBrush());
        }

        // render the text on top
        this->RenderText(this->pressed);
        return true;
    }
    return false;
}
