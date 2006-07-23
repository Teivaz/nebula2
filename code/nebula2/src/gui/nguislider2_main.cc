//------------------------------------------------------------------------------
//  nguislider2_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguislider2.h"
#include "gui/nguibutton.h"
#include "gui/nguilabel.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiSlider2, "gui::nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiSlider2::nGuiSlider2() :
    overallSize(10.0f),
    visibleStart(0.0f),
    visibleSize(1.0f),
    increment(1.0f),
    rangeDirty(true),
    knobNegEdgeLayoutIndex(0),
    knobPosEdgeLayoutIndex(0),
    negAreaEdgeLayoutIndex(0),
    posAreaEdgeLayoutIndex(0),
    dragging(false),
    horizontal(false),
    snapToIncrement(false),
    dragVisibleStart(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiSlider2::~nGuiSlider2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlider2::OnShow()
{
    if (this->horizontal)
    {
        this->SetDefaultBrush("sliderhori_bg");
    }
    else
    {
        this->SetDefaultBrush("slidervert_bg");
    }
    nGuiFormLayout::OnShow();
    kernelServer->PushCwd(this);

    // get some brush sizes
    if (this->horizontal)
    {
        this->arrowBtnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("arrowleft_n");
        this->SetMinSize(vector2(2.0f * this->arrowBtnSize.x, this->arrowBtnSize.y));
        this->SetMaxSize(vector2(1.0f, this->arrowBtnSize.y));
    }
    else
    {
        this->arrowBtnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("arrowup_n");
        this->SetMinSize(vector2(this->arrowBtnSize.x, 2.0f * this->arrowBtnSize.y));
        this->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f));
    }

    // create negative arrow button
    nGuiButton* negBtn = (nGuiButton*) kernelServer->New("nguibutton", "NegButton");
    n_assert(negBtn);
    negBtn->SetMinSize(this->arrowBtnSize);
    negBtn->SetMaxSize(this->arrowBtnSize);
    if (this->horizontal)
    {
        negBtn->SetDefaultBrush("arrowleft_n");
        negBtn->SetPressedBrush("arrowleft_p");
        negBtn->SetHighlightBrush("arrowleft_h");
    }
    else
    {
        negBtn->SetDefaultBrush("arrowup_n");
        negBtn->SetPressedBrush("arrowup_p");
        negBtn->SetHighlightBrush("arrowup_h");
    }
    this->AttachForm(negBtn, Top, 0.0f);
    this->AttachForm(negBtn, Left, 0.0f);
    negBtn->OnShow();
    this->refNegButton = negBtn;

    // create positive arrow button
    nGuiButton* posBtn = (nGuiButton*) kernelServer->New("nguibutton", "PosButton");
    n_assert(posBtn);
    posBtn->SetMinSize(this->arrowBtnSize);
    posBtn->SetMaxSize(this->arrowBtnSize);
    if (this->horizontal)
    {
        posBtn->SetDefaultBrush("arrowright_n");
        posBtn->SetPressedBrush("arrowright_p");
        posBtn->SetHighlightBrush("arrowright_h");
        this->AttachForm(posBtn, Top, 0.0f);
        this->AttachForm(posBtn, Right, 0.0f);
    }
    else
    {
        posBtn->SetDefaultBrush("arrowdown_n");
        posBtn->SetPressedBrush("arrowdown_p");
        posBtn->SetHighlightBrush("arrowdown_h");
        this->AttachForm(posBtn, Bottom, 0.0f);
        this->AttachForm(posBtn, Left, 0.0f);
    }
    posBtn->OnShow();
    this->refPosButton = posBtn;

    // The "slidertrack" brush, if present, lies *between* the arrow buttons.
    if (nGuiServer::Instance()->BrushExists("slidertrack"))
    {
        nGuiLabel* backgroundLabel = (nGuiLabel*) kernelServer->New("nguilabel", "BackgroundLabel");
        n_assert(backgroundLabel);
        backgroundLabel->SetDefaultBrush("slidertrack");
        if (this->horizontal)
        {
            backgroundLabel->SetMinSize(vector2(0, this->arrowBtnSize.y));
            backgroundLabel->SetMaxSize(vector2(1.0f - 2*this->arrowBtnSize.x, this->arrowBtnSize.y));
            this->AttachForm(backgroundLabel, Top, 0.0f);
            this->AttachWidget(backgroundLabel, Left, negBtn, 0);
            this->AttachWidget(backgroundLabel, Right, posBtn, 0);
        }
        else
        {
            backgroundLabel->SetMinSize(vector2(this->arrowBtnSize.x, 0));
            backgroundLabel->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f - 2*this->arrowBtnSize.y));
            this->AttachForm(backgroundLabel, Left, 0.0f);
            this->AttachWidget(backgroundLabel, Top, negBtn, 0);
            this->AttachWidget(backgroundLabel, Bottom, posBtn, 0);
        }
        backgroundLabel->OnShow();
        this->refBgLabel = backgroundLabel;
    }

    // create the slider knob
    nGuiButton* knob = (nGuiButton*) kernelServer->New("nguibutton", "Knob");
    n_assert(knob);
    if (this->horizontal)
    {
        knob->SetDefaultBrush("sliderknobhori_n");
        knob->SetPressedBrush("sliderknobhori_p");
        knob->SetHighlightBrush("sliderknobhori_h");
        knob->SetMinSize(vector2(0.0f, this->arrowBtnSize.y));
        knob->SetMaxSize(vector2(1.0f, this->arrowBtnSize.y));
        this->knobNegEdgeLayoutIndex = this->AttachPos(knob, Left, 0.0f);
        this->knobPosEdgeLayoutIndex = this->AttachPos(knob, Right, 0.0f);
        this->AttachForm(knob, Top, 0.0f);
        this->AttachForm(knob, Bottom, 0.0f);
    }
    else
    {
        knob->SetDefaultBrush("sliderknobvert_n");
        knob->SetPressedBrush("sliderknobvert_p");
        knob->SetHighlightBrush("sliderknobvert_h");
        knob->SetMinSize(vector2(this->arrowBtnSize.x, 0.0f));
        knob->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f));
        this->knobNegEdgeLayoutIndex = this->AttachPos(knob, Top, 0.0f);
        this->knobPosEdgeLayoutIndex = this->AttachPos(knob, Bottom, 0.0f);
        this->AttachForm(knob, Left, 0.0f);
        this->AttachForm(knob, Right, 0.0f);
    }
    knob->OnShow();
    this->refKnob = knob;

    // create negative background area button
    nGuiButton* negArea = (nGuiButton*) kernelServer->New("nguibutton", "NegArea");
    n_assert(negArea);
    if (this->horizontal)
    {
        negArea->SetMinSize(vector2(0.0f, this->arrowBtnSize.y));
        negArea->SetMaxSize(vector2(1.0f, this->arrowBtnSize.y));
        this->AttachWidget(negArea, Left, this->refNegButton, 0.0f);
        this->negAreaEdgeLayoutIndex = this->AttachPos(negArea, Right, 0.0f);
        this->AttachForm(negArea, Top, 0.0f);
        this->AttachForm(negArea, Bottom, 0.0f);
    }
    else
    {
        negArea->SetMinSize(vector2(this->arrowBtnSize.x, 0.0f));
        negArea->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f));
        this->AttachWidget(negArea, Top, this->refNegButton, 0.0f);
        this->negAreaEdgeLayoutIndex = this->AttachPos(negArea, Bottom, 0.0f);
        this->AttachForm(negArea, Left, 0.0f);
        this->AttachForm(negArea, Right, 0.0f);
    }
    negArea->OnShow();
    this->refNegArea = negArea;

    // create positive background area button
    nGuiButton* posArea = (nGuiButton*) kernelServer->New("nguibutton", "PosArea");
    n_assert(posArea);
    if (this->horizontal)
    {
        posArea->SetMinSize(vector2(0.0f, this->arrowBtnSize.y));
        posArea->SetMaxSize(vector2(1.0f, this->arrowBtnSize.y));
        this->posAreaEdgeLayoutIndex = this->AttachPos(posArea, Left, 0.0f);
        this->AttachWidget(posArea, Right, this->refPosButton, 0.0f);
        this->AttachForm(posArea, Top, 0.0f);
        this->AttachForm(posArea, Bottom, 0.0f);
    }
    else
    {
        posArea->SetMinSize(vector2(this->arrowBtnSize.x, 0.0f));
        posArea->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f));
        this->posAreaEdgeLayoutIndex = this->AttachPos(posArea, Top, 0.0f);
        this->AttachWidget(posArea, Bottom, this->refPosButton, 0.0f);
        this->AttachForm(posArea, Left, 0.0f);
        this->AttachForm(posArea, Right, 0.0f);
    }
    posArea->OnShow();
    this->refPosArea = posArea;
    kernelServer->PopCwd();
    this->UpdateLayout(this->rect);
    nGuiServer::Instance()->RegisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlider2::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);

    this->ClearAttachRules();
    if (this->refBgLabel.isvalid())
    {
        this->refBgLabel->Release();
        n_assert(!this->refBgLabel.isvalid());
    }
    if (this->refNegButton.isvalid())
    {
        this->refNegButton->Release();
        n_assert(!this->refNegButton.isvalid());
    }
    if (this->refPosButton.isvalid())
    {
        this->refPosButton->Release();
        n_assert(!this->refPosButton.isvalid());
    }
    if (this->refKnob.isvalid())
    {
        this->refKnob->Release();
        n_assert(!this->refKnob.isvalid());
    }
    if (this->refNegArea.isvalid())
    {
        this->refNegArea->Release();
        n_assert(!this->refNegArea.isvalid());
    }
    if (this->refPosArea.isvalid())
    {
        this->refPosArea->Release();
        n_assert(!this->refPosArea.isvalid());
    }

    // call parent class
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlider2::OnRectChange(const rectangle& newRect)
{
    if (this->IsShown())
    {
        this->UpdateKnobLayout(newRect);
    }
    nGuiFormLayout::OnRectChange(newRect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlider2::OnFrame()
{
    // need to change knob layout?
    if (this->IsShown() && this->rangeDirty)
    {
        this->rangeDirty = false;
        this->UpdateKnobLayout(this->rect);
        this->UpdateLayout(this->rect);
    }
    nGuiFormLayout::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Updates the layout attachment offsets of the knob. Must be called
    when any of the range values or the widget's size changes.
*/
void
nGuiSlider2::UpdateKnobLayout(const rectangle& newSliderRect, bool putEvent)
{
    n_assert(this->shown);

    // compute relativ knob size and knob start...
    float relKnobSize = 1.0f;
    float relKnobStart = 0.0f;
    if (this->overallSize > 0.0f)
    {
        relKnobSize  = n_saturate(this->visibleSize / this->overallSize);
        relKnobStart = n_saturate(this->visibleStart / this->overallSize);
        if ((relKnobStart + relKnobSize) > 1.0f)
        {
            relKnobStart = 1.0f - relKnobSize;
            this->visibleStart = this->overallSize * relKnobStart;
        }
        if (relKnobStart < 0.0f)
        {
            relKnobStart = 0.0f;
            this->visibleStart = 0;
        }
    }
    float relKnobEnd = relKnobStart + relKnobSize;

    // Fix the layout offsets of the knob directly in our parent class's
    // layout rules array.
    // This is a wee bit dirty, but it does the job.
    float relArrowBtnSize;
    if (this->horizontal)
    {
        relArrowBtnSize = this->arrowBtnSize.x / newSliderRect.width();
    }
    else
    {
        relArrowBtnSize = this->arrowBtnSize.y / newSliderRect.height();
    }
    float relSliderSize = 1.0f - 2 * relArrowBtnSize;
    float relNegOffset = relArrowBtnSize + (relSliderSize * relKnobStart);
    float relPosOffset = relArrowBtnSize + (relSliderSize * relKnobEnd);
    this->attachRules[this->knobNegEdgeLayoutIndex].offset = relNegOffset;
    this->attachRules[this->knobPosEdgeLayoutIndex].offset = relPosOffset;
    this->attachRules[this->negAreaEdgeLayoutIndex].offset = relNegOffset;
    this->attachRules[this->posAreaEdgeLayoutIndex].offset = relPosOffset;

    // send a slider changed
    if (putEvent)
    {
        nGuiEvent event(this, nGuiEvent::SliderChanged);
        nGuiServer::Instance()->PutEvent(event);
    }
}

//------------------------------------------------------------------------------
/**
    Called when mouse is moved. Handles the knob dragging.
*/
bool
nGuiSlider2::OnMouseMoved(const vector2& mousePos)
{
    if (this->IsShown() && this->dragging)
    {
        this->HandleDrag(mousePos);
    }
    return nGuiFormLayout::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
    Check for child widget events and act accordingly.
*/
void
nGuiSlider2::OnEvent(const nGuiEvent& event)
{
    nGuiFormLayout::OnEvent(event);

    if (event.GetWidget() == this->refKnob)
    {
        if (event.GetType() == nGuiEvent::ButtonDown)
        {
            this->BeginSliderDrag();
        }
    }
    else if (event.GetWidget() == this->refNegArea)
    {
        if (event.GetType() == nGuiEvent::ButtonDown)
        {
            this->MoveSliderNeg(this->visibleSize);
        }
    }
    else if (event.GetWidget() == this->refPosArea)
    {
        if (event.GetType() == nGuiEvent::ButtonDown)
        {
            this->MoveSliderPos(this->visibleSize);
        }
    }
    else if (event.GetWidget() == this->refPosButton)
    {
        if (event.GetType() == nGuiEvent::ButtonUp)
        {
            this->MoveSliderPos(this->increment);
        }
    }
    else if (event.GetWidget() == this->refNegButton)
    {
        if (event.GetType() == nGuiEvent::ButtonUp)
        {
            this->MoveSliderNeg(this->increment);
        }
    }

    if (event.GetType() == nGuiEvent::ButtonUp)
    {
        if (this->dragging)
        {
            this->EndSliderDrag();
        }
    }
    else if (event.GetType() == nGuiEvent::RButtonDown)
    {
        if (this->dragging)
        {
            this->CancelSliderDrag();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render the slider.
*/
bool
nGuiSlider2::Render()
{
    if (this->IsShown())
    {
        // render the background brush
        nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);
        return nGuiFormLayout::Render();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Begin dragging the slider.
*/
void
nGuiSlider2::BeginSliderDrag()
{
    this->dragging          = true;
    this->startDragMousePos = nGuiServer::Instance()->GetMousePos();
    this->dragVisibleStart  = this->visibleStart;
}

//------------------------------------------------------------------------------
/**
    Finish dragging the slider.
*/
void
nGuiSlider2::EndSliderDrag()
{
    n_assert(this->dragging);
    this->dragging = false;
}

//------------------------------------------------------------------------------
/**
    Cancel a knob dragging action.
*/
void
nGuiSlider2::CancelSliderDrag()
{
    n_assert(this->dragging);
    this->dragging = false;
    this->visibleStart = this->dragVisibleStart;
    this->UpdateKnobLayout(this->rect);
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
    Handle a knob dragging action.
*/
void
nGuiSlider2::HandleDrag(const vector2& mousePos)
{
    n_assert(this->dragging);

    // convert screen space drag distance to relative drag distance
    float dragDist;
    float screenSpaceSize;
    if (this->horizontal)
    {
        dragDist = mousePos.x - this->startDragMousePos.x;
        screenSpaceSize = n_saturate(this->rect.width() - 2.0f * this->arrowBtnSize.x);
    }
    else
    {
        dragDist = mousePos.y - this->startDragMousePos.y;
        screenSpaceSize = n_saturate(this->rect.height() - 2.0f * this->arrowBtnSize.y);
    }
    if (screenSpaceSize > 0.0f)
    {
        float relDragDist = dragDist / screenSpaceSize;

        // update range variables
        float newVisibleStart = this->dragVisibleStart + (relDragDist * this->overallSize);
        this->visibleStart = n_clamp(newVisibleStart, 0.0f, this->overallSize - this->visibleSize);
        if (this->snapToIncrement)
        {
            this->SnapToIncrement();
        }
        this->UpdateKnobLayout(this->rect);
        this->UpdateLayout(this->rect);
    }
}

//------------------------------------------------------------------------------
/**
    Move slider one position into the positive direction.
*/
void
nGuiSlider2::MoveSliderPos(float value)
{
    n_assert(value >= 0.0f);
    this->visibleStart += value;
    if (this->snapToIncrement)
    {
        this->SnapToIncrement();
    }
    if ((this->visibleStart + this->visibleSize) >= this->overallSize)
    {
        this->visibleStart = this->overallSize - this->visibleSize;
    }
    this->UpdateKnobLayout(this->rect);
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
    Move slider one position into the negative direction.
*/
void
nGuiSlider2::MoveSliderNeg(float value)
{
    n_assert(value >= 0.0f);
    this->visibleStart -= value;
    if (this->snapToIncrement)
    {
        this->SnapToIncrement();
    }
    if (this->visibleStart < 0)
    {
        this->visibleStart = 0;
    }
    this->UpdateKnobLayout(this->rect);
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
    If snap-to-increment is enabled, snap the visibleStart value to be
    a multiple of the increment member.
*/
void
nGuiSlider2::SnapToIncrement()
{
    n_assert(this->snapToIncrement);
    n_assert(this->increment > 0.0f);
    float remainder = fmodf(this->visibleStart + this->increment * 0.5f, this->increment);
    this->visibleStart -= remainder;
}


