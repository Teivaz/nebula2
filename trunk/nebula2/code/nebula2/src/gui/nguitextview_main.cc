//------------------------------------------------------------------------------
//  nguitextview_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextview.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nfont2.h"
#include "resource/nresourceserver.h"
#include "gui/nguislider2.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiTextView, "nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiTextView::nGuiTextView() :
    refGfxServer("/sys/servers/gfx"),
    refResourceServer("/sys/servers/resource"),
    fontName("GuiSmall"),
    lineHeight(0.0),
    border(0.005f),
    textColor(0.0f, 0.0f, 0.0f, 1.0f),
    textArray(32, 32),
    selectionEnabled(false),
    lineOffset(0),
    selectionIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTextView::~nGuiTextView()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextView::OnShow()
{
    nGuiFormLayout::OnShow();

    kernelServer->PushCwd(this);

    // create the slider widget
    nGuiSlider2* slider = (nGuiSlider2*) kernelServer->New("nguislider2", "Slider");
    n_assert(slider);
    this->refSlider = slider;
    this->AttachForm(slider, Top, 0.0f);
    this->AttachForm(slider, Right, 0.0f);
    this->AttachForm(slider, Bottom, 0.0f);
    this->UpdateSliderValues();
    slider->OnShow();
    this->UpdateSliderVisibility();

    kernelServer->PopCwd();

    // register as event listener because we need to listen for events from our slider
    this->refGuiServer->RegisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextView::OnHide()
{
    this->refGuiServer->UnregisterEventListener(this);
    if (this->refSlider.isvalid())
    {
        this->refSlider->Release();
        n_assert(!this->refSlider.isvalid());
    }
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextView::OnRectChange(const rectangle& newRect)
{
    nGuiFormLayout::OnRectChange(newRect);
    if (this->IsShown())
    {
        // need to inform slider that number of visible lines may have changed
        this->UpdateSliderValues();
    }
}

//------------------------------------------------------------------------------
/**
    This validates the font resource if not happened yet.
*/
void
nGuiTextView::ValidateFont()
{
    if (!this->refFont.isvalid())
    {
        this->refFont = (nFont2*) this->refResourceServer->FindResource(this->fontName.Get(), nResource::Font);
        if (!this->refFont.isvalid())
        {
            n_error("nGuiTextView %s: Unknown font '%s'!", this->GetName(), this->fontName.Get()); 
        }
        else
        {
            this->refFont->AddRef();
            this->refGfxServer->SetFont(this->refFont.get());
            this->lineHeight = this->refGfxServer->GetTextExtent("X").y;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Compute the number of lines that would fully fit into this widget.
*/
int
nGuiTextView::GetNumVisibleLines()
{
    this->ValidateFont();
    return int(this->rect.height() / this->lineHeight);
}

//------------------------------------------------------------------------------
/**
    Show or hide the vertical slider, depending on number of visible
    lines versus overall number of lines.
*/
void
nGuiTextView::UpdateSliderVisibility()
{
    int numVisibleLines = this->GetNumVisibleLines();
    int numLines = this->textArray.Size();
    if (numVisibleLines >= numLines)
    {
        if (this->refSlider->IsShown())
        {
            this->refSlider->Hide();
        }
    }
    else
    {
        if (!this->refSlider->IsShown())
        {
            this->refSlider->Show();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Check if full range of text is visible, and show/hide slider.
*/
void
nGuiTextView::OnFrame()
{
    this->UpdateSliderVisibility();
    nGuiFormLayout::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Listen to events from our slider.     
*/
void
nGuiTextView::OnEvent(const nGuiEvent& event)
{
    if ((event.GetWidget() == this->refSlider.get()) &&
        (event.GetType() == nGuiEvent::SliderChanged))
    {
        this->lineOffset = int(this->refSlider->GetVisibleRangeStart());
    }
    nGuiFormLayout::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    If selection is enabled, the selection index will be updated with the
    entry under the mouse, and a SelectionChanged event will be thrown.
*/
bool
nGuiTextView::OnButtonDown(const vector2& mousePos)
{
    if (this->IsShown() && this->Inside(mousePos))
    {
        if (this->GetSelectionEnabled())
        {
            // if slider is visible we ignore any clicks in the slider area
            bool ignoreClick = false;
            if (this->refSlider->IsShown())
            {
                if (this->refSlider->Inside(mousePos))
                {
                    // click happened over slider, ignore
                    ignoreClick = true;
                }
            }
            if (!ignoreClick)
            {
                int relLineIndex = int((mousePos.y - this->GetScreenSpaceRect().v0.y) / this->lineHeight);
                int absLineIndex = this->lineOffset + relLineIndex;
                if (absLineIndex < this->GetNumLines())
                {
                    this->selectionIndex = absLineIndex;

                    // throw a SelectionChanged message
                    nGuiEvent event(this, nGuiEvent::SelectionChanged);
                    this->refGuiServer->PutEvent(event);
                }
            }
        }
        return nGuiFormLayout::OnButtonDown(mousePos);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render the widget.
*/
bool
nGuiTextView::Render()
{
    if (this->IsShown())
    {
        // render background
        this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), this->GetDefaultBrush());

        int beginIndex = this->lineOffset;
        int endIndex = beginIndex + this->GetNumVisibleLines();
        if (endIndex > this->textArray.Size())
        {
            endIndex = this->textArray.Size();
        }
        if ((endIndex - beginIndex) > 0)
        {
            nGfxServer2* gfxServer = this->refGfxServer.get();
            gfxServer->SetFont(this->refFont.get());

            // add some border tolerance
            rectangle screenSpaceRect = this->GetScreenSpaceRect();
            rectangle curRect = screenSpaceRect;            
            curRect.v1.y = curRect.v0.y + this->lineHeight;

            // add border for text
            rectangle curTextRect = curRect;
            curTextRect.v0.x += this->border;
            curTextRect.v1.x -= this->border;

            const int renderFlags = nFont2::Left | nFont2::ExpandTabs;
            int i;
            for (i = beginIndex; i < endIndex; i++)
            {
                // selection?
                if (this->selectionEnabled && (i == this->selectionIndex))
                {
                    this->refGuiServer->DrawBrush(curRect, this->GetHighlightBrush());
                }
                gfxServer->DrawText(this->textArray[i].Get(), this->textColor, curTextRect, renderFlags);
                curRect.v0.y = curRect.v1.y;
                curTextRect.v0.y = curTextRect.v1.y;
                curRect.v1.y += this->lineHeight;
                curTextRect.v1.y += this->lineHeight;
            }
        }
        return nGuiFormLayout::Render();
    }
    return false;
}
