//------------------------------------------------------------------------------
//  nguislider_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguislider.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiSlider, "nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiSlider::nGuiSlider() :
    rangeMin(0.0f),
    rangeMax(1.0f),
    dragStartRangeMin(0.0f),
    dragStartRangeMax(1.0f),
    stepSize(0.1f),
    horizontal(false),
    mouseOverSlider(false),
    mouseOverNegArea(false),
    mouseOverPosArea(false),
    dragging(false),
    sliderUvs(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f))
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiSlider::~nGuiSlider()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlider::MoveRange(float amount)
{
    float size = (this->rangeMax - this->rangeMin);
    this->rangeMin += amount;
    this->rangeMax += amount;
    if (this->rangeMin < 0.0f)
    {
        this->rangeMin = 0.0f;
        this->rangeMax = size;
    }
    else if (this->rangeMax > 1.0f)
    {
        this->rangeMax = 1.0f;
        this->rangeMin = 1.0f - size;
    }
}

//------------------------------------------------------------------------------
/**
    Compute slider area in mouse coordinates.
*/
rectangle
nGuiSlider::GetMouseSliderArea() const
{
    float x0, y0, x1, y1;
    rectangle screenSpaceRect = this->GetScreenSpaceRect();
    if (this->horizontal)
    {
        x0 = screenSpaceRect.v0.x + (this->rangeMin * (screenSpaceRect.v1.x - screenSpaceRect.v0.x));
        x1 = x0 + ((this->rangeMax - this->rangeMin) * (screenSpaceRect.v1.x - screenSpaceRect.v0.x));
        y0 = screenSpaceRect.v0.y;
        y1 = screenSpaceRect.v1.y;
    }
    else
    {
        x0 = screenSpaceRect.v0.x;
        x1 = screenSpaceRect.v1.x;
        y0 = screenSpaceRect.v0.y + (this->rangeMin * (screenSpaceRect.v1.y - screenSpaceRect.v0.y));
        y1 = y0 + ((this->rangeMax - this->rangeMin) * (screenSpaceRect.v1.y - screenSpaceRect.v0.y));
    }
    return rectangle(vector2(x0, y0), vector2(x1, y1));
}

//------------------------------------------------------------------------------
/**
    Check if mouse is over current slider area.
*/
bool
nGuiSlider::IsInSliderArea(const vector2& mousePos) const
{
    rectangle r = this->GetMouseSliderArea();
    return r.inside(mousePos);
}

//------------------------------------------------------------------------------
/**
    Check if mouse is left of or above slider area.
*/
bool
nGuiSlider::IsInNegSliderArea(const vector2& mousePos) const
{
    rectangle slider = this->GetMouseSliderArea();
    rectangle screenSpaceRect = this->GetScreenSpaceRect();
    if (this->horizontal)
    {
        return ((screenSpaceRect.v0.x <= mousePos.x) && (mousePos.x <= slider.v0.x) && 
                (screenSpaceRect.v0.y <= mousePos.y) && (mousePos.y <= screenSpaceRect.v1.y));
    }
    else
    {
        return ((screenSpaceRect.v0.x <= mousePos.x) && (mousePos.x <= screenSpaceRect.v1.x) &&
                (screenSpaceRect.v0.y <= mousePos.y) && (mousePos.y <= slider.v0.y));
    }
}

//------------------------------------------------------------------------------
/**
    Check if mouse is right of or below slider area.
*/
bool
nGuiSlider::IsInPosSliderArea(const vector2& mousePos) const
{
    rectangle slider = this->GetMouseSliderArea();
    rectangle screenSpaceRect = this->GetScreenSpaceRect();
    if (this->horizontal)
    {
        return ((slider.v1.x <= mousePos.x) && (mousePos.x <= screenSpaceRect.v1.x) && 
                (screenSpaceRect.v0.y <= mousePos.y) && (mousePos.y <= screenSpaceRect.v1.y));
    }
    else
    {
        return ((screenSpaceRect.v0.x <= mousePos.x) && (mousePos.x <= screenSpaceRect.v1.x) &&
                (slider.v1.y <= mousePos.y) && (mousePos.y <= screenSpaceRect.v1.y));
    }
}

//------------------------------------------------------------------------------
/**
    Handle mouse move. If not in drag mode, this will set the flags
    mouseOverSlider, mouseOverNegArea and mouseOverPosArea. The curMouseX
    and curMouseY member are updated as well.
*/
bool
nGuiSlider::OnMouseMoved(const vector2& mousePos)
{
    rectangle screenSpaceRect = this->GetScreenSpaceRect();
    this->curMousePos = mousePos;
    if (this->dragging)
    {
        if (this->horizontal)
        {
            // transform mouse space to slider space
            float dist = (mousePos.x - this->dragStartMousePos.x) / (screenSpaceRect.v1.x - screenSpaceRect.v0.x);
            this->SetRange(this->dragStartRangeMin, this->dragStartRangeMax);
            this->MoveRange(dist);
        }
        else
        {
            float dist = (mousePos.y - this->dragStartMousePos.y) / (screenSpaceRect.v1.y - screenSpaceRect.v0.y);
            this->SetRange(this->dragStartRangeMin, this->dragStartRangeMax);
            this->MoveRange(dist);
        }
        this->OnAction();
    }
    else
    {
        this->mouseOverSlider = false;
        this->mouseOverNegArea = false;
        this->mouseOverPosArea = false;
        if (this->IsInSliderArea(mousePos))
        {
            this->mouseOverSlider = true;
        }
        else if (this->IsInPosSliderArea(mousePos))
        {
            this->mouseOverPosArea = true;
        }
        else if (this->IsInNegSliderArea(mousePos))
        {
            this->mouseOverNegArea = true;
        }
    }
    return nGuiWidget::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
    Handle button down event. If the mouse is over the slider area,
    draggin will be initiated. If the mouse is over the area left or
    right of the slider, the slider will be moved by one tick.
*/
bool
nGuiSlider::OnButtonDown(const vector2& /*mousePos*/)
{
    if (this->mouseOverSlider)
    {
        this->dragging = true;
        this->dragStartMousePos = this->curMousePos;
        this->dragStartRangeMin = this->rangeMin;
        this->dragStartRangeMax = this->rangeMax;
        return true;
    }
    else if (this->mouseOverPosArea)
    {
        // move slider one positive tick
        this->MoveRange(+this->stepSize);
        this->OnAction();
        return true;
    }
    else if (this->mouseOverNegArea)
    {
        // move slider one negative tick
        this->MoveRange(-this->stepSize);
        this->OnAction();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Handle button up event. If currently dragging, drag mode will be left
    and the script command will be invoked.
*/
bool
nGuiSlider::OnButtonUp(const vector2& /*mousePos*/)
{
    if (this->dragging)
    {
        this->dragging = false;
        this->OnAction();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Render the slider widget.
*/
bool
nGuiSlider::Render()
{
    if (this->IsShown())
    {
        // render background
        this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), this->GetBackgroundBrush());

        // render slider
        rectangle sliderRect = this->GetMouseSliderArea();
        if (this->dragging)
        {
            this->refGuiServer->DrawBrush(sliderRect, this->GetPressedBrush());
        }
        else if (this->mouseOverSlider)
        {
            this->refGuiServer->DrawBrush(sliderRect, this->GetHighlightBrush());
        }
        else
        {
            this->refGuiServer->DrawBrush(sliderRect, this->GetDefaultBrush());
        }
        return true;
    }
    return false;
}
