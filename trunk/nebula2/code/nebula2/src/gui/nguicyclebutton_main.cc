//------------------------------------------------------------------------------
//  nguicyclebutton_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicyclebutton.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiCycleButton, "nguitextlabel");

//------------------------------------------------------------------------------
/**
*/
nGuiCycleButton::nGuiCycleButton() :
    pressed(false),
    focus(false),
    currentTextLabel(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiCycleButton::~nGuiCycleButton()
{
    RemoveAllTextLabels();
}

//-----------------------------------------------------------------------------
/**
    If mouse is over sensitive area, set the focus flag. Clear focus and
    pressed flag otherwise.
*/
bool
nGuiCycleButton::OnMouseMoved(const vector2& mousePos)
{
    if (this->Inside(mousePos))
    {
        this->focus = true;
    }
    else
    {
        this->focus = false;
        this->pressed = false;
    }
    return nGuiWidget::OnMouseMoved(mousePos);
}

//-----------------------------------------------------------------------------
/**
    If button has focus, set the pressed flag.
*/
bool
nGuiCycleButton::OnButtonDown(const vector2& /*mousePos*/)
{
    if (this->focus)
    {
        this->pressed = true;
        this->triggerSound = true;

        // Advance to next label
        currentTextLabel++;
        if (currentTextLabel >= textLabels.Size())
        {
            currentTextLabel = 0;
        }
        SetText(textLabels[currentTextLabel].Get());
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    If pressed flag is set, execute the associated command.
*/
bool
nGuiCycleButton::OnButtonUp(const vector2& mousePos)
{
    if (this->pressed)
    {
        this->pressed = false;
        this->OnAction();
        nGuiWidget::OnButtonUp(mousePos);
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiCycleButton::Render()
{
    if (this->IsShown())
    {
        if (this->pressed)
        {
            this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), this->GetPressedBrush());
        }
        else
        {
            this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), this->GetHighlightBrush());
        }

        // render the text on top
        this->RenderText(this->pressed);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
*/
void nGuiCycleButton::PutTextLabel(const char* label)
{
    n_assert(label != 0);
    n_assert(!HasTextLabel(label));
    textLabels.PushBack(label);
    currentTextLabel = textLabels.Size() - 1;
    SetText(textLabels[currentTextLabel].Get());
}

//-----------------------------------------------------------------------------
/**
*/
bool nGuiCycleButton::HasTextLabel(const char* label) const
{
    n_assert(label != 0);

    for (int i = 0; i < textLabels.Size(); i++)
    {
        if (textLabels[i] == label)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
*/
void nGuiCycleButton::RemoveTextLabel(const char* label)
{
    n_assert(label != 0);
    n_assert(HasTextLabel(label));

    for (int i = 0; i < textLabels.Size(); i++)
    {
        if (textLabels[i] == label)
        {
            textLabels.Erase(i);
            break;
        }
    }

    n_assert(!HasTextLabel(label));
}

//-----------------------------------------------------------------------------
/**
*/
void nGuiCycleButton::RemoveAllTextLabels()
{
    textLabels.Clear();
}


