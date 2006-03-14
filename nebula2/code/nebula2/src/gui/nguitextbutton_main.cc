//------------------------------------------------------------------------------
//  nguitextbutton_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextbutton.h"
#include "gui/nguiserver.h"
#include "gui/nguiwindow.h"

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
    If button has focus, set the pressed flag.
*/
bool
nGuiTextButton::OnButtonDown(const vector2& mousePos)
{
    if (this->Inside(mousePos) && this->GetOwnerWindow()->HasFocus())
    {
        this->focus = true;
        this->pressed = true;
        nGuiServer::Instance()->PlaySound("ButtonClick");
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
void
nGuiTextButton::OnFrame()
{
    nGuiServer* guiServer = nGuiServer::Instance();
    
    // check if there exist a top most modal window
    nGuiWindow* topMostWindow = 0;
    if (guiServer->GetRootWindowPointer()) 
    {
        topMostWindow = guiServer->GetRootWindowPointer()->GetTopMostWindow();
    }
    bool modalTopMostWindow = false;
    if (topMostWindow && topMostWindow->IsModal())
    {
        modalTopMostWindow = true;
    }

    if (modalTopMostWindow && this->GetOwnerWindow() != topMostWindow)
    {
        // if we are not part of the top most window, and this one is modal
        this->focus = false;
        this->pressed = false;
    }
    else
    {
        const vector2 mousePos = guiServer->GetMousePos();
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
    }
    return nGuiWidget::OnFrame();
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiTextButton::Render()
{
    if (this->IsShown())
    {
        if (!this->enabled)
        {
            nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->disabledBrush);
        }
        else if (this->pressed)
        {
            nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->pressedBrush);
        }
        else if (this->focus)
        {
            nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->highlightBrush);
        }
        else if (this->blinking)
        {
            nTime time = nGuiServer::Instance()->GetTime();
            if (fmod(time, this->blinkRate) > this->blinkRate / 2.0)
            {
                nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->highlightBrush);
            }
            else
            {
                nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);
            }
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
