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
void
nGuiButton::OnFrame()
{
    const vector2 mousePos = nGuiServer::Instance()->GetMousePos();
    if (this->Inside(mousePos) && this->GetOwnerWindow()->HasFocus())
    {
        this->focus = true;
    }
    else
    {
        if (!this->IsStickyMouse())
        {
            this->focus = false;
            this->pressed = false;
        }
    }
    return nGuiWidget::OnFrame();
}

//-----------------------------------------------------------------------------
/**
    If button has focus, set the pressed flag.
*/
bool
nGuiButton::OnButtonDown(const vector2& mousePos)
{
    if (this->Inside(mousePos) && this->GetOwnerWindow()->HasFocus())
    {
        this->focus = true;
        this->pressed = true;
        nGuiServer::Instance()->PlaySound(nGuiSkin::ButtonClick);
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
        nGuiBrush* brush = &this->defaultBrush;
        if (!this->IsEnabled())
        {
            brush = &this->disabledBrush;
        }
        else if (this->pressed)
        {
            brush = &this->pressedBrush;
        }
        else if (this->focus)
        {
            brush = &this->highlightBrush;
        }
        else if (this->blinking)
        {
            double time = nGuiServer::Instance()->GetTime();
            if (fmod(time, this->blinkRate) > this->blinkRate/2.0)
            {
                brush = &this->highlightBrush;
            }
        }
        nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), *brush);
        return true;
    }
    return false;
}

