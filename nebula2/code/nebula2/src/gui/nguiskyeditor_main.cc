//------------------------------------------------------------------------------
//  nguiskyeditor_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguihorislidergroup.h"
#include "scene/ntransformnode.h"
#include "scene/nlightnode.h"
#include "gui/nguitextlabel.h"
#include "gui/nguicolorslidergroup.h"
#include "scene/nskinanimator.h"
#include "gui/nguitextlabel.h"
#include "gui/nguitextview.h"
#include "anim2/nanimstateinfo.h"
#include "variable/nvariable.h"
#include "variable/nvariableserver.h"

#include "gui/nguiskyeditor.h"

#include "scene/nskynode.h"
#include "scene/nshapenode.h"
#include "gui/nguicheckbutton.h"
#include "kernel/ntimeserver.h"

nNebulaClass(nGuiSkyEditor, "nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiSkyEditor::nGuiSkyEditor():
    sliderChanged(false),   // ???
    skyPath("/usr/scene"),
    refSky("/usr/scene"),
    layoutChanged(false),
    elementReady(true),
    activeElement(-1),
    oldElement(-1),
    activeState(-1),
    stateReady(true),
    refresh(false),
    saveSky(false),
    updateSkyTime(true),
    activeType(nSkyNode::InvalidElement)
{
    this->FindSkyNode(refSky);
}


//------------------------------------------------------------------------------
/**
*/
nGuiSkyEditor::~nGuiSkyEditor()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::OnShow()
{
    // call parent class
    nGuiFormLayout::OnShow();

    this->ShowSky();

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::OnHide()
{
    this->HideSky();
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::OnEvent(const nGuiEvent& event)
{
    this->EventSky(event);

    nGuiFormLayout::OnEvent(event);
}


//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::ShowSky()
{
    const float leftWidth = 0.3f;
    const float rightWidth = 0.15f;
    const float maxAngle = 360.0f;
    const float minHeight = -90.0f;
    const float maxHeight = 90.0f;
    const float border = 0.005f;
    const float sliderOffset = 0.025f;

    kernelServer->PushCwd(this);

    nGuiHoriSliderGroup* slider;

    if (this->refSky.isvalid())
    {
        vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");
        nGuiTextButton* button = (nGuiTextButton*) kernelServer->New("nguitextbutton", "refreshButton");
        n_assert(button);
        button->SetText("Refresh");
        button->SetFont("GuiSmall");
        button->SetAlignment(nGuiTextButton::Center);
        button->SetDefaultBrush("button_n");
        button->SetPressedBrush("button_p");
        button->SetHighlightBrush("button_h");
        button->SetMinSize(buttonSize);
        button->SetMaxSize(buttonSize);
        button->SetColor(vector4(0,0,0,1));
        this->AttachForm(button, nGuiFormLayout::Top, 3 * border);
        this->AttachForm(button, nGuiFormLayout::Right, 0.005f);
        button->OnShow();
        this->refRefreshButton = button;

        button = (nGuiTextButton*) kernelServer->New("nguitextbutton", "saveButton");
        n_assert(button);
        button->SetText("Save");
        button->SetFont("GuiSmall");
        button->SetAlignment(nGuiTextButton::Center);
        button->SetDefaultBrush("button_n");
        button->SetPressedBrush("button_p");
        button->SetHighlightBrush("button_h");
        button->SetMinSize(buttonSize);
        button->SetMaxSize(buttonSize);
        button->SetColor(vector4(0,0,0,1));
        this->AttachWidget(button, nGuiFormLayout::Right, this->refRefreshButton, 0.005f);
        this->AttachForm(button, nGuiFormLayout::Top, 3 * border);
        button->OnShow();
        this->refSaveButton = button;

        slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkyStartTimeSlider");
        slider->SetLeftText("Skytime start");
        slider->SetRightText("%f");
        slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
        slider->SetMinValue(0.0f);
        slider->SetMaxValue(this->refSky->GetTimePeriod());
        slider->SetValue(this->refSky->GetSkyTime());
        slider->SetKnobSize(this->refSky->GetTimePeriod()/10);
        slider->SetIncrement(1.0f);
        slider->SetLeftWidth(leftWidth);
        slider->SetRightWidth(rightWidth);
        this->AttachWidget(slider, nGuiFormLayout::Top,this->refSaveButton, 3 * border);  // Animation pr?en
        this->AttachForm(slider, nGuiFormLayout::Left, border);
        this->AttachForm(slider, nGuiFormLayout::Right, border);
        slider->OnShow();
        this->refSkyTimeSlider = slider;

        slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkyTimeFactorSlider");
        slider->SetLeftText("Skytime Factor");
        slider->SetRightText("%f");
        slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
        slider->SetMinValue(-1000.0f);
        slider->SetMaxValue(1000.0f);
        slider->SetValue(this->refSky->GetTimeFactor());
        slider->SetKnobSize(200);
        slider->SetKnobSize(100.0f);
        slider->SetIncrement(1.0f);
        slider->SetLeftWidth(leftWidth);
        slider->SetRightWidth(rightWidth);
        this->AttachWidget(slider, nGuiFormLayout::Top,this->refSkyTimeSlider, border);
        this->AttachForm(slider, nGuiFormLayout::Left, border);
        this->AttachForm(slider, nGuiFormLayout::Right, border);
        slider->OnShow();
        this->refTimeFactorSlider = slider;

        slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkyElementSlider");
        slider->SetLeftText("Sky Element");
        slider->SetRightText("%d");
        slider->SetMinValue(0);
        slider->SetMaxValue((float)this->refSky->GetNumElements());
        slider->SetValue(0);
        slider->SetKnobSize(1);
        slider->SetIncrement(1);
        slider->SetLeftWidth(leftWidth);
        slider->SetRightWidth(rightWidth);
        this->AttachWidget(slider, nGuiFormLayout::Top,this->refTimeFactorSlider, border);
        this->AttachForm(slider, nGuiFormLayout::Left, border);
        this->AttachForm(slider, nGuiFormLayout::Right, border);
        slider->OnShow();
        this->refElementSlider = slider;

        slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkyStateSlider");
        slider->SetLeftText("Sky States");
        slider->SetRightText("%d");
        slider->SetMinValue(0);
        slider->SetMaxValue(0);
        slider->SetValue(0);
        slider->SetKnobSize(1);
        slider->SetIncrement(1);
        slider->SetLeftWidth(leftWidth);
        slider->SetRightWidth(rightWidth);
        this->AttachWidget(slider, nGuiFormLayout::Top,this->refElementSlider, border);
        this->AttachForm(slider, nGuiFormLayout::Left, border);
        this->AttachForm(slider, nGuiFormLayout::Right, border);
        slider->OnShow();
        this->refStateSlider = slider;

        slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkyStateTimeSlider");
        slider->SetLeftText("StateTime");
        slider->SetRightText("%f");
        slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
        slider->SetMinValue(0);
        slider->SetMaxValue(0);
        slider->SetValue(1);
        slider->SetKnobSize(1);
        slider->SetIncrement(1);
        slider->SetLeftWidth(leftWidth);
        slider->SetRightWidth(rightWidth);
        this->AttachWidget(slider, nGuiFormLayout::Top,this->refStateSlider, border);
        this->AttachForm(slider, nGuiFormLayout::Left, border);
        this->AttachForm(slider, nGuiFormLayout::Right, border);
        slider->OnShow();
        this->refStateTimeSlider = slider;

        nGuiTextLabel* textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "nSkyElementLabel");
        n_assert(textLabel);
        textLabel->SetText("Element");
        textLabel->SetFont("GuiSmall");
        textLabel->SetAlignment(nGuiTextLabel::Right);
        vector2 textSize = textLabel->GetTextExtent();
        vector2 textMinSize(0.0f, textSize.y);
        vector2 textMaxSize(1.0f, textSize.y);
        textLabel->SetColor(vector4(1.0f, 1.0f, 0.0f, 1.0f));
        textLabel->SetMinSize(textMinSize);
        textLabel->SetMaxSize(textMaxSize);
        this->AttachWidget(textLabel, nGuiFormLayout::Top, this->refStateTimeSlider, border);
        this->AttachForm(textLabel, nGuiFormLayout::Left, border);
        this->AttachPos(textLabel, nGuiFormLayout::Right, 0.45f);
        textLabel->OnShow();
        this->refElementLabel = textLabel;

    this->CreateCloud();
    this->CreateSkycolor();
    this->CreateSun();
    this->CreateSunlight();
    }
    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::HideSky()
{
    if (this->refSky.isvalid())
    {
    // FIXME: Check each slider
        this->refSkyTimeSlider->Release();
        this->refTimeFactorSlider->Release();
        this->refElementSlider->Release();
        this->refStateSlider->Release();
        this->refElementLabel->Release();
        this->refRefreshButton->Release();
        this->refSaveButton->Release();
        this->refStateTimeSlider->Release();
    }
    this->ReleaseCloud();
    this->ReleaseSkycolor();
    this->ReleaseSun();
    this->ReleaseSunlight();
}
//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::EventSky(const nGuiEvent& event)
{

    if (this->refSky.isvalid() &&
        this->refSkyTimeSlider.isvalid() &&
        this->refTimeFactorSlider.isvalid() &&
        this->refElementSlider.isvalid() &&
        this->refStateSlider.isvalid() &&
        this->refRefreshButton.isvalid() &&
        this->refSaveButton.isvalid() &&
        this->refStateTimeSlider.isvalid())
    {
        if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refRefreshButton))
        {
            this->refresh = true;
        }
        else if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refSaveButton))
        {
            this->saveSky = true;
        }
        if (event.GetWidget() == this->refSkyTimeSlider)
        {
            if (this->refSkyTimeSlider->Inside(nGuiServer::Instance()->GetMousePos()))
            {
                this->refSky->SetStartTime(this->refSky->GetStartTime() + (this->refSkyTimeSlider->GetValue() - this->refSky->GetSkyTime()));
                this->updateSkyTime = false;
            }
        }

        if (event.GetWidget() == this->refTimeFactorSlider)
        {
            this->refSky->SetTimeFactor(this->refTimeFactorSlider->GetValue());
        }

        if (event.GetWidget() == this->refElementSlider)
        {
            int actualElement = (int)floor(this->refElementSlider->GetValue() - 1);
            if (actualElement != this->activeElement)
            {
                this->oldElement = this->activeElement;
                this->activeElement = actualElement;
                this->elementReady = false;
                this->layoutChanged = true;
            }
        }

        if (event.GetWidget() == this->refStateSlider)
        {
            int actualState = (int)floor(this->refStateSlider->GetValue() - 1);
            if (actualState != this->activeState)
            {
                this->activeState = actualState;
                this->stateReady = false;
                this->layoutChanged = true;
            }
        }
        if (event.GetWidget() == this->refStateTimeSlider)
        {
            if ((this->activeState > -1) && (this->activeElement > -1))
            {
                int actualState = this->refSky->SetStateTime(this->activeElement, this->activeState, this->refStateTimeSlider->GetValue());
                this->refStateSlider->SetValue((float)actualState + 1);
            }
        }
    }

    this->EventCloud(event);
    this->EventSkycolor(event);
    this->EventSun(event);
    this->EventSunlight(event);

}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::CreateCloud()
{
    const float leftWidth = 0.3f;
    const float rightWidth = 0.15f;
    const float maxAngle = 360;
    const float minHeight = -90;
    const float maxHeight = 90;
    const float border = 0.005f;
    const float sliderOffset = 0.025f;

    kernelServer->PushCwd(this);

    nGuiHoriSliderGroup* slider;
    nGuiColorSliderGroup* colorSlider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudAddSlider");
    slider->SetLeftText("Cloud Add");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(-3.0f);
    slider->SetMaxValue(3.0f);
    slider->SetValue(0);
    slider->SetKnobSize(0.5f);
    slider->SetIncrement(0.001f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refElementLabel, 2*border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudAddSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudMulSlider");
    slider->SetLeftText("Cloud Mul");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(10.0f);
    slider->SetValue(1);
    slider->SetKnobSize(0.5f);
    slider->SetIncrement(0.001f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudAddSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudMulSlider = slider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "CloudColSlider");
    colorSlider->SetLabelText("Cloud Col");
    colorSlider->SetMaxIntensity(2.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(vector4(1,1,1,1));
    this->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refCloudMulSlider, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    this->refCloudColSlider = colorSlider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudGlowSlider");
    slider->SetLeftText("Cloud Glow");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(3.0f);
    slider->SetValue(0.5f);
    slider->SetKnobSize(0.5f);
    slider->SetIncrement(0.001f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudColSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudGlowSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudRangeSlider");
    slider->SetLeftText("Cloud Range");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(10.0f);
    slider->SetValue(1);
    slider->SetKnobSize(0.5f);
    slider->SetIncrement(0.001f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudGlowSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudRangeSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudDensSlider");
    slider->SetLeftText("Cloud Dens");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(1.0f);
    slider->SetValue(0);
    slider->SetKnobSize(0.5f);
    slider->SetIncrement(0.001f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudRangeSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudDensSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudMapResSlider");
    slider->SetLeftText("Cloud MapRes");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(5.0f);
    slider->SetValue(1);
    slider->SetKnobSize(0.5f);
    slider->SetIncrement(0.001f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudDensSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudMapResSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudStrucResSlider");
    slider->SetLeftText("Cloud StrucRes");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(5.0f);
    slider->SetValue(1.0f);
    slider->SetKnobSize(0.5f);
    slider->SetIncrement(0.001f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudMapResSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudStrucResSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudMapDirSlider");
    slider->SetLeftText("Cloud Dir");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(360.0f);
    slider->SetValue(0);
    slider->SetKnobSize(10.0f);
    slider->SetIncrement(0.1f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudStrucResSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudMapDirSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudMapSpeedSlider");
    slider->SetLeftText("Cloud MapSpeed");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.000001f);
    slider->SetMaxValue(10.0f);
    slider->SetValue(1);
    slider->SetKnobSize(1.0f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudMapDirSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudMapSpeedSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudStrucSpeedSlider");
    slider->SetLeftText("Cloud StrucSpeed");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.000001f);
    slider->SetMaxValue(10.0f);
    slider->SetValue(1);
    slider->SetKnobSize(1.0f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudMapSpeedSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudStrucSpeedSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "CloudWeightSlider");
    slider->SetLeftText("Cloud Bumpweight");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(1.0f);
    slider->SetValue(0.7f);
    slider->SetKnobSize(0.1f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refCloudStrucSpeedSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refCloudWeightSlider = slider;

    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::ShowCloud()
{
    if (this->refElement.isvalid())
    {
        if (this->refCloudAddSlider.isvalid())
        {
            this->UpdateSliderFromElement(this->refCloudAddSlider, nShaderState::CloudMod, -3, 3, 1);
            this->refCloudAddSlider->Show();
        }
        if (this->refCloudMulSlider.isvalid())
        {
            this->UpdateSliderFromElement(this->refCloudMulSlider, nShaderState::CloudMod, 0, 10, 2);
            this->refCloudMulSlider->Show();
        }
        if (this->refCloudColSlider.isvalid())
        {
            this->UpdateColorSliderFromElement(this->refCloudColSlider, nShaderState::MatDiffuse, 2);
            this->refCloudColSlider->Show();
        }
        if (this->refCloudGlowSlider.isvalid())
        {
            this->UpdateSliderFromElement(this->refCloudGlowSlider, nShaderState::Glow, 0, 3);
            this->refCloudGlowSlider->Show();
        }
        if (this->refCloudRangeSlider.isvalid())
        {
            this->UpdateSliderFromElement(this->refCloudRangeSlider, nShaderState::SunRange, 0, 10);
            this->refCloudRangeSlider->Show();
        }
        if (this->refCloudDensSlider.isvalid())
        {
            this->UpdateSliderFromElement(this->refCloudDensSlider, nShaderState::Density, 0, 1);
            this->refCloudDensSlider->Show();
        }
        if (this->refCloudMapResSlider.isvalid())
        {
            this->UpdateSliderFromElement(this->refCloudMapResSlider, nShaderState::Map0uvRes, 0, 5);
            this->refCloudMapResSlider->Show();
        }
        if (this->refCloudStrucResSlider.isvalid())
        {
            this->UpdateSliderFromElement(this->refCloudStrucResSlider, nShaderState::Map1uvRes, 0, 5);
            this->refCloudStrucResSlider->Show();
        }
        if (this->refCloudMapDirSlider.isvalid())
        {
            if (this->refElement->HasParam(nShaderState::Move))
            {
                vector4 vec = this->refElement->GetVector(nShaderState::Move);
                vector2 dirvec = vector2(vec.x,vec.y);
                if (dirvec.x == 0) dirvec.x = 0.0001f;
                float dir =(float)(90-((atan(fabs(dirvec.y)/fabs(dirvec.x))*180)/N_PI));
                if      ((dirvec.x > 0)&&(dirvec.y < 0)) dir += 90;
                else if ((dirvec.x < 0)&&(dirvec.y < 0)) dir += 180;
                else if ((dirvec.x < 0)&&(dirvec.y > 0)) dir += 270;
                float min = 0;
                float max = 360;
                this->refCloudMapDirSlider->SetMinValue(min);
                this->refCloudMapDirSlider->SetMaxValue(max);
                this->refCloudMapDirSlider->SetKnobSize((float)(fabs(min)+max)/10);
                this->refCloudMapDirSlider->SetValue(dir);
            }
            else
            {
                this->refCloudMapDirSlider->SetMinValue(0);
                this->refCloudMapDirSlider->SetMaxValue(0);
                this->refCloudMapDirSlider->SetKnobSize(1);
                this->refCloudMapDirSlider->SetValue(0);
            }
            this->refCloudMapDirSlider->Show();
        }
        if (this->refCloudMapSpeedSlider.isvalid())
        {
            if (this->refElement->HasParam(nShaderState::Move))
            {
                vector4 vec = this->refElement->GetVector(nShaderState::Move);
                vector2 dirvec = vector2(vec.x,vec.y);
                float min = 0;
                float max = 10;
                this->refCloudMapSpeedSlider->SetMinValue(min);
                this->refCloudMapSpeedSlider->SetMaxValue(max);
                this->refCloudMapSpeedSlider->SetKnobSize((float)(fabs(min)+max)/10);
                this->refCloudMapSpeedSlider->SetValue(dirvec.len());
            }
            else
            {
                this->refCloudMapSpeedSlider->SetMinValue(0);
                this->refCloudMapSpeedSlider->SetMaxValue(0);
                this->refCloudMapSpeedSlider->SetKnobSize(1);
                this->refCloudMapSpeedSlider->SetValue(0);
            }
            this->refCloudMapSpeedSlider->Show();
        }
        if (this->refCloudStrucSpeedSlider.isvalid())
        {
            if (this->refElement->HasParam(nShaderState::Move))
            {
                vector4 vec = this->refElement->GetVector(nShaderState::Move);
                vector2 dirvec = vector2(vec.z,vec.w);
                float min = 0;
                float max = 10;
                this->refCloudStrucSpeedSlider->SetMinValue(min);
                this->refCloudStrucSpeedSlider->SetMaxValue(max);
                this->refCloudStrucSpeedSlider->SetKnobSize((float)(fabs(min)+max)/10);
                this->refCloudStrucSpeedSlider->SetValue(dirvec.len());
            }
            else
            {
                this->refCloudStrucSpeedSlider->SetMinValue(0);
                this->refCloudStrucSpeedSlider->SetMaxValue(0);
                this->refCloudStrucSpeedSlider->SetKnobSize(1);
                this->refCloudStrucSpeedSlider->SetValue(0);
            }
            this->refCloudStrucSpeedSlider->Show();
        }
        if (this->refCloudWeightSlider.isvalid())
        {
            this->UpdateSliderFromElement(this->refCloudWeightSlider, nShaderState::Weight, 0, 1);
            this->refCloudWeightSlider->Show();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::EventCloud(const nGuiEvent& event)
{

    if (this->elementReady &&
        (this->activeType == nSkyNode::CloudElement) &&
        this->refSky.isvalid() &&
        this->refElement.isvalid() &&
        this->refCloudAddSlider.isvalid() &&
        this->refCloudColSlider.isvalid() &&
        this->refCloudGlowSlider.isvalid() &&
        this->refCloudRangeSlider.isvalid() &&
        this->refCloudDensSlider.isvalid() &&
        this->refCloudMapResSlider.isvalid() &&
        this->refCloudStrucResSlider.isvalid() &&
        this->refCloudMapDirSlider.isvalid() &&
        this->refCloudMapSpeedSlider.isvalid() &&
        this->refCloudStrucSpeedSlider.isvalid() &&
        this->refCloudMulSlider.isvalid())
    {
        if (event.GetWidget() == this->refCloudAddSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudAddSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::CloudMod))
                {
                    vector4 vec = this->refElement->GetVector(nShaderState::CloudMod);
                    vec.x = this->refCloudAddSlider->GetValue();
                    this->refElement->SetVector(nShaderState::CloudMod, vec);
                }
                else
                {
                    vector4 vec(0,1,0,0);
                    vec.x = this->refCloudAddSlider->GetValue();
                    this->refElement->SetVector(nShaderState::CloudMod, vec);
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudMulSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudMulSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::CloudMod))
                {
                    vector4 vec = this->refElement->GetVector(nShaderState::CloudMod);
                    vec.y = this->refCloudMulSlider->GetValue();
                    this->refElement->SetVector(nShaderState::CloudMod, vec);
                }
                else
                {
                    vector4 vec(0,1,0,0);
                    vec.y = this->refCloudMulSlider->GetValue();
                    this->refElement->SetVector(nShaderState::CloudMod, vec);
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudColSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudColSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::MatDiffuse))
                {
                    this->refElement->SetVector(nShaderState::MatDiffuse, this->refCloudColSlider->GetColor());
                }
                else
                {
                    this->refElement->SetVector(nShaderState::MatDiffuse, this->refCloudColSlider->GetColor());
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudGlowSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudGlowSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Glow))
                {
                    this->refElement->SetFloat(nShaderState::Glow, this->refCloudGlowSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::Glow, this->refCloudGlowSlider->GetValue());
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudRangeSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudRangeSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::SunRange))
                {
                    this->refElement->SetFloat(nShaderState::SunRange, this->refCloudRangeSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::SunRange, this->refCloudRangeSlider->GetValue());
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudDensSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudDensSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Density))
                {
                    this->refElement->SetFloat(nShaderState::Density, this->refCloudDensSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::Density, this->refCloudDensSlider->GetValue());
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudMapResSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudMapResSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Map0uvRes))
                {
                    this->refElement->SetFloat(nShaderState::Map0uvRes, this->refCloudMapResSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::Map0uvRes, this->refCloudMapResSlider->GetValue());
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudStrucResSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudStrucResSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Map1uvRes))
                {
                    this->refElement->SetFloat(nShaderState::Map1uvRes, this->refCloudStrucResSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::Map1uvRes, this->refCloudStrucResSlider->GetValue());
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudMapDirSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudMapDirSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Move))
                {
                    vector4 vec = this->refElement->GetVector(nShaderState::Move);
                    float dir = this->refCloudMapDirSlider->GetValue();
                    dir = (dir*N_PI)/180;
                    vector2 dirvec(vec.x,vec.y);
                    vector2 dirvec2(vec.w,vec.z);
                    float len = dirvec.len();
                    float len2 = dirvec2.len();
                    dirvec = vector2(0,1);
                    dirvec.rotate(dir);
                    vec.x = dirvec.x * len;
                    vec.y = dirvec.y * len;
                    vec.z = dirvec.x * len2; // Zusammengeschaltet
                    vec.w = dirvec.y * len2;
                    this->refElement->SetVector(nShaderState::Move, vec);
                }
                else
                {
                    vector4 vec(0.0f,0.1f,0.0f,0.1f);
                    float dir = this->refCloudMapDirSlider->GetValue();
                    dir = (dir*N_PI)/180;
                    vector2 dirvec(vec.x,vec.y);
                    vector2 dirvec2(vec.w,vec.z);
                    float len = dirvec.len();
                    float len2 = dirvec2.len();
                    dirvec = vector2(0,1);
                    dirvec.rotate(dir);
                    vec.x = dirvec.x * len;
                    vec.y = dirvec.y * len;
                    vec.z = dirvec.x * len2; // Zusammengeschaltet
                    vec.w = dirvec.y * len2;
                    this->refElement->SetVector(nShaderState::Move, vec);
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudMapSpeedSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudMapSpeedSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Move))
                {
                    vector4 vec = this->refElement->GetVector(nShaderState::Move);
                    vector2 dirvec(vec.x,vec.y);
                    dirvec.norm();
                    float len = this->refCloudMapSpeedSlider->GetValue();
                    vec.x = dirvec.x * len;
                    vec.y = dirvec.y * len;
                    this->refElement->SetVector(nShaderState::Move, vec);
                }
                else
                {
                    vector4 vec(0.0f,0.1f,0.0f,0.1f);
                    vector2 dirvec(vec.x,vec.y);
                    dirvec.norm();
                    float len = this->refCloudMapSpeedSlider->GetValue();
                    vec.x = dirvec.x * len;
                    vec.y = dirvec.y * len;
                    this->refElement->SetVector(nShaderState::Move, vec);
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudStrucSpeedSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudStrucSpeedSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Move))
                {
                    vector4 vec = this->refElement->GetVector(nShaderState::Move);
                    vector2 dirvec(vec.x,vec.y);
                    dirvec.norm();
                    float len = this->refCloudStrucSpeedSlider->GetValue();
                    vec.z = dirvec.x * len;
                    vec.w = dirvec.y * len;
                    this->refElement->SetVector(nShaderState::Move, vec);
                }
                else
                {
                    vector4 vec(0.0f,0.1f,0.0f,0.1f);
                    vector2 dirvec(vec.x,vec.y);
                    dirvec.norm();
                    float len = this->refCloudStrucSpeedSlider->GetValue();
                    vec.z = dirvec.x * len;
                    vec.w = dirvec.y * len;
                    this->refElement->SetVector(nShaderState::Move, vec);
                    this->ShowCloud();
                }
            }
        }

        if (event.GetWidget() == this->refCloudWeightSlider)
        {
            if (this->refElement.isvalid() && (this->refCloudWeightSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Weight))
                {
                    this->refElement->SetFloat(nShaderState::Weight, this->refCloudWeightSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::Weight, this->refCloudWeightSlider->GetValue());
                    this->ShowCloud();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::HideCloud()
{
    if (this->refElement.isvalid())
    {
        if (this->refCloudAddSlider.isvalid())
            this->refCloudAddSlider->Hide();
        if (this->refCloudMulSlider.isvalid())
            this->refCloudMulSlider->Hide();
        if (this->refCloudColSlider.isvalid())
            this->refCloudColSlider->Hide();
        if (this->refCloudGlowSlider.isvalid())
            this->refCloudGlowSlider->Hide();
        if (this->refCloudRangeSlider.isvalid())
            this->refCloudRangeSlider->Hide();
        if (this->refCloudDensSlider.isvalid())
            this->refCloudDensSlider->Hide();
        if (this->refCloudMapResSlider.isvalid())
            this->refCloudMapResSlider->Hide();
        if (this->refCloudStrucResSlider.isvalid())
            this->refCloudStrucResSlider->Hide();
        if (this->refCloudMapDirSlider.isvalid())
            this->refCloudMapDirSlider->Hide();
        if (this->refCloudMapSpeedSlider.isvalid())
            this->refCloudMapSpeedSlider->Hide();
        if (this->refCloudStrucSpeedSlider.isvalid())
            this->refCloudStrucSpeedSlider->Hide();
        if (this->refCloudWeightSlider.isvalid())
            this->refCloudWeightSlider->Hide();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::ReleaseCloud()
{
    if (this->refElement.isvalid())
    {
        // FIXME: check each one
        this->refCloudAddSlider->Release();
        this->refCloudMulSlider->Release();
        this->refCloudColSlider->Release();
        this->refCloudGlowSlider->Release();
        this->refCloudRangeSlider->Release();
        this->refCloudDensSlider->Release();
        this->refCloudMapResSlider->Release();
        this->refCloudStrucResSlider->Release();
        this->refCloudMapDirSlider->Release();
        this->refCloudMapSpeedSlider->Release();
        this->refCloudStrucSpeedSlider->Release();
        this->refCloudWeightSlider->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::CreateSkycolor()
{
    const float leftWidth = 0.3f;
    const float rightWidth = 0.15f;
    const float border = 0.005f;
    const float sliderOffset = 0.025f;

    kernelServer->PushCwd(this);

    nGuiHoriSliderGroup* slider;
    nGuiColorSliderGroup* colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "SkyTopColSlider");
    colorSlider->SetLabelText("Sky TopCol");
    colorSlider->SetMaxIntensity(1.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(vector4(0.3f,0.3f,1,1));
    this->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refElementLabel, 2*border);
    this->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    this->refSkyTopColSlider = colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "SkyBotColSlider");
    colorSlider->SetLabelText("Sky BotCol");
    colorSlider->SetMaxIntensity(1.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(vector4(0.6f,0.6f,1,1));
    this->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refSkyTopColSlider, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    this->refSkyBotColSlider = colorSlider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkyBottomSlider");
    slider->SetLeftText("Sky Bottom");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(-1.0f);
    slider->SetMaxValue(1.0f);
    slider->SetValue(0);
    slider->SetKnobSize(0.4f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refSkyBotColSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refSkyBottomSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkyBrightSlider");
    slider->SetLeftText("Sky Brightness");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(1.0f);
    slider->SetValue(1);
    slider->SetKnobSize(0.2f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refSkyBottomSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refSkyBrightSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkySatSlider");
    slider->SetLeftText("Sky Saturation");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(1.0f);
    slider->SetValue(1);
    slider->SetKnobSize(0.2f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top, this->refSkyBrightSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refSkySatSlider = slider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "SkySunColSlider");
    colorSlider->SetLabelText("Sky SunCol");
    colorSlider->SetMaxIntensity(1.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(vector4(1,1,1,1));
    this->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refSkySatSlider, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    this->refSkySunColSlider = colorSlider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkySunRangeSlider");
    slider->SetLeftText("Sky Sunrange");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(2.0f);
    slider->SetValue(1);
    slider->SetKnobSize(0.4f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top, this->refSkySunColSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refSkySunRangeSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkySunIntensSlider");
    slider->SetLeftText("Sky Sunintensity");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(2.0f);
    slider->SetValue(1);
    slider->SetKnobSize(0.4f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top, this->refSkySunRangeSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refSkySunIntensSlider = slider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SkySunFlatSlider");
    slider->SetLeftText("Sky Sunflatness");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(5.0f);
    slider->SetValue(0);
    slider->SetKnobSize(0.4f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top, this->refSkySunIntensSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refSkySunFlatSlider = slider;

    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::ShowSkycolor()
{
    if (this->refElement.isvalid())
    {
        if (this->refSkyTopColSlider.isvalid())
        {
            this->UpdateColorSliderFromElement(this->refSkyTopColSlider,nShaderState::TopColor, 1);
            this->refSkyTopColSlider->Show();
        }
        if (this->refSkyBotColSlider.isvalid())
        {
            this->UpdateColorSliderFromElement(this->refSkyBotColSlider,nShaderState::BottomColor, 1);
            this->refSkyBotColSlider->Show();
        }
        if (this->refSkyBottomSlider.isvalid())
        {
            UpdateSliderFromElement(refSkyBottomSlider,nShaderState::SkyBottom, -2, 1);
            this->refSkyBottomSlider->Show();
        }
        if (this->refSkyBrightSlider.isvalid())
        {
            UpdateSliderFromElement(refSkyBrightSlider,nShaderState::Brightness, 0, 1, 4);
            this->refSkyBrightSlider->Show();
        }
        if (this->refSkySatSlider.isvalid())
        {
            UpdateSliderFromElement(refSkySatSlider,nShaderState::Saturation, 0, 1, 4);
            this->refSkySatSlider->Show();
        }
        if (this->refSkySunColSlider.isvalid())
        {
            this->UpdateColorSliderFromElement(this->refSkySunColSlider,nShaderState::SunColor, 1);
            this->refSkySunColSlider->Show();
        }
        if (this->refSkySunRangeSlider.isvalid())
        {
            UpdateSliderFromElement(refSkySunRangeSlider,nShaderState::SunRange, 0, 2);
            this->refSkySunRangeSlider->Show();
        }
        if (this->refSkySunIntensSlider.isvalid())
        {
            UpdateSliderFromElement(refSkySunIntensSlider,nShaderState::Intensity0, 0, 2);
            this->refSkySunIntensSlider->Show();
        }
        if (this->refSkySunFlatSlider.isvalid())
        {
            UpdateSliderFromElement(refSkySunFlatSlider,nShaderState::SunFlat, 0, 8);
            this->refSkySunFlatSlider->Show();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::EventSkycolor(const nGuiEvent& event)
{

    if (this->elementReady &&
        (this->activeType == nSkyNode::SkyElement) &&
        this->refSky.isvalid() &&
        this->refElement.isvalid() &&
        this->refSkyTopColSlider.isvalid() &&
        this->refSkyBotColSlider.isvalid() &&
        this->refSkySunColSlider.isvalid() &&
        this->refSkyBrightSlider.isvalid() &&
        this->refSkySatSlider.isvalid()&&
        this->refSkyBottomSlider.isvalid()&&
        this->refSkySunRangeSlider.isvalid()&&
        this->refSkySunIntensSlider.isvalid()&&
        this->refSkySunFlatSlider.isvalid())
    {
        if (event.GetWidget() == this->refSkyTopColSlider)
        {
            if (this->refElement.isvalid() && (this->refSkyTopColSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::TopColor))
                {
                    this->refElement->SetVector(nShaderState::TopColor, this->refSkyTopColSlider->GetColor());
                }
                else
                {
                    this->refElement->SetVector(nShaderState::TopColor, this->refSkyTopColSlider->GetColor());
                    this->ShowSkycolor();
                }
            }
        }

        if (event.GetWidget() == this->refSkyBotColSlider)
        {
            if (this->refElement.isvalid() && (this->refSkyBotColSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::BottomColor))
                {
                    this->refElement->SetVector(nShaderState::BottomColor, this->refSkyBotColSlider->GetColor());
                }
                else
                {
                    this->refElement->SetVector(nShaderState::BottomColor, this->refSkyBotColSlider->GetColor());
                    this->ShowSkycolor();
                }
            }
        }

        if (event.GetWidget() == this->refSkyBottomSlider)
        {
            if (this->refElement.isvalid() && (this->refSkyBottomSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::SkyBottom))
                {
                    this->refElement->SetFloat(nShaderState::SkyBottom, this->refSkyBottomSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::SkyBottom, this->refSkyBottomSlider->GetValue());
                    this->ShowSkycolor();
                }
            }
        }

        if (event.GetWidget() == this->refSkyBrightSlider)
        {
            if (this->refElement.isvalid() && (this->refSkyBrightSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Brightness))
                {
                    this->refElement->SetVector(nShaderState::Brightness, vector4(1,1,1,this->refSkyBrightSlider->GetValue()));
                }
                else
                {
                    this->refElement->SetVector(nShaderState::Brightness, vector4(1,1,1,this->refSkyBrightSlider->GetValue()));
                    this->ShowSkycolor();
                }
            }
        }

        if (event.GetWidget() == this->refSkySatSlider)
        {
            if (this->refElement.isvalid() && (this->refSkySatSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Saturation))
                {
                    this->refElement->SetVector(nShaderState::Saturation, vector4(1,1,1,this->refSkySatSlider->GetValue()));
                }
                else
                {
                    this->refElement->SetVector(nShaderState::Saturation, vector4(1,1,1,this->refSkySatSlider->GetValue()));
                    this->ShowSkycolor();
                }
            }
        }

        if (event.GetWidget() == this->refSkySunColSlider)
        {
            if (this->refElement.isvalid() && (this->refSkySunColSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::SunColor))
                {
                    this->refElement->SetVector(nShaderState::SunColor, this->refSkySunColSlider->GetColor());
                }
                else
                {
                    this->refElement->SetVector(nShaderState::SunColor, this->refSkySunColSlider->GetColor());
                    this->ShowSkycolor();
                }
            }
        }

        if (event.GetWidget() == this->refSkySunRangeSlider)
        {
            if (this->refElement.isvalid() && (this->refSkySunRangeSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::SunRange))
                {
                    this->refElement->SetFloat(nShaderState::SunRange, this->refSkySunRangeSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::SunRange, this->refSkySunRangeSlider->GetValue());
                    this->ShowSkycolor();
                }
            }
        }

        if (event.GetWidget() == this->refSkySunIntensSlider)
        {
            if (this->refElement.isvalid() && (this->refSkySunIntensSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::Intensity0))
                {
                    this->refElement->SetFloat(nShaderState::Intensity0, this->refSkySunIntensSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::Intensity0, this->refSkySunIntensSlider->GetValue());
                    this->ShowSkycolor();
                }
            }
        }

        if (event.GetWidget() == this->refSkySunFlatSlider)
        {
            if (this->refElement.isvalid() && (this->refSkySunFlatSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::SunFlat))
                {
                    this->refElement->SetFloat(nShaderState::SunFlat, this->refSkySunFlatSlider->GetValue());
                }
                else
                {
                    this->refElement->SetFloat(nShaderState::SunFlat, this->refSkySunFlatSlider->GetValue());
                    this->ShowSkycolor();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::HideSkycolor()
{
    if (this->refSkyTopColSlider.isvalid())
        this->refSkyTopColSlider->Hide();
    if (this->refSkyBotColSlider.isvalid())
        this->refSkyBotColSlider->Hide();
    if (this->refSkySunColSlider.isvalid())
        this->refSkySunColSlider->Hide();
    if (this->refSkyBrightSlider.isvalid())
        this->refSkyBrightSlider->Hide();
    if (this->refSkySatSlider.isvalid())
        this->refSkySatSlider->Hide();
    if (this->refSkyBottomSlider.isvalid())
        this->refSkyBottomSlider->Hide();
    if (this->refSkySunRangeSlider.isvalid())
        this->refSkySunRangeSlider->Hide();
    if (this->refSkySunIntensSlider.isvalid())
        this->refSkySunIntensSlider->Hide();
    if (this->refSkySunFlatSlider.isvalid())
        this->refSkySunFlatSlider->Hide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::ReleaseSkycolor()
{
    if (this->refElement.isvalid())
    {   // FIXME: check each one
        this->refSkyTopColSlider->Release();
        this->refSkyBotColSlider->Release();
        this->refSkySunColSlider->Release();
        this->refSkyBrightSlider->Release();
        this->refSkySatSlider->Release();
        this->refSkyBottomSlider->Release();
        this->refSkySunRangeSlider->Release();
        this->refSkySunIntensSlider->Release();
        this->refSkySunFlatSlider->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::CreateSun()
{
    const float leftWidth = 0.3f;
    const float rightWidth = 0.15f;
    const float border = 0.005f;
    const float sliderOffset = 0.025f;

    kernelServer->PushCwd(this);

    nGuiHoriSliderGroup* slider;
    nGuiColorSliderGroup* colorSlider;

    colorSlider = (nGuiColorSliderGroup*)kernelServer->New("nguicolorslidergroup", "SunColSlider");
    colorSlider->SetLabelText("Sun Color");
    colorSlider->SetMaxIntensity(10.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(vector4(1.0f, 1.0f, 1.0f, 1.0f));
    this->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refElementLabel, 2*border);
    this->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    this->refSunColSlider = colorSlider;

    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "SunScaleSlider");
    slider->SetLeftText("Sun Scale");
    slider->SetRightText("%f");
    slider->SetDisplayFormat(nGuiHoriSliderGroup::Float);
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(5.0f);
    slider->SetValue(0.0f);
    slider->SetKnobSize(0.05f);
    slider->SetIncrement(0.01f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    this->AttachWidget(slider, nGuiFormLayout::Top,this->refSunColSlider, border);
    this->AttachForm(slider, nGuiFormLayout::Left, border);
    this->AttachForm(slider, nGuiFormLayout::Right, border);
    this->refSunScaleSlider = slider;

    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::ShowSun()
{
    if (this->refElement.isvalid())
    {
        if (this->refSunColSlider.isvalid())
        {
            this->UpdateColorSliderFromElement(this->refSunColSlider,nShaderState::MatDiffuse, 10);
            this->refSunColSlider->Show();
        }

        if (this->refSunScaleSlider.isvalid())
        {
            UpdateSliderFromElement(refSunScaleSlider,nShaderState::ScaleVector, 0, 5, 4);
            this->refSunScaleSlider->Show();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::EventSun(const nGuiEvent& event)
{

    if (this->elementReady &&
        (this->activeType == nSkyNode::SunElement) &&
        this->refSky.isvalid() &&
        this->refElement.isvalid() &&
        this->refSunColSlider.isvalid() &&
        this->refSunScaleSlider.isvalid())
    {
        if (event.GetWidget() == this->refSunColSlider)
        {
            if (this->refElement.isvalid() && (this->refSunColSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::MatDiffuse))
                {
                    this->refElement->SetVector(nShaderState::MatDiffuse, this->refSunColSlider->GetColor());
                }
                else
                {
                    this->refElement->SetVector(nShaderState::MatDiffuse, this->refSunColSlider->GetColor());
                    this->ShowSun();
                }
            }
        }

        if (event.GetWidget() == this->refSunScaleSlider)
        {
            if (this->refElement.isvalid() && (this->refSunScaleSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::ScaleVector))
                {
                    this->refElement->SetVector(nShaderState::ScaleVector, vector4(1,1,1,this->refSunScaleSlider->GetValue()));
                }
                else
                {
                    this->refElement->SetVector(nShaderState::ScaleVector, vector4(1,1,1,this->refSunScaleSlider->GetValue()));
                    this->ShowSun();
                }
            }
        }
     }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::HideSun()
{
    if (this->refSunColSlider.isvalid())
        this->refSunColSlider->Hide();
    if (this->refSunScaleSlider.isvalid())
        this->refSunScaleSlider->Hide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::ReleaseSun()
{
    if (this->refElement.isvalid())
    {   // FIXME: check each one
        this->refSunColSlider->Release();
        this->refSunScaleSlider->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::CreateSunlight()
{
    const float leftWidth = 0.3f;
    const float rightWidth = 0.15f;
    const float maxAngle = 360.0f;
    const float minHeight = -90.0f;
    const float maxHeight = 90.0f;
    const float border = 0.005f;
    const float sliderOffset = 0.025f;

    kernelServer->PushCwd(this);

    nGuiColorSliderGroup* colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "SunlightDiffColSlider");
    colorSlider->SetLabelText("Sunlight Diffuse");
    colorSlider->SetMaxIntensity(3.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(vector4(1,1,1,1));
    this->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refElementLabel, 2*border);
    this->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    this->refSunLightDiffuseColSlider = colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "SunlightDiff1ColSlider");
    colorSlider->SetLabelText("Sunlight Diffuse1");
    colorSlider->SetMaxIntensity(3.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(vector4(1,1,1,1));
    this->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refSunLightDiffuseColSlider, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    this->refSunLightDiffuse1ColSlider = colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "SunlightAmbiColSlider");
    colorSlider->SetLabelText("Sunlight Ambient");
    colorSlider->SetMaxIntensity(3.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(vector4(1,1,1,1));
    this->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refSunLightDiffuse1ColSlider, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    this->refSunLightAmbientColSlider = colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "SunlightSpecColSlider");
    colorSlider->SetLabelText("Sunlight Specular");
    colorSlider->SetMaxIntensity(3.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(vector4(1,1,1,1));
    this->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refSunLightAmbientColSlider, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    this->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    this->refSunLightSpecularColSlider = colorSlider;

    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::ShowSunlight()
{
    if (this->refElement.isvalid())
    {
        if (this->refSunLightDiffuseColSlider.isvalid())
        {
            this->UpdateColorSliderFromElement(this->refSunLightDiffuseColSlider,nShaderState::LightDiffuse, 10);
            this->refSunLightDiffuseColSlider->Show();
        }
        if (this->refSunLightDiffuse1ColSlider.isvalid())
        {
            this->UpdateColorSliderFromElement(this->refSunLightDiffuse1ColSlider,nShaderState::LightDiffuse1, 10);
            this->refSunLightDiffuse1ColSlider->Show();
        }
        if (this->refSunLightAmbientColSlider.isvalid())
        {
            this->UpdateColorSliderFromElement(this->refSunLightAmbientColSlider,nShaderState::LightAmbient, 10);
            this->refSunLightAmbientColSlider->Show();
        }
        if (this->refSunLightSpecularColSlider.isvalid())
        {
            this->UpdateColorSliderFromElement(this->refSunLightSpecularColSlider,nShaderState::LightSpecular, 10);
            this->refSunLightSpecularColSlider->Show();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::EventSunlight(const nGuiEvent& event)
{

    if (this->elementReady &&
        (this->activeType == nSkyNode::LightElement) &&
        this->refSky.isvalid() &&
        this->refElement.isvalid() &&
        this->refSunLightDiffuseColSlider.isvalid() &&
        this->refSunLightDiffuse1ColSlider.isvalid() &&
        this->refSunLightAmbientColSlider.isvalid() &&
        this->refSunLightSpecularColSlider.isvalid())
    {
        if (event.GetWidget() == this->refSunLightDiffuseColSlider)
        {
            if (this->refElement.isvalid() && (this->refSunLightDiffuseColSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::LightDiffuse))
                {
                    this->refElement->SetVector(nShaderState::LightDiffuse, this->refSunLightDiffuseColSlider->GetColor());
                }
                else
                {
                    this->refElement->SetVector(nShaderState::LightDiffuse, this->refSunLightDiffuseColSlider->GetColor());
                    this->ShowSunlight();
                }
            }
        }

        if (event.GetWidget() == this->refSunLightDiffuse1ColSlider)
        {
            if (this->refElement.isvalid() && (this->refSunLightDiffuse1ColSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::LightDiffuse1))
                {
                    this->refElement->SetVector(nShaderState::LightDiffuse1, this->refSunLightDiffuse1ColSlider->GetColor());
                }
                else
                {
                    this->refElement->SetVector(nShaderState::LightDiffuse1, this->refSunLightDiffuse1ColSlider->GetColor());
                    this->ShowSunlight();
                }
            }
        }
        if (event.GetWidget() == this->refSunLightAmbientColSlider)
        {
            if (this->refElement.isvalid() && (this->refSunLightAmbientColSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::LightAmbient))
                {
                    this->refElement->SetVector(nShaderState::LightAmbient, this->refSunLightAmbientColSlider->GetColor());
                }
                else
                {
                    this->refElement->SetVector(nShaderState::LightAmbient, this->refSunLightAmbientColSlider->GetColor());
                    this->ShowSunlight();
                }
            }
        }
        if (event.GetWidget() == this->refSunLightSpecularColSlider)
        {
            if (this->refElement.isvalid() && (this->refSunLightSpecularColSlider->Inside(nGuiServer::Instance()->GetMousePos())))
            {
                if (this->refElement->HasParam(nShaderState::LightSpecular))
                {
                    this->refElement->SetVector(nShaderState::LightSpecular, this->refSunLightSpecularColSlider->GetColor());
                }
                else
                {
                    this->refElement->SetVector(nShaderState::LightSpecular, this->refSunLightSpecularColSlider->GetColor());
                    this->ShowSunlight();
                }
            }
        }
     }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::HideSunlight()
{
    if (this->refSunLightDiffuseColSlider.isvalid())
        this->refSunLightDiffuseColSlider->Hide();
    if (this->refSunLightDiffuse1ColSlider.isvalid())
        this->refSunLightDiffuse1ColSlider->Hide();
    if (this->refSunLightAmbientColSlider.isvalid())
        this->refSunLightAmbientColSlider->Hide();
    if (this->refSunLightSpecularColSlider.isvalid())
        this->refSunLightSpecularColSlider->Hide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkyEditor::ReleaseSunlight()
{
    if (this->refElement.isvalid())
    {   // FIXME: Check each one
        this->refSunLightDiffuseColSlider->Release();
        this->refSunLightDiffuse1ColSlider->Release();
        this->refSunLightAmbientColSlider->Release();
        this->refSunLightSpecularColSlider->Release();
    }
}


//------------------------------------------------------------------------------
/**
    called per frame when parent widget is visible
*/
void
nGuiSkyEditor::OnFrame()
{
    nSkyNode::ElementType type;
    if (this->layoutChanged)
    {
        // Element
        if (!this->elementReady)
        {

            if (this->oldElement > -1)
            {
                type = this->refSky->GetElementType(this->oldElement);
                switch (type)
                {
                    case nSkyNode::CloudElement: this->HideCloud(); break;
                    case nSkyNode::SkyElement: this->HideSkycolor(); break;
                    case nSkyNode::SunElement: this->HideSun(); break;
                    case nSkyNode::LightElement: this->HideSunlight(); break;
                }
                this->activeType = nSkyNode::InvalidElement;
            }


            if (this->activeElement > -1)
            {
                this->refElement = this->refSky->GetElement(this->activeElement);
                type = this->refSky->GetElementType(this->activeElement);
                switch (type)
                {
                    case nSkyNode::CloudElement: this->ShowCloud(); break;
                    case nSkyNode::SkyElement: this->ShowSkycolor(); break;
                    case nSkyNode::SunElement: this->ShowSun(); break;
                    case nSkyNode::LightElement: this->ShowSunlight(); break;
                }
                this->activeType = type;
                this->refStateSlider->SetValue(0);
                this->refStateSlider->SetMaxValue((float)this->refSky->GetNumStates(this->activeElement));
                this->refStateSlider->OnShow();
                this->refElementLabel->SetText(this->refElement->GetName());
                this->refStateTimeSlider->SetValue(0);
                this->refStateTimeSlider->SetKnobSize(1);
                this->refStateTimeSlider->SetMaxValue(0);
                this->refStateTimeSlider->OnShow();
            }
            else if (this->activeElement == -1)
            {
                this->refStateSlider->SetValue(0);
                this->refStateSlider->SetMaxValue(0);
                this->refStateSlider->OnShow();
                this->refElementLabel->SetText("nSkyNode");
                this->refStateTimeSlider->SetValue(0);
                this->refStateTimeSlider->SetKnobSize(1);
                this->refStateTimeSlider->SetMaxValue(0);
                this->refStateTimeSlider->OnShow();
            }
            this->elementReady = true;
        }

        //State
        if (!this->stateReady)
        {
            if (this->activeElement > -1)
            {
                type = this->refSky->GetElementType(this->activeElement);
                switch (type)
                {
                    case nSkyNode::CloudElement: this->HideCloud(); break;
                    case nSkyNode::SkyElement: this->HideSkycolor(); break;
                    case nSkyNode::SunElement: this->HideSun(); break;
                    case nSkyNode::LightElement: this->HideSunlight(); break;
                }
                this->activeType = nSkyNode::InvalidElement;
            }

            if ((this->activeState > -1) && (this->activeElement > -1))
            {
                this->refElement = this->refSky->GetState(this->activeElement, this->activeState);
                type = this->refSky->GetElementType(this->activeElement);
                switch (type)
                {
                    case nSkyNode::CloudElement: this->ShowCloud(); break;
                    case nSkyNode::SkyElement: this->ShowSkycolor(); break;
                    case nSkyNode::SunElement: this->ShowSun(); break;
                    case nSkyNode::LightElement: this->ShowSunlight(); break;
                }
                this->activeType = type;
                this->refElementLabel->SetText(this->refElement->GetName());
                this->refStateTimeSlider->SetMaxValue(this->refSky->GetTimePeriod());
                this->refStateTimeSlider->SetValue(this->refSky->GetStateTime(this->activeElement, this->activeState));
                this->refStateTimeSlider->SetKnobSize(this->refSky->GetTimePeriod()/10);
                this->refStateTimeSlider->OnShow();
            }
            else if ((this->activeState == -1) && (this->activeElement > -1))
            {
                this->refElement = this->refSky->GetElement(this->activeElement);
                type = this->refSky->GetElementType(this->activeElement);
                switch (type)
                {
                    case nSkyNode::CloudElement: this->ShowCloud(); break;
                    case nSkyNode::SkyElement: this->ShowSkycolor(); break;
                    case nSkyNode::SunElement: this->ShowSun(); break;
                    case nSkyNode::LightElement: this->ShowSunlight(); break;
                }
                this->activeType = type;
                this->refStateSlider->SetValue(0);
                this->refStateSlider->SetMaxValue((float)this->refSky->GetNumStates(this->activeElement));
                this->refStateSlider->OnShow();
                this->refElementLabel->SetText(this->refElement->GetName());
                this->refStateTimeSlider->SetValue(0);
                this->refStateTimeSlider->SetKnobSize(1);
                this->refStateTimeSlider->SetMaxValue(0);
                this->refStateTimeSlider->OnShow();
            }

            this->stateReady = true;
        }


        this->layoutChanged = false;
        this->UpdateLayout(this->GetRect());
    }

    // Refresh Button was pressed
    if (this->refresh && (this->activeElement > -1) && (this->refSky.isvalid()))
    {
        type = this->refSky->GetElementType(this->activeElement);
        switch (type)
        {
            case nSkyNode::CloudElement: this->ShowCloud(); break;
            case nSkyNode::SkyElement: this->ShowSkycolor(); break;
            case nSkyNode::SunElement: this->ShowSun(); break;
            case nSkyNode::LightElement: this->ShowSunlight(); break;
        }
        this->refStateTimeSlider->OnShow();

        this->refresh = false;
    }

    // Don't update skytime slider, if it was moved
    if (this->updateSkyTime && this->refSkyTimeSlider.isvalid() && this->refSky.isvalid())
    {
        this->refSkyTimeSlider->SetValue(this->refSky->GetSkyTime());
    }
    else
    {
        this->updateSkyTime = true;
    }

    // Save Button was pressed
    if (this->saveSky)
    {   // FIXME: Hardcoded...
        nDynAutoRef<nRoot> refPath(this->skyPath.ExtractToLastSlash().Get());
        refPath->SaveAs("proj:export/gfxlib/examples/SkySave.n2");
        this->saveSky = false;
    }

    nGuiFormLayout::OnFrame();

}

//------------------------------------------------------------------------------
/**

*/
void
nGuiSkyEditor::UpdateSliderFromElement(nGuiHoriSliderGroup* slider, nShaderState::Param param, float min, float max, int vectornr)
{
    if (this->refElement->HasParam(param))
    {
        slider->SetMinValue(min);
        slider->SetMaxValue(max);
        slider->SetKnobSize((float)(fabs(min) + max)/10);
        if (vectornr == 0)
        {
            slider->SetValue(this->refElement->GetFloat(param));
        }
        else if (vectornr > 0 && vectornr <= 4)
        {
            switch (vectornr)
            {
                case 1: slider->SetValue(this->refElement->GetVector(param).x); break;
                case 2: slider->SetValue(this->refElement->GetVector(param).y); break;
                case 3: slider->SetValue(this->refElement->GetVector(param).z); break;
                case 4: slider->SetValue(this->refElement->GetVector(param).w); break;
            }
        }
        else n_assert(false);
    }
    else
    {
        slider->SetMinValue(0);
        slider->SetMaxValue(0);
        slider->SetKnobSize(1);
        slider->SetValue(0);
    }
}
//------------------------------------------------------------------------------
/**

*/
void
nGuiSkyEditor::UpdateColorSliderFromElement(nGuiColorSliderGroup* slider, nShaderState::Param param, float max)
{
    if (this->refElement->HasParam(param))
    {
        slider->SetMaxIntensity(max);
        slider->SetColor(this->refElement->GetVector(param));
    }
    else
    {
        slider->SetMaxIntensity(max);
        slider->SetColor(vector4(1.0f, 1.0f, 1.0f, 0.0f));
    }
}

//------------------------------------------------------------------------------
/**

*/
void
nGuiSkyEditor::FindSkyNode(nRoot* node)
{
    if (node->IsInstanceOf("nskynode"))
    {
        this->skyPath = node->GetFullName();
        this->refSky.set(this->skyPath.Get());
    }
    else
    {
        nRoot* child = node->GetHead();
        while (child)
        {
            this->FindSkyNode(child);
            child = child->GetSucc();
        }
    }
}