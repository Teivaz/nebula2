//-----------------------------------------------------------------------------
//  nguibutton_main.cc
//  (C) 2003 RadonLabs GmbH
//-----------------------------------------------------------------------------
#include "gui/nguibutton.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiButton, "nguiwidget");

//-----------------------------------------------------------------------------
/**
*/
nGuiButton::nGuiButton() :
    focus(false),
    pressed(false)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
nGuiButton::~nGuiButton()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    If mouse is over sensitive area, set the focus flag. Clear focus and
    pressed flag otherwise.
*/
bool
nGuiButton::OnMouseMoved(const vector2& mousePos)
{
    if (this->Inside(mousePos))
    {
        this->focus = true;
    }
    else
    {
        if (this->IsStickyMouse())
        {
            if (!this->pressed)
            {
                this->focus = false;
            }
        }
        else
        {
            this->focus = false;
            this->pressed = false;
        }
    }
    return nGuiWidget::OnMouseMoved(mousePos);
}

//-----------------------------------------------------------------------------
/**
    If button has focus, set the pressed flag.
*/
bool
nGuiButton::OnButtonDown(const vector2& mousePos)
{
    if (this->Inside(mousePos))
    {
        this->focus = true;
        this->pressed = true;
        this->triggerSound = true;
        nGuiWidget::OnButtonDown(mousePos);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    If pressed flag is set, execute the associated command.
*/
bool
nGuiButton::OnButtonUp(const vector2& mousePos)
{
    if (this->pressed)
    {
        this->pressed = false;
        this->focus = false;
        nGuiWidget::OnButtonUp(mousePos);
        this->OnAction();
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiButton::Render()
{
    if (this->IsShown())
    {
        const char* brush = this->GetDefaultBrush();
        if (!this->enabled)
        {
            brush = this->GetDisabledBrush();
        }
        else if (this->pressed)
        {
            brush = this->GetPressedBrush();
        }
        else if (this->focus)
        {
            brush = this->GetHighlightBrush();
        }
        else if (this->blinking)
        {
            double time = this->refGuiServer->GetTime();
            if (fmod(time, 1.0f) > 0.5f)
            {
                brush = this->GetHighlightBrush();
            }
        }
        this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), brush);
        return true;
    }
    return false;
}

