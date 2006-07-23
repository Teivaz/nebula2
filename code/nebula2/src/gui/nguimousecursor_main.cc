//------------------------------------------------------------------------------
//  nguimousecursor_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguimousecursor.h"
#include "resource/nresourceserver.h"
#include "gfx2/ngfxserver2.h"
#include "gui/nguiserver.h"
#include "kernel/ntimeserver.h"

nNebulaClass(nGuiMouseCursor, "gui::nguilabel");


//------------------------------------------------------------------------------
/**
*/
nGuiMouseCursor::nGuiMouseCursor():
    openFirstFrame(false),
    fadeinRequested(false),
    fadeinRequestTime(0.0),
    brushSizeIsDirty(true),
    fadeInTime(0.0),
    lockBrushSize(false),
    brushSize(0.3f, 0.4f),
    hotSpot(0.0f, 0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiMouseCursor::~nGuiMouseCursor()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    On becoming visible, the tooltip positions itself relative to the
    mouse and resizes itself according to it's text.
*/
void
nGuiMouseCursor::OnShow()
{
    this->UpdateRect();
    this->openFirstFrame = true;
    this->fadeinRequested = true;

    nGuiLabel::OnShow();
}

//------------------------------------------------------------------------------
/**
    Update the tooltips position when the mouse moves.
*/
bool
nGuiMouseCursor::OnMouseMoved(const vector2& mousePos)
{
    this->UpdateRect();
    return nGuiLabel::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiMouseCursor::SetBrush(const nString& brush)
{
    this->SetDefaultBrush(brush.Get());
    this->brushSizeIsDirty = true;
    this->openFirstFrame = true;
    this->fadeinRequested = true;
    this->UpdateRect();
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiMouseCursor::Render()
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

        nGuiLabel::Render();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    This computes the color (takes fade in and fade out effect into account).
*/
void
nGuiMouseCursor::UpdateColor()
{
    this->windowColor = vector4(1.0f, 1.0f, 1.0f, 1.0f);

    // NOTE: the use on nTimeServer is intentional, as the current
    // frame time stamp is useless (would give fade delays when
    // resources are loaded)
    nTime time = nTimeServer::Instance()->GetTime();

    // to obscure resource loading delays, the actual fadein snapshot
    // is only taken after the first rendering (where resources
    // are demand-loaded)
    if (this->openFirstFrame && (this->fadeInTime > 0.0f))
    {
        // window is always invisible in first frame
        this->windowColor.w = 0.0f;
    }
    else if (this->fadeinRequested)
    {
        // fade in?
        if ((this->fadeInTime > 0.01f) && (time < (this->fadeinRequestTime + fadeInTime)))
        {
            // fade alpha
            float lerp = n_saturate((float) ((time - this->fadeinRequestTime) / this->fadeInTime));
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
nGuiMouseCursor::UpdateRect()
{
    // get current mouse pos directly from input server
    const vector2& mousePos = nGuiServer::Instance()->GetMousePos();

    if (this->brushSizeIsDirty && !this->lockBrushSize)
    {
        this->brushSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize(this->GetDefaultBrush());
        this->brushSizeIsDirty = false;
    }

    // update screen space rectangle
    vector2 absHotSpot(this->hotSpot.x * this->brushSize.x, this->hotSpot.y * this->brushSize.y);
    rectangle r(mousePos - absHotSpot, mousePos + this->brushSize - absHotSpot);
    //nGuiServer::Instance()->MoveRectToVisibleArea(r);

    this->SetRect(r);
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiMouseCursor::Inside(const vector2& p)
{
    return false;
}


