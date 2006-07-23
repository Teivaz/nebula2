//------------------------------------------------------------------------------
//  nguiiconcanvas_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiiconcanvas.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiIconCanvas, "gui::nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiIconCanvas::nGuiIconCanvas()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiIconCanvas::~nGuiIconCanvas()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiIconCanvas::OnButtonDown(const vector2& mousePos)
{
    bool retval = false;
    if (this->IsShown())
    {
        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetTail(); cur; cur = (nGuiWidget*) cur->GetPred())
        {
            if (cur->OnButtonDown(mousePos))
            {
                retval = true;
                break;
            }
        }

        if (this->Inside(mousePos))
        {
            nGuiServer::Instance()->RunCommand(this, this->buttonDownCommand);
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Called back by child icon which want to be dragged. The canvas should
    return true if it things that dragging this icon is allowed.
*/
bool
nGuiIconCanvas::OnDragStart(const vector2& /*mousePos*/, nGuiIcon* icon)
{
    n_assert(icon);
    return true;
}

//------------------------------------------------------------------------------
/**
    Called back by child icon while dragging.
*/
void
nGuiIconCanvas::OnDragging(const vector2& /*mousePos*/, nGuiIcon* /*icon*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Called back by child icon when the icon has been dropped.
*/
void
nGuiIconCanvas::OnDropped(const vector2& /*mousePos*/, nGuiIcon* /*icon*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Called back by child icon when when icon dragging has been cancelled.
*/
void
nGuiIconCanvas::OnDragCancelled(const vector2& /*mousePos*/, nGuiIcon* /*icon*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Return true when mouse position is over a valid drop position.
    Usually called back by a child icon.
*/
bool
nGuiIconCanvas::IsValidDropPos(const vector2& mousePos, nGuiIcon* /*icon*/)
{
    return this->Inside(mousePos);
}

//------------------------------------------------------------------------------
/**
    Render the canvas background.
*/
bool
nGuiIconCanvas::Render()
{
    if (this->IsShown())
    {
        // render the background resource
        nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);

        // render contained widgets
        nGuiWidget::Render();
        return true;
    }
    return false;
}
