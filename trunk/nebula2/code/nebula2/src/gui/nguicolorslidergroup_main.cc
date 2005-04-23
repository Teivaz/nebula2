//------------------------------------------------------------------------------
//  nguicolorslidergroup_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicolorslidergroup.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiColorSliderGroup, "nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiColorSliderGroup::nGuiColorSliderGroup() :
    labelFont("GuiSmall"),
    textLabelWidth(0.1f),
    intensityLabelWidth(0.1f),
    maxIntensity(1.0f),
    intensityIncrement(0.01f)
{
    this->SetColor(vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

//------------------------------------------------------------------------------
/**
*/
nGuiColorSliderGroup::~nGuiColorSliderGroup()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorSliderGroup::OnShow()
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    nGuiFormLayout::OnShow();
    kernelServer->PushCwd(this);

    // create intensity slider widget
    nGuiSlider2* slider = (nGuiSlider2*) kernelServer->New("nguislider2", "Slider");
    n_assert(slider);
    slider->SetRangeSize(this->maxIntensity + this->maxIntensity * 0.1f);
    slider->SetVisibleRangeStart(this->intensity);
    slider->SetVisibleRangeSize(this->maxIntensity * 0.1f);
    slider->SetHorizontal(true);
    slider->SetIncrement(this->intensityIncrement);
    this->AttachForm(slider, Top, 0.0f);
    this->AttachPos(slider, Left, this->textLabelWidth);
    this->AttachPos(slider, Right, 0.9f - this->intensityLabelWidth);
    slider->OnShow();
    this->refSlider = slider;
    const vector2& sliderMinSize = slider->GetMinSize();
    const vector2& sliderMaxSize = slider->GetMaxSize();

    // create text label
    nGuiTextLabel* textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "TextLabel");
    n_assert(textLabel);
    textLabel->SetText(this->GetLabelText().Get());
    textLabel->SetFont(this->GetLabelFont().Get());
    textLabel->SetAlignment(nGuiTextLabel::Right);
    textLabel->SetColor(skin->GetLabelTextColor());
    textLabel->SetMinSize(vector2(0.0f, sliderMinSize.y));
    textLabel->SetMaxSize(vector2(1.0f, sliderMaxSize.y));
    this->AttachForm(textLabel, Top, 0.0f);
    this->AttachPos(textLabel, Left, 0.0f);
    this->AttachPos(textLabel, Right, this->textLabelWidth);
    textLabel->OnShow();
    this->refTextLabel = textLabel;

    // create intensity text label
    textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "IntensityLabel");
    n_assert(textLabel);
    textLabel->SetText("0.00");
    textLabel->SetFont(this->GetLabelFont().Get());
    textLabel->SetAlignment(nGuiTextLabel::Left);
    textLabel->SetColor(skin->GetLabelTextColor());
    textLabel->SetMinSize(vector2(0.0f, sliderMinSize.y));
    textLabel->SetMaxSize(vector2(1.0f, sliderMaxSize.y));
    this->AttachForm(textLabel, Top, 0.0f);
    this->AttachPos(textLabel, Left, 0.9f - this->intensityLabelWidth);
    this->AttachPos(textLabel, Right, 0.9f);
    textLabel->OnShow();
    this->refIntensityLabel = textLabel;

    // create color label
    nGuiColorLabel* colorLabel = (nGuiColorLabel*) kernelServer->New("nguicolorlabel", "ColorLabel");
    n_assert(colorLabel);
    colorLabel->SetColor(this->color);
    colorLabel->SetMinSize(vector2(0.0f, sliderMinSize.y));
    colorLabel->SetMaxSize(vector2(1.0f, sliderMaxSize.y));
    this->AttachForm(colorLabel, Top, 0.0f);
    this->AttachPos(colorLabel, Left, 0.9f);
    this->AttachPos(colorLabel, Right, 1.0f);
    colorLabel->OnShow();
    this->refColorLabel = colorLabel;

    kernelServer->PopCwd();

    this->SetMinSize(sliderMinSize);
    this->SetMaxSize(sliderMaxSize);

    this->UpdateLayout(this->rect);
    nGuiServer::Instance()->RegisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorSliderGroup::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);

    this->ClearAttachRules();
    if (this->refTextLabel.isvalid())
    {
        this->refTextLabel->Release();
        n_assert(!this->refTextLabel.isvalid());
    }
    if (this->refSlider.isvalid())
    {
        this->refSlider->Release();
        n_assert(!this->refSlider.isvalid());
    }
    if (this->refColorLabel.isvalid())
    {
        this->refColorLabel->Release();
        n_assert(!this->refColorLabel.isvalid());
    }
    if (this->refColorAdjustWindow.isvalid())
    {
        this->refColorAdjustWindow->Release();
        n_assert(!this->refColorAdjustWindow.isvalid());
    }
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorSliderGroup::OnEvent(const nGuiEvent& event)
{
    bool colorChanged = false;
    if ((event.GetWidget() == this->refSlider) && (event.GetType() == nGuiEvent::SliderChanged))
    {
        // get current intensity value
        this->intensity = this->refSlider->GetVisibleRangeStart();

        // update the resulting color
        this->color = ComputeColor(this->normColor, this->intensity);

        colorChanged = true;
    }

    if ((event.GetWidget() == this->refColorLabel) && (event.GetType() == nGuiEvent::ButtonDown))
    {
        // open the color adjust window, if not open yet...
        if (!this->refColorAdjustWindow.isvalid())
        {
            nGuiColorAdjustWindow* w = (nGuiColorAdjustWindow*) nGuiServer::Instance()->NewWindow("nguicoloradjustwindow", false);
            w->Show();
            w->SetColor(this->normColor);
            this->refColorAdjustWindow = w;
        }
    }

    if (this->refColorAdjustWindow.isvalid())
    {
        if ((event.GetWidget() == this->refColorAdjustWindow) && (event.GetType() == nGuiEvent::SliderChanged))
        {
            this->normColor = this->refColorAdjustWindow->GetColor();
            this->color = ComputeColor(this->normColor, this->intensity);
            colorChanged = true;
        }
    }

    nGuiFormLayout::OnEvent(event);

    if (colorChanged)
    {
        nGuiEvent event(this, nGuiEvent::SliderChanged);
        nGuiServer::Instance()->PutEvent(event);
        this->UpdateWidgets();
    }
}

//------------------------------------------------------------------------------
/**
    Update embedded widget when color value has changed.
*/
void
nGuiColorSliderGroup::UpdateWidgets()
{
    if (this->refSlider.isvalid())
    {
        this->refSlider->SetVisibleRangeStart(this->intensity);
    }
    if (this->refColorLabel.isvalid())
    {
        this->refColorLabel->SetColor(this->color);
    }
    if (this->refTextLabel.isvalid())
    {
        char buf[128];
        snprintf(buf, sizeof(buf), "%.2f", this->intensity);
        this->refIntensityLabel->SetText(buf);
    }
}
