#ifndef N_GUICOLORSLIDERGROUP_H
#define N_GUICOLORSLIDERGROUP_H
//------------------------------------------------------------------------------
/**
    @class nGuiColorSliderGroup
    @ingroup Gui

    @brief A complete color slider group, consisting of a horizontal intensity
    slider, and a color label.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiformlayout.h"
#include "gui/nguislider2.h"
#include "gui/nguitextlabel.h"
#include "gui/nguicolorlabel.h"
#include "gui/nguicoloradjustwindow.h"

//------------------------------------------------------------------------------
class nGuiColorSliderGroup : public nGuiFormLayout
{
public:
    /// constructor
    nGuiColorSliderGroup();
    /// destructor
    virtual ~nGuiColorSliderGroup();
    /// set the label font
    void SetLabelFont(const nString& f);
    /// get the label font
    const nString& GetLabelFont() const;
    /// set the label text
    void SetLabelText(const nString& t);
    /// get the label text
    const nString& GetLabelText() const;
    /// set relative text label width
    void SetTextLabelWidth(float w);
    /// get relative text label width
    float GetTextLabelWidth() const;
    /// set intensity label width
    void SetIntensityLabelWidth(float w);
    /// get intensity label width
    float GetIntensityLabelWidth() const;
    /// set max color intensity
    void SetMaxIntensity(float i);
    /// get max color intensity
    float GetMaxIntensity() const;
    /// set intensity increment size
    void SetIntensityIncrement(float i);
    /// get intensity increment size
    float GetIntensityIncrement() const;
    /// set current color
    void SetColor(const vector4& c);
    /// get current color
    const vector4& GetColor() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);

private:
    /// compute color intensity from a color value
    static float ComputeColorIntensity(const vector4& color);
    /// compute normalized color from a color value
    static vector4 ComputeNormalizedColor(const vector4& color);
    /// compute color resulting from normalized color value and color intensity
    static vector4 ComputeColor(const vector4& normalizedColor, float colorIntensity);
    /// update widgets when color value has changed
    void UpdateWidgets();

    nString labelFont;
    nString labelText;
    float textLabelWidth;
    float intensityLabelWidth;
    float maxIntensity;
    float intensityIncrement;

    float intensity;
    vector4 normColor;
    vector4 color;

    nRef<nGuiTextLabel>         refTextLabel;
    nRef<nGuiSlider2>           refSlider;
    nRef<nGuiTextLabel>         refIntensityLabel;
    nRef<nGuiColorLabel>        refColorLabel;
    nRef<nGuiColorAdjustWindow> refColorAdjustWindow;
};

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiColorSliderGroup::ComputeColorIntensity(const vector4& color)
{
    return n_max(n_max(color.x, color.y), color.z);
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nGuiColorSliderGroup::ComputeNormalizedColor(const vector4& color)
{
    float intensity = ComputeColorIntensity(color);
    vector4 c = color * (1.0f / intensity);
    c.w = color.w;
    return c;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nGuiColorSliderGroup::ComputeColor(const vector4& normalizedColor, float colorIntensity)
{
    vector4 c = normalizedColor * colorIntensity;
    c.w = normalizedColor.w;
    return c;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiColorSliderGroup::SetLabelFont(const nString& f)
{
    this->labelFont = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiColorSliderGroup::GetLabelFont() const
{
    return this->labelFont;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiColorSliderGroup::SetLabelText(const nString& t)
{
    this->labelText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiColorSliderGroup::GetLabelText() const
{
    return this->labelText;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiColorSliderGroup::SetTextLabelWidth(float w)
{
    this->textLabelWidth = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiColorSliderGroup::GetTextLabelWidth() const
{
    return this->textLabelWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiColorSliderGroup::SetIntensityLabelWidth(float w)
{
    this->intensityLabelWidth = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiColorSliderGroup::GetIntensityLabelWidth() const
{
    return this->intensityLabelWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiColorSliderGroup::SetIntensityIncrement(float i)
{
    this->intensityIncrement = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiColorSliderGroup::GetIntensityIncrement() const
{
    return this->intensityIncrement;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiColorSliderGroup::SetMaxIntensity(float i)
{
    this->maxIntensity = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiColorSliderGroup::GetMaxIntensity() const
{
    return this->maxIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiColorSliderGroup::SetColor(const vector4& c)
{
    this->color = c;
    this->intensity = this->ComputeColorIntensity(c);
    this->normColor = this->ComputeNormalizedColor(c);
    this->UpdateWidgets();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiColorSliderGroup::GetColor() const
{
    return this->color;
}

//------------------------------------------------------------------------------
#endif
