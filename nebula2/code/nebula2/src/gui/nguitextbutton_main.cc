//------------------------------------------------------------------------------
//  nguitextbutton_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextbutton.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiTextButton, "nguitextlabel");

//------------------------------------------------------------------------------
/**
*/
nGuiTextButton::nGuiTextButton() :
    pressed(false),
    focus(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTextButton::~nGuiTextButton()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    If mouse is over sensitive area, set the focus flag. Clear focus and
    pressed flag otherwise.
*/
bool
nGuiTextButton::OnMouseMoved(const vector2& mousePos)
{
    if (this->Inside(mousePos))
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
    return nGuiWidget::OnMouseMoved(mousePos);
}

//-----------------------------------------------------------------------------
/**
    If button has focus, set the pressed flag.
*/
bool
nGuiTextButton::OnButtonDown(const vector2& mousePos)
{
    if (this->Inside(mousePos))
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
nGuiTextButton::OnButtonUp(const vector2& mousePos)
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
nGuiTextButton::Render()
{
    if (this->IsShown())
    {
        if (this->pressed)
        {
            nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->pressedBrush);
        }
        else if (this->focus)
        {
            nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->highlightBrush);
        }
        else
        {
            nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);
        }

        // render the text on top
        this->RenderText(this->pressed);
        return true;
    }
    return false;
}
