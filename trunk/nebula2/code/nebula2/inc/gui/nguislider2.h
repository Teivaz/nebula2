#ifndef N_GUISLIDER2_H
#define N_GUISLIDER2_H
//------------------------------------------------------------------------------
/**
    A vertical or horizontal slider widget.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiformlayout.h"

class nGuiButton;
class nGuiLabel;

//------------------------------------------------------------------------------
class nGuiSlider2 : public nGuiFormLayout
{
public:
    /// constructor
    nGuiSlider2();
    /// destructor
    virtual ~nGuiSlider2();
    /// set the overall range size
    void SetRangeSize(float i);
    /// get the overall range size
    float GetRangeSize() const;
    /// set the beginning of the visible range
    void SetVisibleRangeStart(float i);
    /// get the beginning of the visible range
    float GetVisibleRangeStart() const;
    /// set the size of the visible range
    void SetVisibleRangeSize(float i);
    /// get the size of the visible range
    float GetVisibleRangeSize() const;
    /// horizontal flag (default is vertical)
    void SetHorizontal(bool b);
    /// get horizontal flag
    bool GetHorizontal() const;
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// called when widget position or size changes
    virtual void OnRectChange(const rectangle& newRect);
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// rendering
    virtual bool Render();

private:
    /// updates the layout attachment offsets of the knob
    void UpdateKnobLayout(const rectangle& newSliderRect);
    /// begin dragging the slider
    void BeginSliderDrag();
    /// end dragging the slider
    void EndSliderDrag();
    /// cancel dragging the slider
    void CancelSliderDrag();
    /// handle knob dragging
    void HandleDrag(const vector2& mousePos);
    /// move slider one position into positive direction
    void MoveSliderPos();
    /// move slider one position into negative direction
    void MoveSliderNeg();

    float overallSize;
    float visibleStart;
    float visibleSize;
    bool rangeDirty;

    nRef<nGuiButton>     refNegButton;
    nRef<nGuiButton>     refPosButton;
    nRef<nGuiButton>     refKnob;

    vector2 arrowBtnSize;
    int knobNegEdgeLayoutIndex;
    int knobPosEdgeLayoutIndex;

    bool dragging;
    bool horizontal;
    vector2 startDragMousePos;
    float dragVisibleStart;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlider2::SetHorizontal(bool b)
{
    this->horizontal = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiSlider2::GetHorizontal() const
{
    return this->horizontal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlider2::SetRangeSize(float i)
{
    if (i != this->overallSize)
    {
        this->overallSize = i;
        this->rangeDirty = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiSlider2::GetRangeSize() const
{
    return this->overallSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlider2::SetVisibleRangeStart(float i)
{
    if (i != this->visibleStart)
    {
        this->visibleStart = i;
        this->rangeDirty = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiSlider2::GetVisibleRangeStart() const
{
    return this->visibleStart;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlider2::SetVisibleRangeSize(float i)
{
    if (i != this->visibleSize)
    {
        this->visibleSize = i;
        this->rangeDirty = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiSlider2::GetVisibleRangeSize() const
{
    return this->visibleSize;
}

//------------------------------------------------------------------------------
#endif    
