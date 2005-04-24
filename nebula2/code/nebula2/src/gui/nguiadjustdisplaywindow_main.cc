//------------------------------------------------------------------------------
//  nguiadjustdisplaywindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiadjustdisplaywindow.h"
#include "gui/nguihorislidergroup.h"
#include "gui/nguicolorslidergroup.h"
#include "scene/nmrtsceneserver.h"
#include "variable/nvariableserver.h"

nNebulaClass(nGuiAdjustDisplayWindow, "nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiAdjustDisplayWindow::nGuiAdjustDisplayWindow() :
    saturationHandle(nVariable::InvalidHandle),
    balanceHandle(nVariable::InvalidHandle),
    hdrBloomIntensityHandle(nVariable::InvalidHandle),
    hdrBrightPassThresholdHandle(nVariable::InvalidHandle),
    hdrBrightPassOffsetHandle(nVariable::InvalidHandle),
    resetSaturation(0.0f),
    resetBloomIntensity(0.0f),
    resetBrightPassThreshold(0.0f),
    resetBrightPassOffset(0.0f)
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
nGuiAdjustDisplayWindow::~nGuiAdjustDisplayWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiAdjustDisplayWindow::InitVariables()
{
    nVariableServer* varServer = nVariableServer::Instance();
    this->saturationHandle             = varServer->GetVariableHandleByName("Saturation");
    this->balanceHandle                = varServer->GetVariableHandleByName("Balance");
    this->hdrBloomIntensityHandle      = varServer->GetVariableHandleByName("HdrBloomScale");
    this->hdrBrightPassThresholdHandle = varServer->GetVariableHandleByName("HdrBrightPassThreshold");
    this->hdrBrightPassOffsetHandle    = varServer->GetVariableHandleByName("HdrBrightPassOffset");
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiAdjustDisplayWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    // initialize variables
    this->InitVariables();

    this->SetTitle("Display Adjustments");
    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // sliders...
    const float leftWidth = 0.3f;
    const float rightWidth = 0.1f;
    const float border = 0.005f;

    // saturation
    nGuiHoriSliderGroup* slider;
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "Saturate");
    slider->SetLeftText("Saturate");
    slider->SetRightText("%.2f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(5.0f);
    slider->SetValue(0.0f);
    slider->SetKnobSize(0.5f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachForm(slider, nGuiFormLayout::Top, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refSaturateSlider = slider;

    // color balance
    nGuiColorSliderGroup* colorSlider;
    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "Balance");
    colorSlider->SetLabelText("Balance");
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetMaxIntensity(5.0f);
    layout->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refSaturateSlider, 2 * border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    colorSlider->OnShow();
    this->refBalanceSlider = colorSlider;

    // hdr bloom intensity
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "HdrBloomIntensity");
    slider->SetLeftText("Bloom Intensity");
    slider->SetRightText("%.1f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(10.0f);
    slider->SetKnobSize(1.0f);
    slider->SetIncrement(0.1f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refBalanceSlider, 2 * border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refHdrBloomIntensitySlider = slider;

    // hdr bright pass threshold slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "HdrBrightPassThresholdSlider");
    slider->SetLeftText("Brightpass Threshold");
    slider->SetRightText("%.1f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(5.0f);
    slider->SetKnobSize(1.0f);
    slider->SetIncrement(0.1f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refHdrBloomIntensitySlider, 2 * border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refHdrBrightPassThresholdSlider = slider;

    // hdr bright pass threshold slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "HdrBrightPassOffsetSlider");
    slider->SetLeftText("Brightpass Offset");
    slider->SetRightText("%.1f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(5.0f);
    slider->SetKnobSize(1.0f);
    slider->SetIncrement(0.1f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refHdrBrightPassThresholdSlider, 2 * border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refHdrBrightPassOffsetSlider = slider;

    // reset button
    nGuiTextButton* btn = (nGuiTextButton*) kernelServer->New("nguitextbutton", "ResetButton");
    btn->SetText("Reset");
    btn->SetDefaultBrush("button_n");
    btn->SetPressedBrush("button_p");
    btn->SetHighlightBrush("button_h");
    btn->SetMinSize(buttonSize);
    btn->SetMaxSize(buttonSize);
    layout->AttachWidget(btn, nGuiFormLayout::Top, this->refHdrBrightPassOffsetSlider, 2 * border);
    layout->AttachForm(btn, nGuiFormLayout::Left, border);
    btn->OnShow();
    this->refResetButton = btn;

    kernelServer->PopCwd();
    this->UpdateSlidersFromValues();

    // set new window rect
    rectangle rect(vector2(0.0f, 0.0f), vector2(0.6f, 0.4f));
    this->SetRect(rect);

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiAdjustDisplayWindow::OnHide()
{
    this->refSaturateSlider->Release();
    this->refBalanceSlider->Release();
    this->refHdrBloomIntensitySlider->Release();
    this->refHdrBrightPassOffsetSlider->Release();
    this->refHdrBrightPassThresholdSlider->Release();
    this->refResetButton->Release();

    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiAdjustDisplayWindow::OnEvent(const nGuiEvent& event)
{
    if (this->refSaturateSlider.isvalid() &&
        this->refBalanceSlider.isvalid() &&
        this->refHdrBloomIntensitySlider.isvalid() &&
        this->refHdrBrightPassThresholdSlider.isvalid() &&
        this->refHdrBrightPassOffsetSlider.isvalid())
    {
        if ((event.GetType() == nGuiEvent::SliderChanged) &&
            (event.GetWidget() == this->refSaturateSlider) ||
            (event.GetWidget() == this->refBalanceSlider) ||
            (event.GetWidget() == this->refHdrBloomIntensitySlider) ||
            (event.GetWidget() == this->refHdrBrightPassThresholdSlider) ||
            (event.GetWidget() == this->refHdrBrightPassOffsetSlider))
        {
            this->UpdateValuesFromSliders();
        }
        else if ((event.GetType() == nGuiEvent::ButtonUp) &&
                (event.GetWidget() == this->refResetButton))
        {
            this->ResetValues();
        }
    }
    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    Update slider values from scene server.
*/
void
nGuiAdjustDisplayWindow::UpdateSlidersFromValues()
{
    nVariableServer* varServer = nVariableServer::Instance();
    float saturation = 1.0f;
    if (varServer->GlobalVariableExists(this->saturationHandle))
    {
        saturation = varServer->GetFloatVariable(this->saturationHandle);
        this->resetSaturation = saturation;
    }
    vector4 balance(1.0f, 1.0f, 1.0f, 1.0f);
    if (varServer->GlobalVariableExists(this->balanceHandle))
    {
        balance = varServer->GetVectorVariable(this->balanceHandle);
        this->resetBalance = balance;
    }
    float bloomIntensity = 1.0f;
    if (varServer->GlobalVariableExists(this->hdrBloomIntensityHandle))
    {
        bloomIntensity = varServer->GetFloatVariable(this->hdrBloomIntensityHandle);
        this->resetBloomIntensity = bloomIntensity;
    }
    float brightPassThreshold = 1.0f;
    if (varServer->GlobalVariableExists(this->hdrBrightPassThresholdHandle))
    {
        brightPassThreshold = varServer->GetFloatVariable(this->hdrBrightPassThresholdHandle);
        this->resetBrightPassThreshold = brightPassThreshold;
    }
    float brightPassOffset = 1.0f;
    if (varServer->GlobalVariableExists(this->hdrBrightPassOffsetHandle))
    {
        brightPassOffset = varServer->GetFloatVariable(this->hdrBrightPassOffsetHandle);
        this->resetBrightPassOffset = brightPassOffset;
    }

    this->refSaturateSlider->SetValue(saturation);
    this->refBalanceSlider->SetColor(balance);
    this->refHdrBloomIntensitySlider->SetValue(bloomIntensity);
    this->refHdrBrightPassThresholdSlider->SetValue(brightPassThreshold);
    this->refHdrBrightPassOffsetSlider->SetValue(brightPassOffset);
}

//------------------------------------------------------------------------------
/**
    Update scene server from slider values.
*/
void
nGuiAdjustDisplayWindow::UpdateValuesFromSliders()
{
    nVariableServer* varServer = nVariableServer::Instance();
    varServer->SetFloatVariable(this->saturationHandle, this->refSaturateSlider->GetValue());
    varServer->SetVectorVariable(this->balanceHandle, this->refBalanceSlider->GetColor());
    varServer->SetFloatVariable(this->hdrBloomIntensityHandle, this->refHdrBloomIntensitySlider->GetValue());
    varServer->SetFloatVariable(this->hdrBrightPassThresholdHandle, this->refHdrBrightPassThresholdSlider->GetValue());
    varServer->SetFloatVariable(this->hdrBrightPassOffsetHandle, this->refHdrBrightPassOffsetSlider->GetValue());
}

//------------------------------------------------------------------------------
/**
    Reset display adjustment values.
*/
void
nGuiAdjustDisplayWindow::ResetValues()
{
    nVariableServer* varServer = nVariableServer::Instance();
    varServer->SetFloatVariable(this->saturationHandle, this->resetSaturation);
    varServer->SetVectorVariable(this->balanceHandle, this->resetBalance);
    varServer->SetFloatVariable(this->hdrBloomIntensityHandle, this->resetBloomIntensity);
    varServer->SetFloatVariable(this->hdrBrightPassThresholdHandle, this->resetBrightPassThreshold);
    varServer->SetFloatVariable(this->hdrBrightPassOffsetHandle, this->resetBrightPassOffset);
    this->UpdateSlidersFromValues();
}
