#ifndef N_GUISLIDER_H
#define N_GUISLIDER_H
//------------------------------------------------------------------------------
/**
    @class nGuiSlider
    @ingroup NebulaGuiSystem
    @brief A scroll bar widget.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

//------------------------------------------------------------------------------
class nGuiSlider : public nGuiWidget
{
public:
    /// constructor
    nGuiSlider();
    /// destructor
    virtual ~nGuiSlider();
    /// Rendering.
    virtual bool Render();
    /// handle mouse moved event
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down event
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up event
    virtual bool OnButtonUp(const vector2& mousePos);
    /// set slider range (between 0 and 1)
    void SetRange(float minVal, float maxVal);
    /// get complete range
    void GetRange(float& minVal, float& maxVal) const;
    /// move range by some amount
    void MoveRange(float amount);
    /// set digital step size
    void SetStepSize(float step);
    /// get step size
    float GetStepSize() const;
    /// set background brush
    void SetBackgroundBrush(const char* name);
    /// get background brush
    const char* GetBackgroundBrush() const;
    /// set horizontal flag
    void SetHorizontal(bool b);
    /// get horizontal flag
    bool GetHorizontal() const;

private:
    /// compute slider mouse coordinates in screen space coords
    rectangle GetMouseSliderArea() const;
    /// check if mouse is on negative side of slider
    bool IsInNegSliderArea(const vector2& mousePos) const;
    /// check if mouse is over slider rect
    bool IsInSliderArea(const vector2& mousePos) const;
    /// check if mouse is on positive side of slider
    bool IsInPosSliderArea(const vector2& mousePos) const;

    float rangeMin;
    float rangeMax;
    float dragStartRangeMin;
    float dragStartRangeMax;
    float stepSize;
    bool horizontal;

    vector2 dragStartMousePos;
    vector2 curMousePos;

    bool mouseOverSlider;
    bool mouseOverNegArea;
    bool mouseOverPosArea;
    bool dragging;

    rectangle sliderUvs;

    nString backgroundBrush;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlider::SetRange(float minVal, float maxVal)
{
    if (minVal < 0.0f)      minVal = 0.0f;
    else if (minVal > 1.0f) minVal = 1.0f;
    if (maxVal < 0.0f)      maxVal = 0.0f;
    else if (maxVal > 1.0f) maxVal = 1.0f;
    n_assert(minVal < maxVal);
    this->rangeMin = minVal;
    this->rangeMax = maxVal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlider::GetRange(float& minVal, float& maxVal) const
{
    minVal = this->rangeMin;
    maxVal = this->rangeMax;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlider::SetStepSize(float step)
{
    this->stepSize = step;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiSlider::GetStepSize() const
{
    return this->stepSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlider::SetHorizontal(bool b)
{
    this->horizontal = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiSlider::GetHorizontal() const
{
    return this->horizontal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlider::SetBackgroundBrush(const char* name)
{
    this->backgroundBrush = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiSlider::GetBackgroundBrush() const
{
    return this->backgroundBrush.IsEmpty() ? 0 : this->backgroundBrush.Get();
}

//------------------------------------------------------------------------------
#endif

