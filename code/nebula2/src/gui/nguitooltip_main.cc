//------------------------------------------------------------------------------
//  nguitooltip_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitooltip.h"
#include "resource/nresourceserver.h"
#include "gfx2/ngfxserver2.h"
#include "gui/nguiserver.h"
#include "kernel/ntimeserver.h"
#include "gui/nguimousecursor.h"

nNebulaClass(nGuiToolTip, "nguitextlabel");

//------------------------------------------------------------------------------
/**
*/
nGuiToolTip::nGuiToolTip():
    openFirstFrame(false),
    fadeinRequested(false),
    fadeinRequestTime(0.0),
    windowColor(1.0f, 1.0f, 1.0f, 1.0f),
    textSizeIsDirty(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiToolTip::~nGuiToolTip()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    On becoming visible, the tooltip positions itself relative to the
    mouse and resizes itself according to it's text.
*/
void
nGuiToolTip::OnShow()
{
    this->UpdateRect();

    this->openFirstFrame = true;
    this->fadeinRequested = true;

    nGuiTextLabel::OnShow();
}

//------------------------------------------------------------------------------
/**
    Update the tooltips position when the mouse moves.
*/
bool
nGuiToolTip::OnMouseMoved(const vector2& mousePos)
{
    this->UpdateRect();
    return nGuiTextLabel::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiToolTip::SetText(const char* text)
{    
    nGuiTextLabel::SetText(text);
    this->textSizeIsDirty = true;
    this->UpdateRect();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiToolTip::SetFont(const char* fontName)
{
    nGuiTextLabel::SetFont(fontName);
    this->textSizeIsDirty = true;
    this->UpdateRect();
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiToolTip::Render()
{     
    if (this->IsShown())
    {
        nGuiServer* guiServer = nGuiServer::Instance();

        this->UpdateColor();

        vector4 globalColor = guiServer->GetGlobalColor();
        guiServer->SetGlobalColor(this->windowColor);

        // take opened time stamp AFTER first rendering to take resource
        // loading delays into account
        if (this->openFirstFrame)
        {
            // NOTE: the use on nTimeServer is intentional, as the current
            // frame time stamp is useless (would give fade delays when
            // resources are loaded)
            this->fadeinRequestTime = nTimeServer::Instance()->GetTime();
            this->openFirstFrame = false;
        }

        nGuiTextLabel::Render();

        // restore previous global color
        guiServer->SetGlobalColor(globalColor);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    This computes the color (takes fade in and fade out effect into account).
*/
void
nGuiToolTip::UpdateColor()
{    
    this->windowColor = vector4(1.0f, 1.0f, 1.0f, 1.0f);

    // NOTE: the use on nTimeServer is intentional, as the current
    // frame time stamp is useless (would give fade delays when
    // resources are loaded)
    nTime time = nTimeServer::Instance()->GetTime();
    nTime fadeInTime = nGuiServer::Instance()->GetToolTipFadeInTime();

    // to obscure resource loading delays, the actual fadein snapshot
    // is only taken after the first rendering (where resources
    // are demand-loaded)
    if (this->openFirstFrame && (fadeInTime > 0.0f))
    {
        // window is always invisible in first frame
        this->windowColor.w = 0.0f;
    }
    else if (this->fadeinRequested)
    {
        // fade in?
        if ((fadeInTime > 0.01f) && (time < (this->fadeinRequestTime + fadeInTime)))
        {
            // fade alpha
            float lerp = n_saturate((float) ((time - this->fadeinRequestTime) / fadeInTime));
            this->windowColor.w *= lerp;
        }
        else
        {
            fadeinRequested = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiToolTip::UpdateRect()
{
    // get current mouse pos directly from input server
    const vector2& mousePos = nGuiServer::Instance()->GetMousePos();

    // (re-)validate the font object
    if (!this->refFont.isvalid())
    {
        this->refFont = (nFont2*) nResourceServer::Instance()->FindResource(this->fontName.Get(), nResource::Font);
        if (!this->refFont.isvalid())
        {
            n_error("nGuiTextLabel %s: Unknown font '%s'!", this->GetName(), this->fontName.Get()); 
        }
    }
    
    if (this->textSizeIsDirty)
    {
        if (this->refFont.isvalid())
        {
            // get text size
            nGfxServer2::Instance()->SetFont(this->refFont.get());
            this->textSize = nGfxServer2::Instance()->GetTextExtent(this->GetText()) + this->border * 2.0f;
            this->textSize.x += 0.0025f; // some extra space

            this->textSizeIsDirty = false;
        }
    }

    // compute tooltip offset to mouse hot spot
    // we assume a 32x32 mouse pointer
    const nDisplayMode2& mode = nGfxServer2::Instance()->GetDisplayMode();
    vector2 cursorSize((32.0f / mode.GetWidth()) ,(32.0f / mode.GetHeight()));
    if (nGfxServer2::Instance()->GetCursorVisibility() == nGfxServer2::Gui)
    {
        nGuiMouseCursor* cursor = nGuiServer::Instance()->GetGuiMouseCursor();
        if (cursor)
        {
            cursorSize = cursor->GetBrushSize();
        }
    }
    vector2 offset(0.0f, cursorSize.y);

    // update screen space rectangle
    rectangle r(mousePos + offset, mousePos + this->textSize + offset);
    nGuiServer::Instance()->MoveRectToVisibleArea(r);
    
    this->SetRect(r);
}

