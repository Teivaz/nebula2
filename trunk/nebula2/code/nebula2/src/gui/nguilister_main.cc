//------------------------------------------------------------------------------
//  nguilister_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguilister.h"
#include "kernel/ndirectory.h"
#include "util/npathstring.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiLister, "nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiLister::nGuiLister() :
    firstIndex(0),
    selectedIndex(0),
    textEntryWidget(0),
    sliderWidget(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiLister::~nGuiLister()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/

void
nGuiLister::Clear()
{
    textArray.Clear();

    this->sliderWidget->SetRange(0.0f, 1.0f);

    int labelIndex = 0;
    int numLabels = this->labelArray.Size();
    for (labelIndex = 0; labelIndex < numLabels; labelIndex++)
    {
        this->labelArray[labelIndex]->SetText("");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiLister::AppendText(const char* text)
{
    this->textArray.Append(nString(text));

    if (this->labelArray.Size() < this->textArray.Size())
    {
        // update slider length and position
        this->sliderWidget->SetStepSize(1.0f / this->textArray.Size());

        float length = float(this->labelArray.Size()) / float(this->textArray.Size());

        // map current first index to slider position
        float rangeMin = (float)this->firstIndex / this->textArray.Size();
        this->sliderWidget->SetRange(rangeMin, rangeMin + length);
    }
    // else: max range remains 1.0 and step size 0.0

    this->UpdateTextLabels();
}

//------------------------------------------------------------------------------
/**
*/
const char*
nGuiLister::GetText() const
{
    if (this->textEntryWidget)
    {
        return this->textEntryWidget->GetText();
    }
    else
    {
        return this->labelArray[selectedIndex]->GetText();
    }
}

//------------------------------------------------------------------------------
/**
    Update the text labels (the "list view") with the text items in textArray.
*/

void
nGuiLister::UpdateTextLabels()
{
    int labelIndex = 0;
    int numLabels = this->labelArray.Size();
    for (labelIndex = 0; labelIndex < numLabels; labelIndex++)
    {
        int textIndex = this->firstIndex + labelIndex;
        if (textIndex < this->textArray.Size())
        {
            this->labelArray[labelIndex]->SetText(this->textArray[textIndex].Get());
        }
        else
        {
            this->labelArray[labelIndex]->SetText("");
        }
    }
}

//------------------------------------------------------------------------------
/**
    Gets the current slider values and updates the text labels accordingly.
*/
void
nGuiLister::UpdateTextLabelsFromSlider()
{
    n_assert(this->sliderWidget);
    float rangeMin, rangeMax;
    this->sliderWidget->GetRange(rangeMin, rangeMax);

    // convert the relative range into an index into the text array
    this->firstIndex = int(this->textArray.Size() * rangeMin);

    // update the text labels
    this->UpdateTextLabels();
}

//------------------------------------------------------------------------------
/**
    This initializes the child widgets.
*/
void
nGuiLister::OnShow()
{
    // clear arrays
    this->textArray.Clear();
    this->labelArray.Clear();

    // initialize the child widget pointers
    nClass* guiTextEntryClass = kernelServer->FindClass("nguitextentry");
    nClass* guiSliderClass    = kernelServer->FindClass("nguislider");
    nClass* guiTextLabelClass = kernelServer->FindClass("nguitextlabel");
    n_assert(guiTextEntryClass);
    n_assert(guiSliderClass);
    n_assert(guiTextLabelClass);
    nGuiWidget* child;

    this->textEntryWidget = 0;
    for (child = (nGuiWidget*) this->GetHead(); child; child = (nGuiWidget*) child->GetSucc())
    {
        if (child->IsInstanceOf(guiTextEntryClass))
        {
            this->textEntryWidget = (nGuiTextEntry*) child;
        }
        else if (child->IsInstanceOf(guiSliderClass))
        {
            this->sliderWidget = (nGuiSlider*) child;
        }
        else if (child->IsInstanceOf(guiTextLabelClass))
        {
            this->labelArray.Append((nGuiTextLabel*)child);
        }
    }
    n_assert(this->sliderWidget);

    this->firstIndex = 0;
    this->selectedIndex = 0;

    // initialize the child widgets
    rectangle listRect = this->GetRect();
    float listWidth  = listRect.v1.x - listRect.v0.x;
    float listHeight = listRect.v1.y - listRect.v0.y;
    float textWidgetHeight = listHeight / (this->labelArray.Size() + 1);
    float sliderWidth  = listWidth * 0.05f;
    float textWidgetWidth = listWidth - sliderWidth;

    int labelIndex;
    int numTextLabels = this->labelArray.Size();
    for (labelIndex = 0; labelIndex < numTextLabels; labelIndex++)
    {
        nGuiTextLabel* label = this->labelArray[labelIndex];
        rectangle labelRect(vector2(0.0f, labelIndex * textWidgetHeight), 
                            vector2(textWidgetWidth, labelIndex * textWidgetHeight + textWidgetHeight));
        label->SetRect(labelRect);
    }

    if (this->textEntryWidget)
    {
        rectangle entryRect(vector2(0.0f, listHeight - textWidgetHeight), vector2(textWidgetWidth, listHeight));
        this->textEntryWidget->SetRect(entryRect);
    }
    rectangle sliderRect(vector2(listWidth - sliderWidth, 0.0f), vector2(listWidth, listHeight - textWidgetHeight));
    this->sliderWidget->SetRect(sliderRect);
    this->sliderWidget->SetRange(0.0f, 1.0f);
    this->sliderWidget->SetStepSize(0.0f);

    // compute render position for text list background
    this->textListRect.set(vector2(0.0f, 0.0f), vector2(listWidth - sliderWidth, listHeight - textWidgetHeight));

    nGuiWidget::OnShow();
}

//------------------------------------------------------------------------------
/**
    Handle mouse move.
*/
bool
nGuiLister::OnMouseMoved(const vector2& mousePos)
{
    bool mouseOverWidget = false;

    if (this->IsShown())
    {
        // record current mouse pos
        this->curMousePos = mousePos;

        // distribute to child widgets
        nGuiWidget* widget;
        for (widget = (nGuiWidget*) this->GetHead(); widget; widget = (nGuiWidget*) widget->GetSucc())
        {
            mouseOverWidget |= widget->OnMouseMoved(mousePos);
        }
        this->UpdateTextLabelsFromSlider();
    }
    return mouseOverWidget;
}

//------------------------------------------------------------------------------
/**
    Handle Button Down. If the mouse is over one of the text label widgets,
    the text entry widget will be updated with the filename.
*/
bool
nGuiLister::OnButtonDown(const vector2& mousePos)
{
    int numTextLabels = this->labelArray.Size();
    int labelIndex;
    for (labelIndex = 0; labelIndex < numTextLabels; labelIndex++)
    {
        nGuiTextLabel* label = this->labelArray[labelIndex];
        if (label->Inside(this->curMousePos))
        {
            const char* text = label->GetText();
            if (text[0] != 0)
            {
                selectedIndex = this->firstIndex + labelIndex;

                if (this->textEntryWidget)
                {
                    this->textEntryWidget->SetText(text);
                }
            }
        }
    }

    // distribute Button Down to all child widgets
    bool retval = nGuiWidget::OnButtonDown(mousePos);
    this->UpdateTextLabelsFromSlider();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Render the whole lister widget.
*/
bool
nGuiLister::Render()
{
    if (this->IsShown())
    {
        // render the background for the text lister
        this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(this->textListRect), this->GetDefaultBrush());

        // render the selection resource if there're any text entries and if it's visible
        if (textArray.Size() != 0 && (selectedIndex >= firstIndex))
        {
            // index of corresponding text label
            int labelIndex = selectedIndex - firstIndex;
            this->refGuiServer->DrawBrush(this->labelArray[labelIndex]->GetScreenSpaceRect(), this->GetCursorBrush());
        }

        // let each child widget render itself
        nGuiWidget* widget;
        for (widget = (nGuiWidget*) this->GetHead(); widget; widget = (nGuiWidget*) widget->GetSucc())
        {
            widget->Render();
        }    
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render audio effects.
*/
bool
nGuiLister::RenderAudio()
{
    if (this->IsShown())
    {
        // let each child widget render itself
        nGuiWidget* widget;
        for (widget = (nGuiWidget*) this->GetHead(); widget; widget = (nGuiWidget*) widget->GetSucc())
        {
            widget->RenderAudio();
        }    
        return true;
    }
    return false;
}
