//------------------------------------------------------------------------------
//  nguicheckbuttongroup_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicheckbuttongroup.h"
#include "gui/nguicheckbutton.h"

nNebulaScriptClass(nGuiCheckButtonGroup, "nguicheckbutton");

//------------------------------------------------------------------------------
/**
*/
nGuiCheckButtonGroup::nGuiCheckButtonGroup()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiCheckButtonGroup::~nGuiCheckButtonGroup()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This simply distributes the Render() call to the sub objects.
*/
bool
nGuiCheckButtonGroup::Render()
{
    if (this->IsShown())
    {
        nGuiWidget* curWidget;
        for (curWidget = (nGuiWidget*) this->GetHead();
            curWidget;
            curWidget = (nGuiWidget*) curWidget->GetSucc())
        {
            curWidget->Render();
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    This simply distributes the the OnMouseMoved() call to the sub objects.
*/
bool
nGuiCheckButtonGroup::OnMouseMoved(const vector2& mousePos)
{
    bool mouseOverWidget = false;
    nGuiWidget* curWidget;
    for (curWidget = (nGuiWidget*) this->GetHead();
         curWidget;
         curWidget = (nGuiWidget*) curWidget->GetSucc())
    {
        mouseOverWidget |= curWidget->OnMouseMoved(mousePos);
    }
    return mouseOverWidget;
}

//------------------------------------------------------------------------------
/**
    This makes sure that only one button of the button group is in the
    checked state.
*/
bool
nGuiCheckButtonGroup::OnButtonDown(const vector2& /*mousePos*/)
{
    // check if the mouse is over any button in the group
    bool mouseOverGroup = false;
    nGuiCheckButton* curCheckButton;
    for (curCheckButton = (nGuiCheckButton*) this->GetHead();
         curCheckButton;
         curCheckButton = (nGuiCheckButton*) curCheckButton->GetSucc())
    {
        mouseOverGroup |= curCheckButton->GetMouseOver();
    }

    if (mouseOverGroup)
    {
        // if mouse is over any check button in the group, activate
        // this button, and deactivate all others
        nGuiCheckButton* curCheckButton;
        for (curCheckButton = (nGuiCheckButton*) this->GetHead();
             curCheckButton;
             curCheckButton = (nGuiCheckButton*) curCheckButton->GetSucc())
        {
            if (curCheckButton->GetMouseOver())
            {
                curCheckButton->SetState(true);
                curCheckButton->OnAction();
            }
            else
            {
                curCheckButton->SetState(false);
            }
        }

        // invoke the callback command, since our state has changed
        this->OnAction();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Set the new "on" button by index.
*/
void
nGuiCheckButtonGroup::SetOnButtonIndex(int index)
{
    nGuiCheckButton* curCheckButton;
    int curIndex;
    bool indexVisited = false;
    for (curCheckButton = (nGuiCheckButton*) this->GetHead(), curIndex = 0;
         curCheckButton;
         curCheckButton = (nGuiCheckButton*) curCheckButton->GetSucc(), curIndex++)
    {
        if (curIndex == index)
        {
            curCheckButton->SetState(true);
            indexVisited = true;
        }
        else
        {
            curCheckButton->SetState(false);
        }
    }
    if (!indexVisited)
    {
        n_error("nGuiCheckButtonGroup: invalid check button index %d!", index);
    }
}

//------------------------------------------------------------------------------
/**
    Get the button index of the "on" button. Returns -1 if none of the
    buttons is active.
*/
int
nGuiCheckButtonGroup::GetOnButtonIndex() const
{
    nGuiCheckButton* curCheckButton;
    int curIndex;
    for (curCheckButton = (nGuiCheckButton*) this->GetHead(), curIndex = 0;
         curCheckButton;
         curCheckButton = (nGuiCheckButton*) curCheckButton->GetSucc(), curIndex++)
    {
        if (curCheckButton->GetState())
        {
            return curIndex;
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
/**
   Is coordinate in mouse sensitive area.
*/
bool
nGuiCheckButtonGroup::Inside(const vector2& p)
{
    if (this->IsShown())
    {
        bool inside = false;
        nGuiWidget* curWidget;
        for (curWidget = (nGuiWidget*) this->GetHead();
            curWidget;
            curWidget = (nGuiWidget*) curWidget->GetSucc())
        {
            inside |= curWidget->Inside(p);
        }
        return inside;
    }
    else
    {
        return false;
    }
}

