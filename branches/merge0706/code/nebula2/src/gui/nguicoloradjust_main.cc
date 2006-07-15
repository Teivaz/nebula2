//------------------------------------------------------------------------------
//  nguicoloradjust_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicoloradjust.h"
#include "gui/nguihorislidergroup.h"
#include "gui/nguitextureview.h"
#include "gui/nguicolorlabel.h"
#include "gui/nguicolorpicker.h"

nNebulaClass(nGuiColorAdjust, "nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiColorAdjust::nGuiColorAdjust():
    brightness(1.0f),
    color(1.0, 1.0, 1.0, 1.0),
    noManualBrightnessChange(false),
    noManualRedChange(false),
    noManualGreenChange(false),
    noManualBlueChange(false)
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
nGuiColorAdjust::~nGuiColorAdjust()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorAdjust::OnShow()
{
    // call parent class
    nGuiFormLayout::OnShow();

    // get client area form layout object
    kernelServer->PushCwd(this);

    // Init Widgets...
    // Adjust Widgets to Display Resolution
    vector2 displaySize = nGuiServer::Instance()->GetDisplaySize();
    float xFactor = 640 / displaySize.x;
    float yFactor = 480 / displaySize.y;
    // Because the fontsize varies in different resolutions, a factor is needed to adjust the needed space
    float xFactorText = (((displaySize.x/640)-1.0f)/4)+1.0f;
    
    const float xBorder = 0.01f * xFactor;
    const float yBorder = 0.01f * yFactor;
    const float leftWidth = 0.25f * xFactorText;
    const float rightWidth = 0.12f * xFactorText;
    const float maxVal = 1.0f;
    const float knobSize = 0.1f;
    const float increment = 0.01f;
    static const vector2 currentColorSize(0.06f * xFactor , 0.292f * yFactor);
    static const vector4 white(1.0f, 1.0f, 1.0f, 1.0f);
    nGuiHoriSliderGroup* slider;
      
    nGuiColorPicker* colorpicker = (nGuiColorPicker*) kernelServer->New("nguicolorpicker", "ColorPicker");
    colorpicker->SetIntensity(this->brightness);
    this->AttachForm(colorpicker, nGuiFormLayout::Top, 2 * yBorder);
    this->AttachForm(colorpicker, nGuiFormLayout::Right, 2.85f * xBorder);
    colorpicker->OnShow();
    this->refColorPicker = colorpicker;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "BrignessSlider");
    slider->SetLeftText("Brightness");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(maxVal);
    slider->SetKnobSize(knobSize);
    slider->SetIncrement(increment);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    this->AttachWidget(slider, nGuiFormLayout::Top, this->refColorPicker, 3 * yBorder);
    this->AttachForm(slider, nGuiFormLayout::Left, xBorder);
    this->AttachForm(slider, nGuiFormLayout::Right, xBorder);
    slider->OnShow();
    this->refBrightnessSlider = slider;

    nGuiColorLabel* activeColor = (nGuiColorLabel*) kernelServer->New("nguicolorlabel", "ActiveColor");
    activeColor->SetColor(this->color);
    activeColor->SetMinSize(currentColorSize);
    activeColor->SetMaxSize(currentColorSize);
    activeColor->SetRect(rectangle(vector2(0.0f, 0.0f), currentColorSize));
    this->AttachWidget(activeColor, nGuiFormLayout::Right, this->refColorPicker, 0.5f * xBorder);
    this->AttachForm(activeColor, nGuiFormLayout::Left, xBorder);
    this->AttachForm(activeColor, nGuiFormLayout::Top, 2.75f * yBorder);
    activeColor->OnShow();
    this->refCurrentColor = activeColor;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "RedSlider");
    slider->SetLeftText("Red");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(maxVal);
    slider->SetKnobSize(knobSize);
    slider->SetIncrement(increment);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    this->AttachWidget(slider, nGuiFormLayout::Top, this->refBrightnessSlider, 2.55f * yBorder);
    this->AttachForm(slider, nGuiFormLayout::Left, xBorder);
    this->AttachForm(slider, nGuiFormLayout::Right, xBorder);
    slider->OnShow();
    this->refRedValueSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "GreenSlider");
    slider->SetLeftText("Green");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(maxVal);
    slider->SetKnobSize(knobSize);
    slider->SetIncrement(increment);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    this->AttachWidget(slider, nGuiFormLayout::Top, this->refRedValueSlider, 1.5f * yBorder);
    this->AttachForm(slider, nGuiFormLayout::Left, xBorder);
    this->AttachForm(slider, nGuiFormLayout::Right, xBorder);
    slider->OnShow();
    this->refGreenValueSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "BlueSlider");
    slider->SetLeftText("Blue");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(maxVal);
    slider->SetKnobSize(knobSize);
    slider->SetIncrement(increment);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    this->AttachWidget(slider, nGuiFormLayout::Top, this->refGreenValueSlider, 1.5f * yBorder);
    this->AttachForm(slider, nGuiFormLayout::Left, xBorder);
    this->AttachForm(slider, nGuiFormLayout::Right, xBorder);
    slider->OnShow();
    this->refBlueValueSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "AlphaSlider");
    slider->SetLeftText("Alpha");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(maxVal);
    slider->SetKnobSize(knobSize);
    slider->SetIncrement(increment);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    this->AttachWidget(slider, nGuiFormLayout::Top, this->refBlueValueSlider, 1.5f * yBorder);
    this->AttachForm(slider, nGuiFormLayout::Left, xBorder);
    this->AttachForm(slider, nGuiFormLayout::Right, xBorder);
    slider->OnShow();
    this->refAlphaValueSlider = slider;

    kernelServer->PopCwd();

    // update all layouts
    this->UpdateLayout(this->rect);

    nGuiServer::Instance()->RegisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorAdjust::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);

    this->refAlphaValueSlider->Release();
    this->refBlueValueSlider->Release();
    this->refBrightnessSlider->Release();
    this->refCurrentColor->Release();
    this->refGreenValueSlider->Release();
    this->refRedValueSlider->Release();
    this->refColorPicker->Release();
   
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
    When a color changed, the brightness has to be adjusted
