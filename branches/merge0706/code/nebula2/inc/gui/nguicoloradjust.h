#ifndef N_GUICOLORADJUST_H
#define N_GUICOLORADJUST_H
//------------------------------------------------------------------------------
/**
    @class nGuiColorAdjust
    @ingroup Gui

    @brief To select a rgb-color.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nGuiHoriSliderGroup;
class nGuiTextButton;
class nGuiColorLabel;
class nGuiColorPicker;

//------------------------------------------------------------------------------
class nGuiColorAdjust : public nGuiFormLayout
{
public:
    /// constructor
    nGuiColorAdjust();
    /// destructor
    virtual ~nGuiColorAdjust();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// set color
    void SetColor(const vector4& color);
    /// get color
    const vector4& GetColor() const;
    
private:
    /// Adjusts the color to the brightness
    void UpdateColor();
    /// Adjusts the brightness to the color
    void UpdateBrightness();
    /// Updates the used widgets
    void UpdateWidgets(int sliderFlags);

    enum SliderFlags
    {
        Brightness = (1<<0),
        Red   = (1<<1),
        Green = (1<<2),
        Blue  = (1<<3),
        Alpha = (1<<4),

        AllSliders = (Brightness | Red | Green | Blue | Alpha),
    };

    bool noManualBrightnessChange;
    bool noManualRedChange;
    bool noManualGreenChange;
    bool noManualBlueChange;

    float brightness;
    vector4 color;

    nRef<nGuiHoriSliderGroup> refBrightnessSlider;
    nRef<nGuiHoriSliderGroup> refRedValueSlider;
    nRef<nGuiHoriSliderGroup> refGreenValueSlider;
    nRef<nGuiHoriSliderGroup> refBlueValueSlider;
    nRef<nGuiHoriSliderGroup> refAlphaValueSlider;
    nRef<nGuiColorLabel> refCurrentColor;
    nRef<nGuiColorPicker> refColorPicker;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiColorAdjust::SetColor(const vector4& color)
{
    this->color.set(color);
    this->UpdateBrightness();
    this->UpdateWidgets(AllSliders);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiColorAdjust::GetColor() const
{
    return this->color;
}
//------------------------------------------------------------------------------
#endif
