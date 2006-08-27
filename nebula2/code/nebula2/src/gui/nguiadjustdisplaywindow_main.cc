//------------------------------------------------------------------------------
//  nguiadjustdisplaywindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiadjustdisplaywindow.h"
#include "gui/nguihorislidergroup.h"
#include "gui/nguicolorslidergroup.h"
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
    fogDistancesHandle(nVariable::InvalidHandle),
    fogColorHandle(nVariable::InvalidHandle),
    focusHandle(nVariable::InvalidHandle),
    noiseIntensityHandle(nVariable::InvalidHandle),
    noiseScaleHandle(nVariable::InvalidHandle),
    noiseFrequencyHandle(nVariable::InvalidHandle),
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
    this->fogDistancesHandle           = varServer->GetVariableHandleByName("FogDistances");
    this->fogColorHandle               = varServer->GetVariableHandleByName("FogColor");
    this->focusHandle                  = varServer->GetVariableHandleByName("CameraFocus");
    this->noiseIntensityHandle         = varServer->GetVariableHandleByName("NoiseIntensity");
    this->noiseScaleHandle             = varServer->GetVariableHandleByName("NoiseScale");
    this->noiseFrequencyHandle         = varServer->GetVariableHandleByName("NoiseFrequency");
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
    layout->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refSaturateSlider, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    colorSlider->OnShow();
    this->refBalanceSlider = colorSlider;

    // hdr bloom intensity
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "HdrBloomIntensity");
    slider->SetLeftText("HDR Intensity");
    slider->SetRightText("%.1f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(10.0f);
    slider->SetKnobSize(1.0f);
    slider->SetIncrement(0.1f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refBalanceSlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refHdrBloomIntensitySlider = slider;

    // hdr bright pass threshold slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "HdrBrightPassThresholdSlider");
    slider->SetLeftText("HDR Threshold");
    slider->SetRightText("%.1f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(5.0f);
    slider->SetKnobSize(1.0f);
    slider->SetIncrement(0.1f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refHdrBloomIntensitySlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refHdrBrightPassThresholdSlider = slider;

    // hdr bright pass threshold slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "HdrBrightPassOffsetSlider");
    slider->SetLeftText("HDR Offset");
    slider->SetRightText("%.1f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(5.0f);
    slider->SetKnobSize(1.0f);
    slider->SetIncrement(0.1f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refHdrBrightPassThresholdSlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refHdrBrightPassOffsetSlider = slider;

    // fog color slider
    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "FogColor");
    colorSlider->SetLabelText("Fog Color");
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetMaxIntensity(1.0f);
    layout->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refHdrBrightPassOffsetSlider, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    colorSlider->OnShow();
    this->refFogColorSlider = colorSlider;

    // fog near plane slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "FogNearSlider");
    slider->SetLeftText("Fog Near");
    slider->SetRightText("%.1f");
    slider->SetMinValue(0.1f);
    slider->SetMaxValue(1000.0f);
    slider->SetKnobSize(100.0f);
    slider->SetIncrement(0.1f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refFogColorSlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refFogNearSlider = slider;

    // fog far plane slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "FogFarSlider");
    slider->SetLeftText("Fog Far");
    slider->SetRightText("%.1f");
    slider->SetMinValue(0.1f);
    slider->SetMaxValue(1000.0f);
    slider->SetKnobSize(100.0f);
    slider->SetIncrement(0.1f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refFogNearSlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refFogFarSlider = slider;

    // camera focus distance slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "FocusDistSlider");
    slider->SetLeftText("Focus Dist");
    slider->SetRightText("%.2f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(100.0f);
    slider->SetKnobSize(10.0f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refFogFarSlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refFocusDistSlider = slider;

    // camera focus length slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "FocusLengthSlider");
    slider->SetLeftText("Focus Length");
    slider->SetRightText("%.2f");
    slider->SetMinValue(0.01f);
    slider->SetMaxValue(1000.0f);
    slider->SetKnobSize(100.0f);
    slider->SetIncrement(1.0f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refFocusDistSlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refFocusLengthSlider = slider;

    // noise intensity slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "NoiseIntensitySlider");
    slider->SetLeftText("Noise Intensity");
    slider->SetRightText("%.2f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(1.0f);
    slider->SetKnobSize(0.1f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refFocusLengthSlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refNoiseIntensitySlider = slider;

    // noise scale slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "NoiseScaleSlider");
    slider->SetLeftText("Noise Scale");
    slider->SetRightText("%.2f");
    slider->SetMinValue(1.0f);
    slider->SetMaxValue(100.0f);
    slider->SetKnobSize(10.0f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refNoiseIntensitySlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refNoiseScaleSlider = slider;

    // noise frequency slider
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "NoiseFreqSlider");
    slider->SetLeftText("Noise Freq");
    slider->SetRightText("%.2f");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(100.0f);
    slider->SetKnobSize(10.0f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refNoiseScaleSlider, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refNoiseFrequencySlider = slider;

    kernelServer->PopCwd();
    this->UpdateSlidersFromValues();

    // set new window rect
    float vSize = 15 * (border + this->refSaturateSlider->GetMinSize().y);
    rectangle rect(vector2(0.0f, 0.0f), vector2(0.5f, vSize));
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
    if(this->refSaturateSlider.isvalid())
    {
        this->refSaturateSlider->Release();
    }
    if(this->refBalanceSlider.isvalid())
    {
        this->refBalanceSlider->Release();
    }
    if(this->refHdrBloomIntensitySlider.isvalid())
    {
         this->refHdrBloomIntensitySlider->Release();
    }
    if(this->refHdrBrightPassOffsetSlider.isvalid())
    {
        this->refHdrBrightPassOffsetSlider->Release();
    }
    if(this->refHdrBrightPassThresholdSlider.isvalid())
    {
        this->refHdrBrightPassThresholdSlider->Release();
    }
    if(this->refFogColorSlider.isvalid())
    {
        this->refFogColorSlider->Release();
    }
    if(this->refFogNearSlider.isvalid())
    {
        this->refFogNearSlider->Release();
    }
    if(this->refFogFarSlider.isvalid())
    {
        this->refFogFarSlider->Release();
    }
    if(this->refFocusDistSlider.isvalid())
    {
        this->refFocusDistSlider->Release();
    }
    if(this->refFocusLengthSlider.isvalid())
    {
        this->refFocusLengthSlider->Release();
    }
    if(this->refNoiseIntensitySlider.isvalid())
    {
        this->refNoiseIntensitySlider->Release();
    }
    if(this->refNoiseScaleSlider.isvalid())
    {
        this->refNoiseScaleSlider->Release();
    }
    if(this->refNoiseFrequencySlider.isvalid())
    {
        this->refNoiseFrequencySlider->Release();
    }
/*    if(this->refResetButton.isvalid())
    {
        this->refResetButton->Release();
    }
    if(this->refSaveButton.isvalid())
    {
        this->refSaveButton->Release();
    }
    if(this->refLoadButton.isvalid())
    {
        this->refLoadButton->Release();
    }
    if(this->refMessageBox.isvalid())
    {
        this->refMessageBox->Release();
    }
    if(this->refBrowseButton.isvalid())
    {
        this->refBrowseButton->Release();
    }
    if(this->refFilenameEntry.isvalid())
    {
        this->refFilenameEntry->Release();
    }
    if(this->refSetDefaultButton.isvalid())
    {
        this->refSetDefaultButton->Release();
    }*/
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
        this->refHdrBrightPassOffsetSlider.isvalid() &&
        this->refFogColorSlider.isvalid() &&
        this->refFogNearSlider.isvalid() &&
        this->refFogFarSlider.isvalid() &&
        this->refFocusDistSlider.isvalid() &&
        this->refFocusLengthSlider.isvalid() &&
        this->refNoiseIntensitySlider.isvalid() &&
        this->refNoiseScaleSlider.isvalid() &&
        this->refNoiseFrequencySlider.isvalid())
    {
        if ((event.GetType() == nGuiEvent::SliderChanged) &&
            (event.GetWidget() == this->refSaturateSlider) ||
            (event.GetWidget() == this->refBalanceSlider) ||
            (event.GetWidget() == this->refHdrBloomIntensitySlider) ||
            (event.GetWidget() == this->refHdrBrightPassThresholdSlider) ||
            (event.GetWidget() == this->refHdrBrightPassOffsetSlider) ||
            (event.GetWidget() == this->refFogColorSlider) ||
            (event.GetWidget() == this->refFogNearSlider) ||
            (event.GetWidget() == this->refFogFarSlider) ||
            (event.GetWidget() == this->refFocusDistSlider) ||
            (event.GetWidget() == this->refFocusLengthSlider) ||
            (event.GetWidget() == this->refNoiseIntensitySlider) ||
            (event.GetWidget() == this->refNoiseScaleSlider) ||
            (event.GetWidget() == this->refNoiseFrequencySlider))
        {
            this->UpdateValuesFromSliders();
        }
/*        else if ((event.GetType() == nGuiEvent::ButtonUp) &&
                (event.GetWidget() == this->refResetButton))
        {
            this->ResetValues();
        }
        else if ((event.GetType() == nGuiEvent::ButtonUp) &&
                (event.GetWidget() == this->refBrowseButton))
        {
            this->CreateFileDialog();
        }
        else if ((event.GetType() == nGuiEvent::ButtonUp) &&
                (event.GetWidget() == this->refLoadButton))
        {
            if(!strlen(refFilenameEntry->GetText()) == 0)
            {
                this->LoadValuesFromFile(refFilenameEntry->GetText());
            }
        }
        else if ((event.GetType() == nGuiEvent::ButtonUp) &&
                (event.GetWidget() == this->refSaveButton))
        {
            if(!strlen(refFilenameEntry->GetText()) == 0)
            {
                this->SaveValuesToFile(refFilenameEntry->GetText());
            }
        }
        else if ((event.GetType() == nGuiEvent::ButtonUp) &&
                (event.GetWidget() == this->refSetDefaultButton))
        {
            this->SaveValuesAsDefault();
        }*/
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
    vector4 fogColor;
    if (varServer->GlobalVariableExists(this->fogColorHandle))
    {
        fogColor = varServer->GetVectorVariable(this->fogColorHandle);
        this->resetFogColor = fogColor;
    }
    vector4 fogDistances;
    if (varServer->GlobalVariableExists(this->fogDistancesHandle))
    {
        fogDistances = varServer->GetVectorVariable(this->fogDistancesHandle);
        this->resetFogDistances = fogDistances;
    }
    vector4 cameraFocus;
    if (varServer->GlobalVariableExists(this->focusHandle))
    {
        cameraFocus = varServer->GetVectorVariable(this->focusHandle);
        this->resetFocus = cameraFocus;
    }
    float noiseIntensity = 0.0f;
    if (varServer->GlobalVariableExists(this->noiseIntensityHandle))
    {
        noiseIntensity = varServer->GetFloatVariable(this->noiseIntensityHandle);
        this->resetNoiseIntensity = noiseIntensity;
    }
    float noiseScale = 100.0f;
    if (varServer->GlobalVariableExists(this->noiseScaleHandle))
    {
        noiseScale = varServer->GetFloatVariable(this->noiseScaleHandle);
        this->resetNoiseScale = noiseScale;
    }
    float noiseFrequency = 100.0f;
    if (varServer->GlobalVariableExists(this->noiseFrequencyHandle))
    {
        noiseFrequency = varServer->GetFloatVariable(this->noiseFrequencyHandle);
        this->resetNoiseFrequency = noiseFrequency;
    }

    this->refSaturateSlider->SetValue(saturation);
    this->refBalanceSlider->SetColor(balance);
    this->refHdrBloomIntensitySlider->SetValue(bloomIntensity);
    this->refHdrBrightPassThresholdSlider->SetValue(brightPassThreshold);
    this->refHdrBrightPassOffsetSlider->SetValue(brightPassOffset);
    this->refFogColorSlider->SetColor(fogColor);
    this->refFogNearSlider->SetValue(fogDistances.x);
    this->refFogFarSlider->SetValue(fogDistances.y);
    this->refFocusDistSlider->SetValue(cameraFocus.x);
    this->refFocusLengthSlider->SetValue(cameraFocus.y);
    this->refNoiseIntensitySlider->SetValue(noiseIntensity);
    this->refNoiseScaleSlider->SetValue(noiseScale);
    this->refNoiseFrequencySlider->SetValue(noiseFrequency);
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
    varServer->SetVectorVariable(this->fogColorHandle, this->refFogColorSlider->GetColor());
    varServer->SetVectorVariable(this->fogDistancesHandle, vector4(this->refFogNearSlider->GetValue(), this->refFogFarSlider->GetValue(), 0.0f, 0.0f));
    varServer->SetVectorVariable(this->focusHandle, vector4(this->refFocusDistSlider->GetValue(), this->refFocusLengthSlider->GetValue(), 0.0f, 0.0f));
    varServer->SetFloatVariable(this->noiseIntensityHandle, this->refNoiseIntensitySlider->GetValue());
    varServer->SetFloatVariable(this->noiseScaleHandle, this->refNoiseScaleSlider->GetValue());
    varServer->SetFloatVariable(this->noiseFrequencyHandle, this->refNoiseFrequencySlider->GetValue());
}

