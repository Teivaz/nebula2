//------------------------------------------------------------------------------
//  nguiicon_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiicon.h"
#include "gui/nguiiconcanvas.h"

nNebulaClass(nGuiIcon, "gui::nguilabel");

//------------------------------------------------------------------------------
/**
*/
nGuiIcon::nGuiIcon() :
    dragging(false)
{
    this->canvasClass = kernelServer->FindClass("nguiiconcanvas");
}

//------------------------------------------------------------------------------
/**
*/
nGuiIcon::~nGuiIcon()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Get pointer to my icon canvas.
*/
nGuiIconCanvas*
nGuiIcon::GetCanvas() const
{
    nGuiWidget* parent = (nGuiWidget*) this->GetParent();
    n_assert(parent && parent->IsA(this->canvasClass));
    return (nGuiIconCanvas*) parent;
}

//------------------------------------------------------------------------------
/**
    Called when dragging this icon should start.
*/
bool
nGuiIcon::OnDragStart(const vector2& mousePos)
{
    if (this->GetCanvas()->OnDragStart(mousePos, this))
    {
        this->dragging = true;
        this->dragStartMousePos = mousePos;
        this->dragStartRect = this->rect;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Returns true when the icon thinks that the provided mouse position
    is a valid drop position.
*/
bool
nGuiIcon::IsValidDropPos(const vector2& mousePos)
{
    return this->GetCanvas()->IsValidDropPos(mousePos, this);
}

//------------------------------------------------------------------------------
/**
    Called when mouse moved and this icon is currently dragged.
    This will change the position of the icon accordingly must
    return true when icon is over a "droppable" position.
*/
void
nGuiIcon::OnDragging(const vector2& mousePos)
{
    n_assert(this->dragging);
    nGuiIconCanvas* canvas = this->GetCanvas();
    canvas->OnDragging(mousePos, this);

    float canvasWidth  = canvas->GetRect().v1.x - canvas->GetRect().v0.x;
    float canvasHeight = canvas->GetRect().v1.y - canvas->GetRect().v0.y;

    // fix widget rectangle
    vector2 dragDist = mousePos - dragStartMousePos;
    rectangle r = this->dragStartRect;
    r.v0 += dragDist;
    r.v1 += dragDist;
    float width = r.width();
    float height = r.height();
    vector2 size = r.v1 - r.v0;
    if (r.midpoint().x < 0.0f)
    {
        r.v0.x = -width * 0.5f;
        r.v1.x = r.v0.x + width;
    }
    if (r.midpoint().y < 0.0f)
    {
        r.v0.y = -height * 0.5f;
        r.v1.y = r.v0.y + height;
    }
    if (r.midpoint().x > canvasWidth)
    {
        r.v0.x = canvasWidth - width * 0.5f;
        r.v1.x = r.v0.x + width;
    }
    if (r.midpoint().y > canvasHeight)
    {
        r.v0.y = canvasHeight - height * 0.5f;
        r.v1.y = r.v0.y + height;
    }
/*
    if (r.v0.x < 0.0f)
    {
        r.v0.x = 0.0f;
        r.v1.x = size.x;
    }
    else if (r.v1.x > canvasWidth)
    {
        r.v1.x = canvasWidth;
        r.v0.x = canvasWidth - size.x;
    }
    if (r.v0.y < 0.0f)
    {
        r.v0.y = 0.0f;
        r.v1.y = size.y;
    }
    else if (r.v1.y > canvasHeight)
    {
        r.v1.y = canvasHeight;
        r.v0.y = canvasHeight - size.y;
    }
*/
    this->rect = r;
}

//------------------------------------------------------------------------------
/**
    Called when this icon has been dropped over a valid position.
*/
void
nGuiIcon::OnDropped(const vector2& mousePos)
{
    this->dragging = false;
    this->GetCanvas()->OnDropped(mousePos, this);
}

//------------------------------------------------------------------------------
/**
    Called when dragging has been cancelled.
*/
void
nGuiIcon::OnDragCancelled(const vector2& mousePos)
{
    this->rect = this->dragStartRect;
    this->dragging = false;
    this->GetCanvas()->OnDragCancelled(mousePos, this);
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiIcon::OnButtonDown(const vector2& mousePos)
{
    if (this->IsShown() && (this->Inside(mousePos)))
    {
        return this->OnDragStart(mousePos);
    }
    return nGuiWidget::OnButtonDown(mousePos);
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiIcon::OnMouseMoved(const vector2& mousePos)
{
    if (this->dragging)
    {
        this->OnDragging(mousePos);
    }
    return nGuiLabel::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiIcon::OnButtonUp(const vector2& mousePos)
{
    if (this->dragging)
    {
        if (this->IsValidDropPos(mousePos))
        {
            this->OnDropped(mousePos);
        }
        else
        {
            this->OnDragCancelled(mousePos);
        }
    }
    return nGuiWidget::OnButtonUp(mousePos);
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiIcon::OnRButtonDown(const vector2& mousePos)
{
    if (this->dragging)
    {
        this->OnDragCancelled(mousePos);
    }
    return nGuiWidget::OnRButtonDown(mousePos);
}
