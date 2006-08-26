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
#include "input/ninputserver.h"

nNebulaScriptClass(nGuiTextView, "nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiTextView::nGuiTextView() :
    fontName("GuiSmall"),
    lineHeight(0.0),
    border(0.005f),
    textColor(0.0f, 0.0f, 0.0f, 1.0f),
    textArray(32, 32),
    selectionEnabled(false),
    lookupEnabled(false),
    lineOffset(0),
    selectionIndex(0),
    calculateTextAlwaysWithSlider(false)
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
    slider->Show();
    this->UpdateSliderVisibility();

    kernelServer->PopCwd();

    // register as event listener because we need to listen for events from our slider
    nGuiServer::Instance()->RegisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextView::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);
    this->ClearAttachRules();
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
        this->refFont = (nFont2*) nResourceServer::Instance()->FindResource(this->fontName.Get(), nResource::Font);
        if (!this->refFont.isvalid())
        {
            n_error("nGuiTextView %s: Unknown font '%s'!", this->GetName(), this->fontName.Get());
        }
        else
        {
            this->refFont->AddRef();
            nGfxServer2::Instance()->SetFont(this->refFont.get());
            this->lineHeight = nGfxServer2::Instance()->GetTextExtent("X").y;
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
        if (this->refSlider.isvalid()  && this->refSlider->IsShown())
        {
            this->refSlider->Hide();
        }
    }
    else
    {
        if (this->refSlider.isvalid()  && !this->refSlider->IsShown())
        {
            this->refSlider->Show();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Check if full range of text is visible, and show/hide slider.

    23-Nov-04   floh    removed the HasFocus(), this doesn't make sense here...
*/
void
nGuiTextView::OnFrame()
{
    // check for mousewheel movement
    if (this->Inside(nGuiServer::Instance()->GetMousePos()))
    {
        if (nInputServer::Instance()->GetButton("ScrollUp"))
        {
            this->ScrollUp(3);
        }
        else if (nInputServer::Instance()->GetButton("ScrollDown"))
        {
            this->ScrollDown(3);
        }
    }
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
        this->lineOffset = n_frnd(this->refSlider->GetVisibleRangeStart());
        // make sure this is never smaller than 0
        if (this->lineOffset < 0)
        {
            this->lineOffset = 0;
        }
    }

    // check for doubble click into the text
    if (event.GetType() == nGuiEvent::DoubleClick)
    {
        if (!this->refSlider->IsShown() || !this->refSlider->Inside(nGuiServer::Instance()->GetMousePos()))
        {
            int relLineIndex = int((nGuiServer::Instance()->GetMousePos().y - this->GetScreenSpaceRect().v0.y) / this->lineHeight);
            int absLineIndex = this->lineOffset + relLineIndex;

            if (this->selectionIndex == absLineIndex)
            {
                // only react on double clicks that do not mean the slider.
                // throw a SelectionDblClicked message
                nGuiEvent event(this, nGuiEvent::SelectionDblClicked);
                nGuiServer::Instance()->PutEvent(event);
            }
        }
    }
    nGuiFormLayout::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    If selection is enabled, the selection index will be updated with the
    entry under the mouse, and a SelectionChanged event will be thrown.
*/
bool
nGuiTextView::OnKeyDown(nKey key)
{
    vector2 mousePos = nGuiServer::Instance()->GetMousePos();

    if (this->IsShown() && this->Inside(mousePos))
    {
        if (this->GetSelectionEnabled())
        {
            // if slider is visible we ignore any clicks in the slider area

            if (this->refSlider->IsShown())
            {
                if (this->refSlider->Inside(mousePos))
                {
                    // click happened over slider, ignore Keys
                    return true;
                }
            }

            // Scroll Up and Down

            if (key == N_KEY_UP)
            {
                if ( this->selectionIndex < 1 )
                {
                    return true;
                }

                ScrollDown(1);
                this->selectionIndex--;
                // throw a SelectionChanged message
                nGuiEvent event(this, nGuiEvent::SelectionChanged);
                nGuiServer::Instance()->PutEvent(event);
                return true;
            }
            if (key == N_KEY_DOWN)
            {
                if ( this->selectionIndex >= this->GetNumLines()-1 )
                {
                    return true;
                }

                ScrollUp(1);
                this->selectionIndex++;
                // throw a SelectionChanged message
                nGuiEvent event(this, nGuiEvent::SelectionChanged);
                nGuiServer::Instance()->PutEvent(event);
                return true;
            }

            // LookUp starts here

            if ( ! this->GetLookUpEnabled() ) return true;
            if (key > N_KEY_Z) return true;
            if (key < N_KEY_1) return true;

            int index=0;

            if (key >= N_KEY_A)
            {
                for ( index=0; index<this->GetNumLines() ;index++)
                {
                    if(textArray[index].Length() > 0)
                    {
                        if(textArray[index][0] >= ('a'+key-N_KEY_A) ) break;
                    }
                }
            }

            int absLineIndex = index;
            if (absLineIndex < this->GetNumLines())
            {
                int delta = absLineIndex - this->selectionIndex ;

                if (delta > 0)
                {
                    ScrollUp(delta);
                }
                else if (delta < 0)
                {
                    ScrollDown(-delta);
                }

                this->selectionIndex = absLineIndex;

                // throw a SelectionChanged message
                nGuiEvent event(this, nGuiEvent::SelectionChanged);
                nGuiServer::Instance()->PutEvent(event);
            }
        }
        return true;//nGuiFormLayout::OnButtonDown(mousePos);
    }
    return false;
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
                    nGuiServer::Instance()->PutEvent(event);
                }
            }
        }
        return nGuiFormLayout::OnButtonDown(mousePos);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Scroll up one line.