*/
void
nGuiColorAdjust::UpdateBrightness()
{
    // Adjust the brightness to the highest color value
    float biggestColor = (color.x>color.y)? color.x : color.y;
    this->brightness = (color.z>biggestColor)? color.z : biggestColor;

    // set dirtyflags
    this->noManualBrightnessChange = true;
    this->noManualRedChange = false; 
    this->noManualGreenChange = false;
    this->noManualBlueChange = false;
}

//------------------------------------------------------------------------------
/**
    When the brighness changed, the colors have to be adjusted
*/
void
nGuiColorAdjust::UpdateColor()
{
    static const float riseColor = 1.0f/255.0f;
    float normFactor;
    // detect highest color value
    float biggestColor = (this->color.x>color.y)? this->color.x : this->color.y;
    biggestColor = (this->color.z>biggestColor)? this->color.z : biggestColor;

    // if the color values are 0, they have to be risen in order to get values
    // higher than 0 when multiplied with the brightness.
    if (biggestColor == 0.0f)
    {
        if (brightness > riseColor)
        {
            biggestColor = riseColor;
            this->color.x = biggestColor;
            this->color.y = biggestColor;
            this->color.z = biggestColor;
            normFactor = 1.0f/biggestColor;
        }
        else
        {
            normFactor = 0.0f;
        }
    }
    else normFactor = 1.0f/biggestColor;
    // the color values have to be normed when beeing multiplied with the brightness
    this->color.set(this->color.x * (this->brightness) * normFactor, 
                    this->color.y * (this->brightness) * normFactor, 
                    this->color.z * (this->brightness) * normFactor, 
                    this->color.w );
    
    // set dirtyflags
    this->noManualRedChange = true; 
    this->noManualGreenChange = true;
    this->noManualBlueChange = true;
    this->noManualBrightnessChange = false;
}

