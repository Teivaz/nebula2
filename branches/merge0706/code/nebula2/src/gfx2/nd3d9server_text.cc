//------------------------------------------------------------------------------
//  nd3d9server_text.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9font.h"
#include "gfx2/nd3d9texture.h"

//------------------------------------------------------------------------------
/**
    Initialize the text renderer. Must be called by ReloadResources().
*/
void
nD3D9Server::OpenTextRenderer()
{
    n_assert(this->d3d9Device);

    if (!this->refDefaultFont.isvalid())
    {
        nFontDesc fontDesc;
        fontDesc.SetAntiAliased(true);
        fontDesc.SetHeight(16);
        fontDesc.SetWeight(nFontDesc::Bold);
        fontDesc.SetTypeFace("Arial");
        nFont2* font = this->NewFont("Default", fontDesc);
        if (!font->Load())
        {
            n_error("nD3D9Server: Failed to load default font!");
        }
        this->refDefaultFont = font;
        this->SetFont(font);
    }

    // create sprite object for batched rendering
    HRESULT hr = D3DXCreateSprite(this->d3d9Device, &this->d3dSprite);
    n_dxtrace(hr, "D3DCreateSprite() failed!");
}

//------------------------------------------------------------------------------
/**
    Shutdown the text renderer. Must be called by UnloadResources().
*/
void
nD3D9Server::CloseTextRenderer()
{
    this->textElements.Clear();
    if (this->d3dSprite)
    {
        this->d3dSprite->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Public text rendering routine. Either draws the text immediately,
    or stores the text internally in a text element array and draws it
    towards the end of frame.
*/
void
nD3D9Server::DrawText(const nString& text, const vector4& color, const rectangle& rect, uint flags, bool immediate)
{
    if (this->refFont.isvalid())
    {
        if (immediate)
        {
            this->DrawTextImmediate(this->refFont, text, color, rect, flags);
        }
        else
        {
            TextElement textElement(this->refFont, text, color, rect, flags);
            this->textElements.Append(textElement);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Draws the accumulated text elements and flushes the text buffer.
*/
void
nD3D9Server::DrawTextBuffer()
{
    int i;
    int num = this->textElements.Size();
    for (i = 0; i < num; i++)
    {
        const TextElement& textElement = this->textElements[i];
        this->DrawTextImmediate(textElement.refFont, textElement.text, textElement.color, textElement.rect, textElement.flags);
    }
    this->textElements.Clear();
}

//------------------------------------------------------------------------------
/**
    Internal text rendering routine.

    @param  text    the text to draw
    @param  color   the text color
    @param  rect    screen space rectangle in which to draw the text
    @param  flags   combination of nFont2::RenderFlags
*/
void
nD3D9Server::DrawTextImmediate(nFont2* font, const nString& text, const vector4& color, const rectangle& rect, uint flags)
{
    if (!text.IsValid())
    {
        // nothing to do.
        return;
    }
    
    n_assert(0 != font);
    if (!font->IsLoaded())
    {
        font->Load();
    }
    float dispWidth  = (float) this->windowHandler.GetDisplayMode().GetWidth();
    float dispHeight = (float) this->windowHandler.GetDisplayMode().GetHeight();
    RECT r;
    r.left   = (LONG) (rect.v0.x * dispWidth);
    r.top    = (LONG) (rect.v0.y * dispHeight);
    r.right  = (LONG) (rect.v1.x * dispWidth);
    r.bottom = (LONG) (rect.v1.y * dispHeight);     

    DWORD d3dFlags = 0;
    nString wordBreakString;
    if (flags & nFont2::Bottom)     d3dFlags |= DT_BOTTOM;
    if (flags & nFont2::Top)        d3dFlags |= DT_TOP;
    if (flags & nFont2::Center)     d3dFlags |= DT_CENTER;
    if (flags & nFont2::Left)       d3dFlags |= DT_LEFT;
    if (flags & nFont2::Right)      d3dFlags |= DT_RIGHT;
    if (flags & nFont2::VCenter)    d3dFlags |= DT_VCENTER;
    if (flags & nFont2::NoClip)     d3dFlags |= DT_NOCLIP;
    if (flags & nFont2::ExpandTabs) d3dFlags |= DT_EXPANDTABS;

    DWORD d3dColor = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
    ID3DXFont* d3dFont = ((nD3D9Font*)font)->GetD3DFont();
    n_assert(d3dFont);
    this->d3dSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
    if (flags & nFont2::WordBreak)
    {
        //d3dFlags |= DT_WORDBREAK;
        this->BreakLines(text, rect, wordBreakString);
        d3dFont->DrawText(this->d3dSprite, wordBreakString.Get(), -1, &r, d3dFlags, d3dColor);
    }
    else
    {
        d3dFont->DrawText(this->d3dSprite, text.Get(), -1, &r, d3dFlags, d3dColor);
    }
    this->d3dSprite->End();
}

//------------------------------------------------------------------------------
/**
    Get text extents.

    - 16-Feb-04     floh    hmm, ID3DXFont extent computation is confused by spaces,
                            now uses GDI functions to compute text extents
*/
vector2
nD3D9Server::GetTextExtent(const nString& text)
{
    int width = 0;
    int height = 0;
    float dispWidth  = (float) this->windowHandler.GetDisplayMode().GetWidth();
    float dispHeight = (float) this->windowHandler.GetDisplayMode().GetHeight();
    if (this->refFont.isvalid())
    {
        if (!this->refFont->IsLoaded())
        {
            this->refFont->Load();
        }
        ID3DXFont* d3dFont = ((nD3D9Font*)this->refFont.get())->GetD3DFont();
        RECT rect = { 0 };
        if (text.IsValid())
        {
            d3dFont->DrawTextA(NULL, text.Get(), -1, &rect, DT_LEFT | DT_NOCLIP | DT_CALCRECT, 0);
        }
        else
        {
            // hmm, an empty text should give us at least the correct height
            d3dFont->DrawTextA(NULL, " ", -1, &rect, DT_LEFT | DT_NOCLIP | DT_CALCRECT, 0);
            rect.right = 0;
            rect.left = 0;
        }
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }
    return vector2((float(width) / dispWidth), (float(height) / dispHeight));
}
