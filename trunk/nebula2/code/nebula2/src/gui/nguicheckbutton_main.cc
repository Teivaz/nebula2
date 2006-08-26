//------------------------------------------------------------------------------
//  nguicheckbutton_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicheckbutton.h"
#include "gui/nguiserver.h"
#include "gui/nguiwindow.h"

nNebulaScriptClass(nGuiCheckButton, "nguitextlabel");

//------------------------------------------------------------------------------
/**
*/
nGuiCheckButton::nGuiCheckButton() :
    mouseOver(false),
    pressed(false),
    partOfGroup(false)
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
    if(!this->IsPartOfGroup())
    {
        nGuiServer* guiServer = nGuiServer::Instance();
        // check if there exist a top most modal window
        nGuiWindow* topMostWindow = guiServer->GetRootWindowPointer()->GetTopMostWindow();
        bool modalTopMostWindow = false;
        if (topMostWindow && topMostWindow->IsModal())
        {
            modalTopMostWindow = true;
        }

        if (modalTopMostWindow && this->GetOwnerWindow() != topMostWindow)
        {
            // if we are not part of the top most window, and this one is modal
            this->mouseOver = false;
        }
        else
        {
            if (this->Inside(mousePos))
            {
                this->mouseOver = true;
            }
            else
            {
                this->mouseOver = false;
            }
        }
    }
    return nGuiWidget::OnMouseMoved(mousePos);
}

//-----------------------------------------------------------------------------
/**
    If mouse currently over button, toggle the pressed flag. And call the
    script callback command.
*/
bool
nGuiCheckButton::OnButtonDown(const vector2& mousePos)
{
    if (this->mouseOver)
    {
        this->pressed = !this->pressed;
        this->OnAction();
        nGuiServer::Instance()->PlaySound("ButtonClick");
        nGuiTextLabel::OnButtonDown(mousePos);
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
void
nGuiCheckButton::SetMouseOver(bool b)
{
    this->mouseOver = b;
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
        nGuiBrush* brush = &this->defaultBrush;
        if (!this->enabled)
        {
            brush = &this->disabledBrush;
        }
        else if (this->pressed)
        {
            brush = &this->pressedBrush;
        }
        else if ( this->GetMouseOver() )
        {
            brush = &this->highlightBrush;
        }
        else if (this->blinking)
        {
            nTime time = nGuiServer::Instance()->GetTime();
            if (fmod(time, this->blinkRate) > this->blinkRate/2.0)
            {
                brush = &this->highlightBrush;
            }
        }
        nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), *brush);

        // render the text on top
        this->RenderText(this->pressed);
        return true;
    }
    return false;
}
