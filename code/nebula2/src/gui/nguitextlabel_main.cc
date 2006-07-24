//------------------------------------------------------------------------------
//  nguitextlabel_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextlabel.h"
#include "gfx2/ngfxserver2.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiTextLabel, "nguilabel");

//------------------------------------------------------------------------------
/**
*/
nGuiTextLabel::nGuiTextLabel() :
    color(1.0f, 1.0f, 1.0f, 1.0f),
    blinkColor(0.9f, 0.9f, 0.9f, 1.0f),
    align(Center),
    border(0.005f, 0.0f),
    pressedOffset(0.0015f, 0.0015f),
    fontName("GuiDefault"),
    clipping(true),
    vCenter(true),
    wordBreak(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTextLabel::~nGuiTextLabel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextLabel::SetText(const char* text)
{
    n_assert(text);
    this->text = text;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nGuiTextLabel::GetText() const
{
    return this->text.Get();
}

//------------------------------------------------------------------------------
/**
*/
vector2
nGuiTextLabel::GetTextExtent()
{
    this->ValidateFont();
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nFont2* oldFont = gfxServer->GetFont();
    gfxServer->SetFont(this->refFont.get());
    vector2 extent = gfxServer->GetTextExtent(this->GetText());
    gfxServer->SetFont(oldFont);
    return extent;
}

//------------------------------------------------------------------------------
/**
    Make sure the font object is valid.
*/
void
nGuiTextLabel::ValidateFont()
{
    if (!this->refFont.isvalid())
    {
        this->refFont = (nFont2*) nResourceServer::Instance()->FindResource(this->fontName.Get(), nResource::Font);
        if (!this->refFont.isvalid())
        {
            n_error("nGuiTextLabel %s: Unknown font '%s'!", this->GetName(), this->fontName.Get());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render the label text only. This is called by the general Render()
    method.

    @param  pressed         true if text should appear "pressed down"
*/
void
nGuiTextLabel::RenderText(bool pressed)
{
    if (this->text.IsEmpty())
    {
        // no text, nothing to render
        return;
    }

    this->ValidateFont();

    // compute the text position
    nGfxServer2::Instance()->SetFont(this->refFont.get());
    uint renderFlags = 0;
    switch (this->align)
    {
        case Left:  renderFlags |= nFont2::Left; break;
        case Right: renderFlags |= nFont2::Right; break;
        default:    renderFlags |= nFont2::Center; break;
    }
    if (!this->clipping)
    {
        renderFlags |= nFont2::NoClip;
    }
    if (this->wordBreak)
    {
        renderFlags |= nFont2::WordBreak;
    }
    if (this->vCenter)
    {
        renderFlags |= nFont2::VCenter;
    }

    rectangle screenSpaceRect = this->GetScreenSpaceRect();
    screenSpaceRect.v0 += this->border;
    screenSpaceRect.v1 -= this->border;

    // draw text
    rectangle pressedRect = screenSpaceRect;
    pressedRect.v0 += this->pressedOffset;
    pressedRect.v1 += this->pressedOffset;
    if (pressed)
    {
        nGuiServer::Instance()->DrawText(this->GetText(), this->color, pressedRect, renderFlags);
    }
    else if (this->blinking)
    {
        nTime time = nGuiServer::Instance()->GetTime();
        if (n_fmod(float(time), 0.6f) > 0.3)
        {
            nGuiServer::Instance()->DrawText(this->GetText(), this->blinkColor, screenSpaceRect, renderFlags);
        }
        else
        {
            nGuiServer::Instance()->DrawText(this->GetText(), this->color, screenSpaceRect, renderFlags);
        }
    }
    else
    {
        nGuiServer::Instance()->DrawText(this->GetText(), this->color, screenSpaceRect, renderFlags);
    }
}

//------------------------------------------------------------------------------
/**
    Renders the text label. This will first invoke the parent class
    nGuiLabel to render the labels background bitmap (if one exists).
    Afterwards the actual text will be rendered through the text server.
*/
bool
nGuiTextLabel::Render()
{
    if (this->IsShown())
    {
        // render the background image, if defined
        nGuiLabel::Render();

        // render the text
        this->RenderText(false);
        return true;
    }
    return false;
}
