//------------------------------------------------------------------------------
//  nguitextlabel_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextlabel.h"
#include "resource/nresourceserver.h"

nNebulaScriptClass(nGuiTextLabel, "nguilabel");

//------------------------------------------------------------------------------
/**
*/
nGuiTextLabel::nGuiTextLabel() :
    refGfxServer("/sys/servers/gfx"),
    refResourceServer("/sys/servers/resource"),
    color(1.0f, 1.0f, 1.0f, 1.0f),
    shadowColor(0.0f, 0.0f, 0.0f, 0.5f),
    shadowOffset(0.0015f, 0.0015f),
    align(Center),
    border(0.01f, 0.0f),
    fontName("GuiDefault"),
    clipping(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTextLabel::~nGuiTextLabel()
{
    if (this->refFont.isvalid())
    {
        this->refFont->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetText(const char* text)
{
    n_assert(text);
    this->text = text;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiTextLabel::GetText() const
{
    return this->text.Get();
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

    // (re-)validate the font object
    if (!this->refFont.isvalid())
    {
        this->refFont = (nFont2*) this->refResourceServer->FindResource(this->fontName.Get(), nResource::Font);
        if (!this->refFont.isvalid())
        {
            n_error("nGuiTextLabel %s: Unknown font '%s'!", this->GetName(), this->fontName.Get()); 
        }
        else
        {
            this->refFont->AddRef();
        }
    }

    // compute the text position
    this->refGfxServer->SetFont(this->refFont.get());
    uint renderFlags = nFont2::VCenter;
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

    rectangle screenSpaceRect = this->GetScreenSpaceRect();
    screenSpaceRect.v0 += this->border;
    screenSpaceRect.v1 -= this->border;
    /*
    rectangle shadowRect = screenSpaceRect;
    shadowRect.v0 += this->shadowOffset;
    shadowRect.v1 += this->shadowOffset;
    if (!pressed)
    {
        // draw the shadow text
        this->refGfxServer->DrawText(this->GetText(), this->shadowColor, shadowRect, renderFlags);
    }
    */

    // draw text
    rectangle pressedRect = screenSpaceRect;
    pressedRect.v0 += this->shadowOffset;
    pressedRect.v1 += this->shadowOffset;
    if (pressed)
    {
        this->refGfxServer->DrawText(this->GetText(), this->color, pressedRect, renderFlags);
    }
    else
    {
        this->refGfxServer->DrawText(this->GetText(), this->color, screenSpaceRect, renderFlags);
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
