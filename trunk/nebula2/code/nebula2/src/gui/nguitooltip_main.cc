//------------------------------------------------------------------------------
//  nguitooltip_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitooltip.h"
#include "input/ninputserver.h"
#include "resource/nresourceserver.h"

nNebulaClass(nGuiToolTip, "nguitextlabel");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nguitooltip

    @cppclass
    nGuiToolTip
    
    @superclass
    nguitextlabel
    
    @classinfo
    Docs needed.
*/

//------------------------------------------------------------------------------
/**
*/
nGuiToolTip::nGuiToolTip() :
    refInputServer("/sys/servers/input")
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
    Make sure that the rectangle is within screen boundaries.
*/
void
nGuiToolTip::ClipRect(rectangle& r) const
{
    vector2 size = r.v1 - r.v0;
    if (r.v0.x < 0.0f)
    {
        r.v0.x = 0.0f;
        r.v1.x = size.x;
    }
    else if (r.v1.x > 1.0f)
    {
        r.v1.x = 1.0f;
        r.v0.x = 1.0f - size.x;
    }
    if (r.v0.y < 0.0f)
    {
        r.v0.y = 0.0f;
        r.v1.y = size.y;
    }
    else if (r.v1.y > 1.0f)
    {
        r.v1.y = 1.0f;
        r.v0.y = 1.0f - size.y;
    }
}

//------------------------------------------------------------------------------
/**
    On becoming visible, the tooltip positions itself relative to the
    mouse and resizes itself according to it's text.
*/
void
nGuiToolTip::OnShow()
{
    // get current mouse pos directly from input server
    const vector2& mousePos = this->refInputServer->GetMousePos();

    // (re-)validate the font object
    if (!this->refFont.isvalid())
    {
        this->refFont = (nFont2*) this->refResourceServer->FindResource(this->fontName.Get(), nResource::Font);
        if (!this->refFont.isvalid())
        {
            n_error("nGuiTextLabel %s: Unknown font '%s'!", this->GetName(), this->fontName.Get()); 
        }
    }

    // get text size
    this->refGfxServer->SetFont(this->refFont.get());
    vector2 size = this->refGfxServer->GetTextExtent(this->GetText()) + this->border * 2.0f;

    // compute tooltip offset to mouse hot spot
    // we assume a 32x32 mouse pointer
    const nDisplayMode2& mode = this->refGfxServer->GetDisplayMode();
    vector2 offset(0.0f, 32.0f / mode.GetHeight());

    // update screen space rectangle
    rectangle r(mousePos + offset, mousePos + size + offset);
    this->ClipRect(r);
    this->SetRect(r);

    nGuiTextLabel::OnShow();
}

//------------------------------------------------------------------------------
/**
    Update the tooltips position when the mouse moves.
*/
bool
nGuiToolTip::OnMouseMoved(const vector2& mousePos)
{
    rectangle r = this->GetRect();
    vector2 size = r.v1 - r.v0;

    // compute tooltip offset to mouse hot spot
    // we assume a 32x32 mouse pointer
    const nDisplayMode2& mode = this->refGfxServer->GetDisplayMode();
    vector2 offset(0.0f, 32.0f / mode.GetHeight());

    r.v0 = mousePos + offset;
    r.v1 = mousePos + size + offset;

    this->ClipRect(r);
    this->SetRect(r);

    return nGuiTextLabel::OnMouseMoved(mousePos);
}
