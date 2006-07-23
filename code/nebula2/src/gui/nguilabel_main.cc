//------------------------------------------------------------------------------
//  nguilabel_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguilabel.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiLabel, "gui::nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiLabel::nGuiLabel() :
    mouseOver(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiLabel::~nGuiLabel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    OnMouseMoved() simply sets the mouseOver flag, if a highlight resource
    is set, rendering will then render the highlighted state of the
    label.
*/
bool
nGuiLabel::OnMouseMoved(const vector2& mousePos)
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

//------------------------------------------------------------------------------
/**
    This simply renders the label using the default bitmap resource.
*/
bool
nGuiLabel::Render()
{
    if (this->IsShown())
    {
        nGuiBrush* brush = &this->defaultBrush;
        if (!this->enabled)
        {
            brush = &this->disabledBrush;
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
        return true;
    }
    return false;

    //return nGuiWidget::Render();
}
