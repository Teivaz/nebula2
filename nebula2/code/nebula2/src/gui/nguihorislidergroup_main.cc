//------------------------------------------------------------------------------
//  nguihorislidergroup_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguihorislidergroup.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"

nNebulaScriptClass(nGuiHoriSliderGroup, "nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiHoriSliderGroup::nGuiHoriSliderGroup() :
    labelFont("GuiSmall"),
    displayFormat(Int),
    minValue(0.0f),
    maxValue(10.0f),
    curValue(0.0f),
    knobSize(1.0f),
    increment(1.0f),
    leftWidth(0.2f),
    rightWidth(0.1f),
    snapToIncrement(false),
    leftTextAlignment(nGuiTextLabel::Right),
    rightTextAlignment(nGuiTextLabel::Left)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiHoriSliderGroup::~nGuiHoriSliderGroup()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHoriSliderGroup::OnShow()
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    nGuiFormLayout::OnShow();
    kernelServer->PushCwd(this);

    // create slider widget
    nGuiSlider2* slider = (nGuiSlider2*) kernelServer->New("nguislider2", "Slider");
    n_assert(slider);
    slider->SetRangeSize((this->maxValue - this->minValue) + this->knobSize);
    slider->SetVisibleRangeStart(this->curValue - this->minValue);
    slider->SetVisibleRangeSize(this->knobSize);
    slider->SetIncrement(this->increment);
    slider->SetHorizontal(true);
    slider->SetSnapToIncrement(false);
    this->AttachForm(slider, Top, 0.0f);
    this->AttachPos(slider, Left, this->leftWidth);
    this->AttachPos(slider, Right, 1.0f - this->rightWidth);
    slider->OnShow();
    this->refSlider = slider;
    const vector2& sliderMinSize = slider->GetMinSize();
    const vector2& sliderMaxSize = slider->GetMaxSize();

    // create left text label
    nGuiTextLabel* leftLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "LeftLabel");
    n_assert(leftLabel);
    leftLabel->SetText(this->leftText.Get());
    leftLabel->SetFont(this->GetLabelFont());
    leftLabel->SetAlignment(leftTextAlignment);
    leftLabel->SetColor(skin->GetLabelTextColor());
    leftLabel->SetMinSize(vector2(0.0f, sliderMinSize.y));
    leftLabel->SetMaxSize(vector2(1.0f, sliderMaxSize.y));
    this->AttachForm(leftLabel, Top, 0.0f);
    this->AttachForm(leftLabel, Left, 0.0f);
    this->AttachPos(leftLabel, Right, this->leftWidth);
    leftLabel->OnShow();
    this->refLeftLabel = leftLabel;

    // create right text label
    nGuiTextLabel* rightLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "RightLabel");
    n_assert(rightLabel);
    rightLabel->SetText(this->rightText.Get());
    rightLabel->SetFont(this->GetLabelFont());
    rightLabel->SetAlignment(rightTextAlignment);
    rightLabel->SetColor(skin->GetLabelTextColor());
    rightLabel->SetMinSize(vector2(0.0f, sliderMinSize.y));
    rightLabel->SetMaxSize(vector2(1.0f, sliderMaxSize.y));
    this->AttachForm(rightLabel, Top, 0.0f);
    this->AttachForm(rightLabel, Right, 0.0f);
    this->AttachPos(rightLabel, Left, 1.0f - this->rightWidth);
    rightLabel->OnShow();
    this->refRightLabel = rightLabel;

    kernelServer->PopCwd();

    this->SetMinSize(vector2(this->GetMinSize().x,sliderMinSize.y));
    this->SetMaxSize(vector2(this->GetMaxSize().x,sliderMaxSize.y));

    this->UpdateLayout(this->rect);
    nGuiServer::Instance()->RegisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHoriSliderGroup::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);

    this->ClearAttachRules();
    if (this->refSlider.isvalid())
    {
        this->refSlider->Release();
        n_assert(!this->refSlider.isvalid());
    }
    if (this->refLeftLabel.isvalid())
    {
        this->refLeftLabel->Release();
        n_assert(!this->refLeftLabel.isvalid());
    }
    if (this->refRightLabel.isvalid())
    {
        this->refRightLabel->Release();
        n_assert(!this->refRightLabel.isvalid());
    }

    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHoriSliderGroup::OnFrame()
{
    if (this->refSlider.isvalid())
    {
        this->curValue = this->minValue + this->refSlider->GetVisibleRangeStart();

        // update left and right label formatted strings
        char buf[1024];
        if (Int == this->displayFormat)
        {
            snprintf(buf, sizeof(buf), this->leftText.Get(), int(this->curValue));
            this->refLeftLabel->SetText(buf);
            snprintf(buf, sizeof(buf), this->rightText.Get(), int(this->curValue));
            this->refRightLabel->SetText(buf);
        }
        else
        {
            snprintf(buf, sizeof(buf), this->leftText.Get(), this->curValue);
            this->refLeftLabel->SetText(buf);
            snprintf(buf, sizeof(buf), this->rightText.Get(), this->curValue);
            this->refRightLabel->SetText(buf);
        }
    }
    nGuiFormLayout::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHoriSliderGroup::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::SliderChanged)
    {
        if (this->refSlider.isvalid() && (event.GetWidget() == this->refSlider))
        {
            // replicate event
            nGuiEvent event(this, nGuiEvent::SliderChanged);
            nGuiServer::Instance()->PutEvent(event);
        }
    }
}

//------------------------------------------------------------------------------
/**

*/
nGuiSlider2*
nGuiHoriSliderGroup::GetSlider() const
{
    return this->refSlider.get_unsafe();
}
