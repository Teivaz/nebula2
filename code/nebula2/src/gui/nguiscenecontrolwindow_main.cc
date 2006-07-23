//------------------------------------------------------------------------------
//  nguiscenecontrolwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiscenecontrolwindow.h"
#include "gui/nguihorislidergroup.h"
#include "gui/nguicolorslidergroup.h"
#include "scene/ntransformnode.h"
#include "scene/nlightnode.h"

nNebulaClass(nGuiSceneControlWindow, "gui::nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiSceneControlWindow::nGuiSceneControlWindow():
    diffuseColor(1.0f,1.0f,1.0f,1.0f),
    specularColor(1.0f,1.0f,1.0f,1.0f),
    ambientColor(1.0f,1.0f,1.0f,1.0f),
    lightPath("/usr/scene/default/stdlight/l"),
    lightTransformPath("/usr/scene/default/stdlight"),
    refLightTransform("/usr/scene/default/stdlight"),
    refLight("/usr/scene/default/stdlight/l")
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
nGuiSceneControlWindow::~nGuiSceneControlWindow()
{
    // make sure everything gets cleared
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSceneControlWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout;
    kernelServer->PushCwd(layout);

    // read current light values
    if (this->refLight.isvalid())
    {
        // look up the current Light Colors and update Brightness Sliders
        this->diffuseColor  = this->refLight->GetVector(nShaderState::LightDiffuse);
        this->specularColor = this->refLight->GetVector(nShaderState::LightSpecular);
        this->ambientColor  = this->refLight->GetVector(nShaderState::LightAmbient);
    }
    if (this->refLightTransform.isvalid())
    {
        this->lightAngles = this->refLightTransform->GetEuler();
    }

    // sliders and color labels ...
    const float leftWidth = 0.3f;
    const float rightWidth = 0.15f;
    const float maxAngle = 360;
    const float minHeight = -90;
    const float maxHeight = 90;
    const float border = 0.005f;
    const float knobSize = 45.0f;

    nGuiHoriSliderGroup* slider;
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "LightHori");
    slider->SetLeftText("Light Hori");
    slider->SetRightText("%d");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(maxAngle);
    slider->SetValue(n_rad2deg(this->lightAngles.y));
    slider->SetKnobSize(36);
    slider->SetIncrement(1.0f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachForm(slider, nGuiFormLayout::Top, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refLightDirection = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "LightVert");
    slider->SetLeftText("Light Vert");
    slider->SetRightText("%d");
    slider->SetMinValue(minHeight);
    slider->SetMaxValue(maxHeight);
    slider->SetValue(n_rad2deg(this->lightAngles.x));
    slider->SetKnobSize(20);
    slider->SetIncrement(1.0f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refLightDirection, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refLightHeight = slider;

    nGuiColorSliderGroup* colorSlider;
    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "Diffuse");
    colorSlider->SetLabelText("Diffuse");
    colorSlider->SetMaxIntensity(10.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(this->diffuseColor);
    layout->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refLightHeight, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    colorSlider->OnShow();
    this->refDiffuseSlider = colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "Specular");
    colorSlider->SetLabelText("Specular");
    colorSlider->SetMaxIntensity(10.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(this->specularColor);
    layout->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refDiffuseSlider, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    colorSlider->OnShow();
    this->refSpecularSlider = colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "Ambient");
    colorSlider->SetLabelText("Ambient");
    colorSlider->SetMaxIntensity(10.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(this->ambientColor);
    layout->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refSpecularSlider, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    colorSlider->OnShow();
    this->refAmbientSlider = colorSlider;

    // Create SkyEditor
    //nGuiSkyEditor* skyEditor = (nGuiSkyEditor*) kernelServer->New("nguiskyeditor","SkyEditor");
    //layout->AttachWidget(skyEditor, nGuiFormLayout::Top, this->refAmbientSlider, 2*border);
    //layout->AttachForm(skyEditor, nGuiFormLayout::Left, border);
    //layout->AttachForm(skyEditor, nGuiFormLayout::Right, border);
    //skyEditor->OnShow();
    //this->refSkyEditor = skyEditor;
    //if (this->refSkyEditor->SkyLoad())
    //{
    //    windowRect = rectangle(vector2(0.0f, 0.0f), vector2(0.4f, 0.8f));
    //}

    this->kernelServer->PopCwd();

    // set new window rect
    this->SetTitle("Scene Control");
    rectangle windowRect(vector2(0.0f, 0.0f), vector2(0.4f, 0.3f));
    this->SetRect(windowRect);

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSceneControlWindow::OnHide()
{
    this->refLightDirection->Release();
    this->refLightHeight->Release();
    this->refDiffuseSlider->Release();
    this->refSpecularSlider->Release();
    this->refAmbientSlider->Release();

    //if (this->refSkyEditor.isvalid())
    //{
    //    this->refSkyEditor->Release();
    //}

    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSceneControlWindow::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::SliderChanged)
    {
        // scene light menu
        if (this->refLightDirection.isvalid() &&
            this->refLightHeight.isvalid() &&
            this->refDiffuseSlider.isvalid() &&
            this->refSpecularSlider.isvalid() &&
            this->refAmbientSlider.isvalid())
        {
            if (event.GetWidget() == this->refLightDirection || event.GetWidget() == this->refLightHeight)
            {
                this->UpdateLightPosition();
            }
            else if (event.GetWidget() == this->refDiffuseSlider)
            {
                this->diffuseColor = this->refDiffuseSlider->GetColor();
                if (this->refLight.isvalid())
                {
                    this->refLight->SetVector(nShaderState::LightDiffuse, this->diffuseColor);
                }
            }
            else if (event.GetWidget() == this->refSpecularSlider)
            {
                this->specularColor = this->refSpecularSlider->GetColor();
                if (this->refLight.isvalid())
                {
                    this->refLight->SetVector(nShaderState::LightSpecular, this->specularColor);
                }
            }
            else if (event.GetWidget() == this->refAmbientSlider)
            {
                this->ambientColor = this->refAmbientSlider->GetColor();
                if (this->refLight.isvalid())
                {
                    this->refLight->SetVector(nShaderState::LightAmbient, this->ambientColor);
                }
            }
        }
    }

    //if (this->refSkyEditor.isvalid())
    //{
    //    this->refSkyEditor->OnEvent(event);
    //}

    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    Update Light Rotation and Height.
*/
void
nGuiSceneControlWindow::UpdateLightPosition()
{
    if (this->refLightTransform.isvalid())
    {
        this->lightAngles = vector3(n_deg2rad(this->refLightHeight->GetValue()), n_deg2rad(this->refLightDirection->GetValue()), 0.0f);
        this->refLightTransform->SetEuler(this->lightAngles);
    }
}
