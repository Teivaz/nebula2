//------------------------------------------------------------------------------
//  nguiadjustdisplaywindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiadjustdisplaywindow.h"
#include "gui/nguihorislidergroup.h"
#if 0
#include "scene/nmrtsceneserver.h"
#endif

nNebulaClass(nGuiAdjustDisplayWindow, "nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiAdjustDisplayWindow::nGuiAdjustDisplayWindow()
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
nGuiAdjustDisplayWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    this->SetTitle("Display Adjustments");
    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // sliders...
    const float leftWidth = 0.3f;
    const float rightWidth = 0.1f;
    const int maxVal = 768;
    const int knobSize = 50;
    const float border = 0.005f;

    nGuiHoriSliderGroup* slider;
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "Saturate");
    slider->SetLeftText("Saturate");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(0);
    slider->SetKnobSize(knobSize);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachForm(slider, nGuiFormLayout::Top, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refSaturate = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "BalanceRed");
    slider->SetLeftText("Balance (red)");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(0);
    slider->SetKnobSize(knobSize);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refSaturate, 2 * border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refBalanceRed = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "BalanceGreen");
    slider->SetLeftText("Balance (green)");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(0);
    slider->SetKnobSize(knobSize);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refBalanceRed, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refBalanceGreen = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "BalanceBlue");
    slider->SetLeftText("Balance (blue)");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(0);
    slider->SetKnobSize(knobSize);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refBalanceGreen, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refBalanceBlue = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "LuminanceRed");
    slider->SetLeftText("Luminance (red)");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(0);
    slider->SetKnobSize(knobSize);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refBalanceBlue, 2 * border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refLuminanceRed = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "LuminanceBlue");
    slider->SetLeftText("Luminance (blue)");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(0);
    slider->SetKnobSize(knobSize);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refLuminanceRed, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refLuminanceGreen = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "LuminanceGreen");
    slider->SetLeftText("Luminance (green)");
    slider->SetRightText("%d");
    slider->SetMinValue(0);
    slider->SetMaxValue(maxVal);
    slider->SetValue(0);
    slider->SetKnobSize(knobSize);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refLuminanceGreen, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refLuminanceBlue = slider;

    nGuiTextButton* btn = (nGuiTextButton*) kernelServer->New("nguitextbutton", "ResetButton");
    btn->SetText("Reset");
    btn->SetDefaultBrush("button_n");
    btn->SetPressedBrush("button_p");
    btn->SetHighlightBrush("button_h");
    btn->SetMinSize(buttonSize);
    btn->SetMaxSize(buttonSize);
    layout->AttachWidget(btn, nGuiFormLayout::Top, this->refLuminanceBlue, 2 * border);
    layout->AttachForm(btn, nGuiFormLayout::Left, border);
    btn->OnShow();
    this->refResetButton = btn;

    kernelServer->PopCwd();
    this->UpdateSlidersFromSceneServer();

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
    this->refSaturate->Release();
    this->refBalanceRed->Release();
    this->refBalanceGreen->Release();
    this->refBalanceBlue->Release();
    this->refLuminanceRed->Release();
    this->refLuminanceGreen->Release();
    this->refLuminanceBlue->Release();
    this->refResetButton->Release();

    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiAdjustDisplayWindow::OnEvent(const nGuiEvent& event)
{
    if (this->refSaturate.isvalid() &&
        this->refBalanceRed.isvalid() &&
        this->refBalanceGreen.isvalid() &&
        this->refBalanceBlue.isvalid() &&
        this->refLuminanceRed.isvalid() &&
        this->refBalanceGreen.isvalid() &&
        this->refBalanceBlue.isvalid() &&
        this->refResetButton.isvalid())
    {
        if ((event.GetType() == nGuiEvent::SliderChanged) &&
            (event.GetWidget() == this->refSaturate) ||
            (event.GetWidget() == this->refBalanceRed) ||
            (event.GetWidget() == this->refBalanceGreen) ||
            (event.GetWidget() == this->refBalanceBlue) ||
            (event.GetWidget() == this->refLuminanceRed) ||
            (event.GetWidget() == this->refLuminanceGreen) ||
            (event.GetWidget() == this->refLuminanceBlue))
        {
            this->UpdateSceneServerFromSliders();
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
nGuiAdjustDisplayWindow::UpdateSlidersFromSceneServer()
{
#if 0
    if (nSceneServer::Instance()->IsA(kernelServer->FindClass("nmrtsceneserver")))
    {
        nMRTSceneServer* sceneServer = (nMRTSceneServer*) nSceneServer::Instance();

        float sat = sceneServer->GetSaturation();
        const vector4& balance = sceneServer->GetBalance();
        const vector4& lum = sceneServer->GetLuminance();

        this->refSaturate->SetValue(int(sat * 255.0f));
        this->refBalanceRed->SetValue(int(balance.x * 255.0f));
        this->refBalanceGreen->SetValue(int(balance.y * 255.0f));
        this->refBalanceBlue->SetValue(int(balance.z * 255.0f));
        this->refLuminanceRed->SetValue(int(lum.x * 255.0f));
        this->refLuminanceGreen->SetValue(int(lum.y * 255.0f));
        this->refLuminanceBlue->SetValue(int(lum.z * 255.0f));
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Update scene server from slider values.
*/
void
nGuiAdjustDisplayWindow::UpdateSceneServerFromSliders()
{
#if 0
    if (nSceneServer::Instance()->IsA(kernelServer->FindClass("nmrtsceneserver")))
    {
        nMRTSceneServer* sceneServer = (nMRTSceneServer*) nSceneServer::Instance();
        
        float sat = this->refSaturate->GetValue() / 255.0f;
        vector4 balance;
        vector4 lum;
        balance.x = this->refBalanceRed->GetValue() / 255.0f;
        balance.y = this->refBalanceGreen->GetValue() / 255.0f;
        balance.z = this->refBalanceBlue->GetValue() / 255.0f;
        balance.w = 1.0f;
        lum.x = this->refLuminanceRed->GetValue() / 255.0f;
        lum.y = this->refLuminanceGreen->GetValue() / 255.0f;
        lum.z = this->refLuminanceBlue->GetValue() / 255.0f;
        lum.w = 1.0f;

        sceneServer->SetSaturation(sat);
        sceneServer->SetBalance(balance);
        sceneServer->SetLuminance(lum);
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Reset display adjustment values.
*/
void
nGuiAdjustDisplayWindow::ResetValues()
{
#if 0
    if (nSceneServer::Instance()->IsA(kernelServer->FindClass("nmrtsceneserver")))
    {
        nMRTSceneServer* sceneServer = (nMRTSceneServer*) nSceneServer::Instance();
        sceneServer->SetSaturation(1.0f);
        sceneServer->SetBalance(vector4(1.0f, 1.0f, 1.0f, 1.0f));
        sceneServer->SetLuminance(vector4(0.299f, 0.587f, 0.114f, 0.0f));
        this->UpdateSlidersFromSceneServer();
    }
#endif
}
