#ifndef N_GUIHORISLIDERGROUP_H
#define N_GUIHORISLIDERGROUP_H
//------------------------------------------------------------------------------
/**
    @class nGuiHoriSliderGroup
    @ingroup Gui

    A horizontal slider group consisting of a label, a horizontal slider,
    and a text label which displays the current slider's numerical value
    as a printf-formatted string.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiformlayout.h"
#include "gui/nguislider2.h"
#include "gui/nguitextlabel.h"

//------------------------------------------------------------------------------
class nGuiHoriSliderGroup : public nGuiFormLayout
{
public:
    /// constructor
    nGuiHoriSliderGroup();
    /// destructor
    virtual ~nGuiHoriSliderGroup();
    /// set the label font
    void SetLabelFont(const char* f);
    /// get the label font
    const char* GetLabelFont() const;
    /// set text to left of slider (a printf formatted string)
    void SetLeftText(const char* t);
    /// get text to left of slider
    const char* GetLeftText() const;
    /// set text to right of slider (a printf formatted string)
    void SetRightText(const char* t);
    /// get text to right of slider
    const char* GetRightText() const;
    /// set relative width of left text label
    void SetLeftWidth(float w);
    /// get relative width of left text label
    float GetLeftWidth() const;
    /// set relative width of right text label
    void SetRightWidth(float w);
    /// get relative width of right text label
    float GetRightWidth() const;
    /// set the slider's minimum value
    void SetMinValue(int v);
    /// get the slider's minimum value
    int GetMinValue() const;
    /// set the slider's maximum value
    void SetMaxValue(int v);
    /// get the slider's maximum value
    int GetMaxValue() const;
    /// set the slider's current value
    void SetValue(int v);
    /// get the slider's current value
    int GetValue() const;
    /// set the knob size
    void SetKnobSize(int s);
    /// get the knob size
    int GetKnobSize() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);

protected:
    nString labelFont;
    nString leftText;
    nString rightText;
    float leftWidth;
    float rightWidth;
    int minValue;
    int maxValue;
    int curValue;
    int knobSize;
    nRef<nGuiTextLabel> refLeftLabel;
    nRef<nGuiTextLabel> refRightLabel;
    nRef<nGuiSlider2>   refSlider;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetLabelFont(const char* f)
{
    n_assert(f);
    this->labelFont = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiHoriSliderGroup::GetLabelFont() const
{
    return this->labelFont.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetLeftWidth(float w)
{
    this->leftWidth = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderGroup::GetLeftWidth() const
{
    return this->leftWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetRightWidth(float w)
{
    this->rightWidth = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderGroup::GetRightWidth() const
{
    return this->rightWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetLeftText(const char* t)
{
    this->leftText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiHoriSliderGroup::GetLeftText() const
{
    return this->leftText.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetRightText(const char* t)
{
    this->rightText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiHoriSliderGroup::GetRightText() const
{
    return this->rightText.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetMinValue(int v)
{
    this->minValue = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiHoriSliderGroup::GetMinValue() const
{
    return this->minValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetMaxValue(int v)
{
    this->maxValue = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiHoriSliderGroup::GetMaxValue() const
{
    return this->maxValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetValue(int v)
{
    this->curValue = v;
    if (this->refSlider.isvalid())
    {
        this->refSlider->SetVisibleRangeStart(float(this->curValue - this->minValue));
        this->refSlider->SetVisibleRangeSize(float(this->knobSize));
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiHoriSliderGroup::GetValue() const
{
    return this->curValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetKnobSize(int s)
{
    this->knobSize = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiHoriSliderGroup::GetKnobSize() const
{
    return this->knobSize;
}

//------------------------------------------------------------------------------
#endif
    