*/
void
nGuiTextView::ScrollUp(int numLines)
{
    n_assert(numLines > 0);
    this->lineOffset += numLines;
    if ((this->lineOffset + this->GetNumVisibleLines()) > this->GetNumLines())
    {
        this->lineOffset = this->GetNumLines() - this->GetNumVisibleLines();
        if (this->lineOffset < 0)
        {
            this->lineOffset = 0;
        }
    }
    this->UpdateSliderValues();
}

//------------------------------------------------------------------------------
/**
    Scroll down one line.
*/
void
nGuiTextView::ScrollDown(int numLines)
{
    n_assert(numLines > 0);
    this->lineOffset -= numLines;
    if (this->lineOffset < 0)
    {
        this->lineOffset = 0;
    }
    this->UpdateSliderValues();
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
        nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);

        n_assert(this->textArray.Size() == this->colorArray.Size());

        int beginIndex = this->lineOffset;
        int endIndex = beginIndex + this->GetNumVisibleLines();
        if (endIndex > this->textArray.Size())
        {
            endIndex = this->textArray.Size();
        }
        if ((endIndex - beginIndex) > 0)
        {
            nGfxServer2* gfxServer = nGfxServer2::Instance();
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
                    nGuiServer::Instance()->DrawBrush(curRect, this->highlightBrush);
                }
                nGuiServer::Instance()->DrawText(this->textArray[i].Get(), this->colorArray[i], curTextRect, renderFlags);
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

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextView::ActivateFont()
{
    n_assert(this->refFont.isvalid());
    nGfxServer2::Instance()->SetFont(this->refFont.get());
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextView::AppendColoredText(const nString& text, const vector4& color)
{
    if (text.IsEmpty())
    {
        // just add a empty line
        this->AppendEmptyLine();
        return;
    }

    // get clipping rect
    rectangle rect = this->GetRect();
    float border = this->GetBorder();
    rect.v0.x += border;
    rect.v0.y += border;
    rect.v1.y -= border;

    if (this->refSlider.isvalid() && this->refSlider->IsShown() || this->calculateTextAlwaysWithSlider)
    {
        // add slider and border
        if (this->refSlider.isvalid())
        {
            // show slider if needed
            bool hideSlider = false;
            if (!this->refSlider->IsShown())
            {
                hideSlider = true;
                this->refSlider->Show();
            }
            this->OnRectChange(this->GetRect());

            const rectangle& sliderRect = this->refSlider->GetRect();

            rect.v1.x = sliderRect.v0.x - border;
            if (rect.v1.x < rect.v0.x)
            {
                rect.v1.x = rect.v0.x;
            }

            // hide slider if shown by us
            if (hideSlider)
            {
                this->refSlider->Hide();
            }
            this->OnRectChange(this->GetRect());
        }
    }
    else
    {
        rect.v1.x -= border;
    }

    // break lines
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    this->ActivateFont();
    nString newText;
    gfxServer->BreakLines(text, rect, newText);

    // extract lines
    nArray<nString> lines;
    newText.Tokenize("\n", lines);

    // append lines
    int i;
    for (i = 0; i < lines.Size(); i++)
    {
        this->AppendColoredLine(lines[i], color);
    }
}
