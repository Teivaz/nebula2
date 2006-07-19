#ifndef N_GUIHORISLIDERGROUP_H
#define N_GUIHORISLIDERGROUP_H
//------------------------------------------------------------------------------
/**
    @class nGuiHoriSliderGroup
    @ingroup Gui

    @brief A horizontal slider group consisting of a label, a horizontal slider,
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
    /// set the printf display format (default is Int)
    enum DisplayFormat
    {
        Int,
        Float,
    };

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
    void SetMinValue(float v);
    /// get the slider's minimum value
    float GetMinValue() const;
    /// set the slider's maximum value
    void SetMaxValue(float v);
    /// get the slider's maximum value
    float GetMaxValue() const;
    /// set the slider's current value
    void SetValue(float v);
    /// get the slider's current value
    float GetValue() const;
    /// set the knob size
    void SetKnobSize(float s);
    /// get the knob size
    float GetKnobSize() const;
    /// set increment value
    void SetIncrement(float i);
    /// get increment value
    float GetIncrement() const;
    /// set snap-to-increment flag
    void SetSnapToIncrement(bool b);
    /// get snap-to-increment flag
    bool GetSnapToIncrement() const;
    /// set display format (default is Int)
    void SetDisplayFormat(DisplayFormat f);
    /// get display format
    DisplayFormat GetDisplayFormat() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// set alignment of text in left label
    virtual void SetLeftTextAlignment(nGuiTextLabel::Alignment a);
    /// set alignment of text in right label
    virtual void SetRightTextAlignment(nGuiTextLabel::Alignment a);
    /// set alignment of text in right label
    virtual nGuiSlider2* GetSlider() const;

protected:
    nString labelFont;
    nString leftText;
    nString rightText;
    DisplayFormat displayFormat;
    float leftWidth;
    float rightWidth;
    float minValue;
    float maxValue;
    float curValue;
    float knobSize;
    float increment;
    bool snapToIncrement;
    nGuiTextLabel::Alignment leftTextAlignment;
    nGuiTextLabel::Alignment rightTextAlignment;
    nRef<nGuiTextLabel> refLeftLabel;
    nRef<nGuiTextLabel> refRightLabel;
    nRef<nGuiSlider2>   refSlider;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetSnapToIncrement(bool b)
{
    this->snapToIncrement = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiHoriSliderGroup::GetSnapToIncrement() const
{
    return this->snapToIncrement;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetIncrement(float i)
{
    this->increment = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderGroup::GetIncrement() const
{
    return this->increment;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetDisplayFormat(DisplayFormat f)
{
    this->displayFormat = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiHoriSliderGroup::DisplayFormat
nGuiHoriSliderGroup::GetDisplayFormat() const
{
    return this->displayFormat;
}

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
nGuiHoriSliderGroup::SetMinValue(float v)
{
    this->minValue = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderGroup::GetMinValue() const
{
    return this->minValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetMaxValue(float v)
{
    this->maxValue = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderGroup::GetMaxValue() const
{
    return this->maxValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetValue(float v)
{
    this->curValue = v;
    if (this->refSlider.isvalid())
    {
        this->refSlider->SetVisibleRangeStart(this->curValue - this->minValue);
        this->refSlider->SetVisibleRangeSize(this->knobSize);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderGroup::GetValue() const
{
    return this->curValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderGroup::SetKnobSize(float s)
{
    this->knobSize = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderGroup::GetKnobSize() const
{
    return this->knobSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nGuiHoriSliderGroup::SetLeftTextAlignment(nGuiTextLabel::Alignment a)
{
    this->leftTextAlignment = a;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nGuiHoriSliderGroup::SetRightTextAlignment(nGuiTextLabel::Alignment a)
{
    this->rightTextAlignment = a;
}

//------------------------------------------------------------------------------
#endif
    
