//------------------------------------------------------------------------------
//  nguihorislidergroup_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguihorislidergroup.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"

nNebulaScriptClass(nGuiHoriSliderGroup, "nguiformlayout");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nguihorislidergroup

    @cppclass
    nGuiHoriSliderGroup
    
    @superclass
    nguiformlayout
    
    @classinfo
    Docs needed.
*/

//------------------------------------------------------------------------------
/**
*/
nGuiHoriSliderGroup::nGuiHoriSliderGroup() :
    labelFont("GuiSmall"),
    minValue(0),
    maxValue(10),
    curValue(0),
    knobSize(1),
    leftWidth(0.2f),
    rightWidth(0.1f)
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
    nGuiSkin* skin = this->refGuiServer->GetSkin();
    n_assert(skin);

    nGuiFormLayout::OnShow();
    kernelServer->PushCwd(this);

    // create slider widget
    nGuiSlider2* slider = (nGuiSlider2*) kernelServer->New("nguislider2", "Slider");
    n_assert(slider);
    slider->SetRangeSize(float((this->maxValue - this->minValue) + this->knobSize));
    slider->SetVisibleRangeStart(float(this->curValue - this->minValue));
    slider->SetVisibleRangeSize(float(this->knobSize));
    slider->SetHorizontal(true);
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
    leftLabel->SetAlignment(nGuiTextLabel::Right);
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
    rightLabel->SetAlignment(nGuiTextLabel::Left);
    rightLabel->SetColor(skin->GetLabelTextColor());
    rightLabel->SetMinSize(vector2(0.0f, sliderMinSize.y));
    rightLabel->SetMaxSize(vector2(1.0f, sliderMaxSize.y));
    this->AttachForm(rightLabel, Top, 0.0f);
    this->AttachForm(rightLabel, Right, 0.0f);
    this->AttachPos(rightLabel, Left, 1.0f - this->rightWidth);
    rightLabel->OnShow();
    this->refRightLabel = rightLabel;

    kernelServer->PopCwd();

    this->SetMinSize(sliderMinSize);
    this->SetMaxSize(sliderMaxSize);

    this->UpdateLayout(this->rect);
    this->refGuiServer->RegisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHoriSliderGroup::OnHide()
{
    this->refGuiServer->UnregisterEventListener(this);

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

    // clear the form layout attachments
    this->ClearAttachments();

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
        this->curValue = int(this->refSlider->GetVisibleRangeStart());

        // update left and right label formatted strings
        char buf[1024];
        snprintf(buf, sizeof(buf), this->leftText.Get(), this->curValue);
        this->refLeftLabel->SetText(buf);
        snprintf(buf, sizeof(buf), this->rightText.Get(), this->curValue);
        this->refRightLabel->SetText(buf);
    }
    nGuiFormLayout::OnFrame();
}