//------------------------------------------------------------------------------
/**
    Updates the sliders, colorpicker and current color
*/
void
nGuiColorAdjust::UpdateWidgets(int sliderFlags)
{
    char rightText[128];

    // update colorpicker
    this->refColorPicker->SetIntensity(this->brightness);
    this->refColorPicker->SetAlpha(this->color.w);

    // update current color label
    this->refCurrentColor->SetColor(this->color);

    // update brightness slider
    if (sliderFlags & Brightness)
    {
        this->refBrightnessSlider->SetValue(this->brightness);
    }
    snprintf(rightText, sizeof(rightText), "%.2f", this->brightness);
    this->refBrightnessSlider->SetRightText(rightText);

    // update red slider
    if (sliderFlags & Red)
    {
        this->refRedValueSlider->SetValue(this->color.x);
    }
    snprintf(rightText, sizeof(rightText), "%.2f", this->color.x);
    this->refRedValueSlider->SetRightText(rightText);

    // update green slider
    if (sliderFlags & Green)
    {
        this->refGreenValueSlider->SetValue(this->color.y);
    }
    snprintf(rightText, sizeof(rightText), "%.2f", this->color.y);
    this->refGreenValueSlider->SetRightText(rightText);

    // update blue slider
    if (sliderFlags & Blue)
    {
        this->refBlueValueSlider->SetValue(this->color.z);
    }
    snprintf(rightText, sizeof(rightText), "%.2f", this->color.z);
    this->refBlueValueSlider->SetRightText(rightText);

    // update alpha slider
    if (sliderFlags & Alpha)
    {
        this->refAlphaValueSlider->SetValue(this->color.w);
    }
    snprintf(rightText, sizeof(rightText), "%.2f", this->color.w);
    this->refAlphaValueSlider->SetRightText(rightText);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorAdjust::OnEvent(const nGuiEvent& event)
{
    bool colorChanged = false;
    if (event.GetType() == nGuiEvent::SliderChanged)
    {
        float newValue;
        if ((this->refColorPicker.isvalid()) && (event.GetWidget() == this->refColorPicker))
        {
            this->color.set(this->refColorPicker->GetColor());
            this->UpdateWidgets(AllSliders);
            colorChanged = true;
        }
        else if ((this->refBrightnessSlider.isvalid()) && (event.GetWidget() == this->refBrightnessSlider))
        {
            if (this->noManualBrightnessChange) 
            {
                this->noManualBrightnessChange = false;
            }
            else
            {
                newValue = this->refBrightnessSlider->GetValue();
                if (newValue != this->brightness)
                {
                    this->brightness = newValue;
                    this->UpdateColor();
                    this->UpdateWidgets(Red | Green | Blue | Alpha);
                    colorChanged = true;
                }
            }
        }
        else if ((this->refRedValueSlider.isvalid()) && (event.GetWidget() == this->refRedValueSlider))
        {
            if (this->noManualRedChange) 
            {
                this->noManualRedChange = false;
            }
            else
            {
                newValue = this->refRedValueSlider->GetValue();
                if (newValue != this->color.x)
                {
                    this->color.x = newValue;
                    this->UpdateBrightness();
                    this->UpdateWidgets(Brightness | Green | Blue | Alpha);
                    colorChanged = true;
                }
            }
        }
        else if ((this->refGreenValueSlider.isvalid()) && (event.GetWidget() == this->refGreenValueSlider))
        {
            if (this->noManualGreenChange) 
            {
                this->noManualGreenChange = false;
            }
            else
            {
                newValue = this->refGreenValueSlider->GetValue();
                if (newValue != this->color.y)
                {
                    this->color.y = newValue;
                    this->UpdateBrightness();
                    this->UpdateWidgets(Brightness | Red | Blue | Alpha);
                    colorChanged = true;
                }
            }
        }
        else if ((this->refBlueValueSlider.isvalid()) && (event.GetWidget() == this->refBlueValueSlider))
        {
            if (this->noManualBlueChange) 
            {
                this->noManualBlueChange = false;
            }
            else
            {
                newValue = this->refBlueValueSlider->GetValue();
                if (newValue != this->color.z)
                {
                    this->color.z = newValue;
                    this->UpdateBrightness();
                    this->UpdateWidgets(Brightness | Red | Green | Alpha);
                    colorChanged = true;
                }
            }
        }
        else if ((this->refAlphaValueSlider.isvalid()) && (event.GetWidget() == this->refAlphaValueSlider))
        {
            this->color.w = this->refAlphaValueSlider->GetValue();
            this->UpdateWidgets(Brightness | Red | Green | Blue);
            colorChanged = true;
        }
    }

    nGuiFormLayout::OnEvent(event);

    if (colorChanged)
    {
        nGuiEvent newEvent(this, nGuiEvent::SliderChanged);
        nGuiServer::Instance()->PutEvent(newEvent);
    }
}
