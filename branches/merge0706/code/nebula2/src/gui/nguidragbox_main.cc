//------------------------------------------------------------------------------
//  nguidragbox_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguidragbox.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiDragBox, "nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiDragBox::nGuiDragBox() :    
    dragThreshold(30.0f / 1024.0f),
    dragging(false),
    dragBoxValid(false),
    dragBoxStartedSent(false)
{
    this->SetDefaultBrush("dragbox");
}

//------------------------------------------------------------------------------
/**
*/
nGuiDragBox::~nGuiDragBox()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Registers the drag box as event listener.
*/
void
nGuiDragBox::OnShow()
{
    nGuiServer::Instance()->RegisterEventListener(this);
    nGuiWidget::OnShow();
}

//------------------------------------------------------------------------------
/**
    Unregisters the drag box as event listener.
*/
void
nGuiDragBox::OnHide()
{
    nGuiWidget::OnHide();
    nGuiServer::Instance()->UnregisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
    Called when left mouse button is pressed down. This initializes a new
    drag operation and emits a DragBoxStarted event
*/
bool
nGuiDragBox::OnButtonDown(const vector2& mousePos)
{
    if (this->IsEnabled())
    {
        this->dragging = true;
        this->dragStartMousePos = mousePos;
        this->dragBoxValid = false;
        this->dragBoxStartedSent = false;
    }
    return nGuiWidget::OnButtonDown(mousePos);
}

//------------------------------------------------------------------------------
/**
    Called when left mouse button is released. If the drag box is valid
    (if the release pos is farther away from the start pos then the drag
    threshold), a DragBoxSelection event will be generated.
*/
bool
nGuiDragBox::OnButtonUp(const vector2& mousePos)
{
    if (this->IsEnabled() && this->dragging)
    {
        this->UpdateDragBox(mousePos);
        this->dragging = false;
        if (this->dragBoxValid)
        {
            nGuiEvent event(this, nGuiEvent::DragBoxFinished);
            nGuiServer::Instance()->PutEvent(event);
        }
    }
    return nGuiWidget::OnButtonUp(mousePos);
}

//------------------------------------------------------------------------------
/**
    Called when right mouse button is pressed. If a drag operation is in
    progress, a DragBoxCancelled event will be generated.
*/
bool
nGuiDragBox::OnRButtonDown(const vector2& mousePos)
{
    if (this->IsEnabled() && this->dragging)
    {
        this->dragging = false;
        nGuiEvent event(this, nGuiEvent::DragBoxCancelled);
        nGuiServer::Instance()->PutEvent(event);
    }
    return nGuiWidget::OnRButtonDown(mousePos);
}

//------------------------------------------------------------------------------
/**
    Manually cancel the drag box.
*/
void
nGuiDragBox::CancelDragging()
{
    if (this->IsEnabled() && this->dragging)
    {
        this->dragging = false;
    }
}

//------------------------------------------------------------------------------
/**
    Called when mouse is moved. If a valid drag operation is in progress,
    a DragBoxUpdated event will be generated.
*/
bool
nGuiDragBox::OnMouseMoved(const vector2& mousePos)
{
    if (this->IsEnabled() && this->dragging)
    {
        this->UpdateDragBox(mousePos);
        if (this->dragBoxValid)
        {
            if (!this->dragBoxStartedSent)
            {
                nGuiEvent event(this, nGuiEvent::DragBoxStarted);
                nGuiServer::Instance()->PutEvent(event);
                this->dragBoxStartedSent = true;
            }
            nGuiEvent event(this, nGuiEvent::DragBoxUpdated);
            nGuiServer::Instance()->PutEvent(event);
        }
    }
    return nGuiWidget::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
    Render the drag box. We assume that our widget rectangle has been updated
    by UpdateDragBox().
*/
bool
nGuiDragBox::Render()
{
    if (this->IsEnabled() &&
        this->IsDragging() &&
        this->IsDragBoxValid() &&
        this->IsShown())
    {
        nGuiServer* guiServer = nGuiServer::Instance();
        vector4 globalColor = guiServer->GetGlobalColor();
        guiServer->SetGlobalColor(vector4(1.0f, 1.0f, 1.0f, 1.0f));
        guiServer->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);
        guiServer->SetGlobalColor(globalColor);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Update the drag box data. This will set the dragBoxValid member if the
    provided mouse position is not within the drag threshold from the start
    mouse position, will update the widget's rectangle, and the 2d- and
    3d-drag box data.
*/
void
nGuiDragBox::UpdateDragBox(const vector2& mousePos)
{
    n_assert(this->dragging);

    // check mouse move distance
    vector2 mouseDist = mousePos - this->dragStartMousePos;
    if ((n_abs(mouseDist.x) > this->dragThreshold) &&
        (n_abs(mouseDist.y) > this->dragThreshold))
    {
        // we have a valid drag box
        this->dragBoxValid = true;

        // update the drag box rectangle
        this->dragBox2D.v0.x = n_saturate(n_min(mousePos.x, this->dragStartMousePos.x));
        this->dragBox2D.v0.y = n_saturate(n_min(mousePos.y, this->dragStartMousePos.y));
        this->dragBox2D.v1.x = n_saturate(n_max(mousePos.x, this->dragStartMousePos.x));
        this->dragBox2D.v1.y = n_saturate(n_max(mousePos.y, this->dragStartMousePos.y));

        // update the widget rectangle
        this->SetRect(this->dragBox2D);
    }
    else
    {
        // mouse is still to close to the start position
        this->dragBoxValid = false;
    }
}

//------------------------------------------------------------------------------
/**
    Return true if a world space position is inside the current drag box.
*/
bool
nGuiDragBox::IsWorldCoordInDragBox(const vector3& pos) const
{
    if (this->dragBoxValid)
    {
        // transform vector to 2d screen space
        vector4 vec4(pos.x, pos.y, pos.z, 1.0f);
        vector4 proj4 = nGfxServer2::Instance()->GetTransform(nGfxServer2::ViewProjection) * vec4;
        vector2 screen2(((proj4.x / proj4.w) + 1.0f) * 0.5f, (((-proj4.y / proj4.w) + 1.0f) * 0.5f));
        return this->dragBox2D.inside(screen2);
    }
    else
    {
        return false;
    }
}
