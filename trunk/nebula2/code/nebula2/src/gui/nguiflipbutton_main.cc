//------------------------------------------------------------------------------
//  nguiflipbutton_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiflipbutton.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiFlipButton, "nguitextlabel");

//------------------------------------------------------------------------------
/**
*/
nGuiFlipButton::nGuiFlipButton() :
    mouseOver(false),
    pressed(false),
    curState(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiFlipButton::~nGuiFlipButton()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiFlipButton::OnMouseMoved(const vector2& mousePos)
{
    if (this->Inside(mousePos))
    {
        this->mouseOver = true;
    }
    else
    {
        this->mouseOver = false;
        this->pressed = false;
    }
    return nGuiWidget::OnMouseMoved(mousePos);
}

//-----------------------------------------------------------------------------
/**
    If mouse currently over button, switch to the next state and call the
    script callback command.
*/
bool
nGuiFlipButton::OnButtonDown(const vector2& /*mousePos*/)
{
    if (this->mouseOver)
    {
        this->curState += 1;
        if (this->curState >= this->textArray.Size())
        {
            this->curState = 0;
        }
        this->SetText(this->textArray[this->curState].Get());
        this->triggerSound = true;
        this->pressed = true;
        this->OnAction();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiFlipButton::OnButtonUp(const vector2& /*mousePos*/)
{
    this->pressed = false;
    return true;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiFlipButton::AddState(const char* text)
{
    n_assert(text);
    nString str(text);
    this->textArray.Append(str);
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiFlipButton::SetState(int i)
{
    n_assert((i >= 0) && (i < this->textArray.Size()));
    this->curState = i;
    this->SetText(this->textArray[this->curState].Get());
}

//-----------------------------------------------------------------------------
/**
*/
int
nGuiFlipButton::GetState() const
{
    return this->curState;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiFlipButton::Render()
{
    if (this->IsShown())
    {
        if (this->pressed)
        {
            this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), this->GetPressedBrush());
        }
        else
        {
            this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), this->GetDefaultBrush());
        }

        // render the right text on top
        this->RenderText(this->pressed);

        return true;
    }
    return false;
}

