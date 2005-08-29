//------------------------------------------------------------------------------
//  nd3d9server_text.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9font.h"

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
    // kill any leftover text nodes
    TextNode* textNode;
    while (textNode = (TextNode*) this->textNodeList.RemHead())
    {
        n_delete(textNode);
    }

    if (this->d3dSprite)
    {
        this->d3dSprite->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Add a new text node to the text node list. Text node are accumulated 
    until EndScene(), where they are rendered and flushed.
*/
void
nD3D9Server::Text(const char* text, const vector4& color, float x, float y)
{
    TextNode* newTextNode = n_new(TextNode(text, color, x, y));
    this->textNodeList.AddTail(newTextNode);
}

//------------------------------------------------------------------------------
/**
    Render all text nodes added during this frame via DrawText(), and release
    the text nodes.
*/
void
nD3D9Server::DrawTextBuffer()
{
#if __NEBULA_STATS__
    this->profGUIDrawText.StartAccum();
#endif

    if (!this->textNodeList.IsEmpty())
    {
        n_assert(this->d3d9Device);

        // flush the mesh at stream 0 because we cannot preserve its state
        this->SetMesh(0, 0);

        ID3DXFont* d3dFont = ((nD3D9Font*)this->refDefaultFont.get())->GetD3DFont();
        this->d3dSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
        TextNode* textNode;
        while (textNode = (TextNode*) this->textNodeList.RemHead())
        {
            int width = this->windowHandler.GetDisplayMode().GetWidth();
            int height = this->windowHandler.GetDisplayMode().GetHeight();
            int len = textNode->string.Length();
            RECT r;
            r.right = width;
            r.bottom = height;     

            r.left = (LONG) ((textNode->xpos + 1.0f) * 0.5f * width) + 1;
            r.top = (LONG) ((textNode->ypos + 1.0f) * 0.5f * height) + 1;
            d3dFont->DrawText(this->d3dSprite,
                textNode->string.Get(),
                len,&r,
                DT_LEFT|DT_NOCLIP|DT_SINGLELINE,
                D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, textNode->color.w));
            r.left = (LONG) ((textNode->xpos + 1.0f) * 0.5f * width);
            r.top = (LONG) ((textNode->ypos + 1.0f) * 0.5f * height);
            d3dFont->DrawText(this->d3dSprite,
                textNode->string.Get(),
                len,&r,
                DT_LEFT|DT_NOCLIP|DT_SINGLELINE,
                D3DCOLOR_COLORVALUE(textNode->color.x, textNode->color.y, textNode->color.z, textNode->color.w));

            n_delete(textNode);
        }
        this->d3dSprite->End();
    }

#if __NEBULA_STATS__
    this->profGUIDrawText.StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    New style immediate text rendering method.

    @param  text    the text to draw
    @param  color   the text color
    @param  rect    screen space rectangle in which to draw the text
    @param  flags   combination of nFont2::RenderFlags
*/
void
nD3D9Server::DrawText(const char* text, const vector4& color, const rectangle& rect, uint flags)
{
#if __NEBULA_STATS__
    this->profGUIDrawText.StartAccum();
#endif

    n_assert(text);
    if (this->refFont.isvalid())
    {
        if (!this->refFont->IsLoaded())
        {
            this->refFont->Load();
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
        if (flags & nFont2::WordBreak)
        {
            //d3dFlags |= DT_WORDBREAK;
            this->BreakLines(text, rect, wordBreakString);
            text = wordBreakString.Get();
        }

        DWORD d3dColor = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
        ID3DXFont* d3dFont = ((nD3D9Font*)this->refFont.get())->GetD3DFont();
        n_assert(d3dFont);
        this->d3dSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
        d3dFont->DrawText(this->d3dSprite, text, -1, &r, d3dFlags, d3dColor);
        this->d3dSprite->End();
    }

#if __NEBULA_STATS__
    this->profGUIDrawText.StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Get text extents.

    - 16-Feb-04     floh    hmm, ID3DXFont extent computation is confused by spaces,
                            now uses GDI functions to compute text extents
*/
vector2
nD3D9Server::GetTextExtent(const char* text)
{
#if __NEBULA_STATS__
    this->profGUIGetTextExtent.StartAccum();
#endif
    n_assert(text);
    int width = 0;
    int height = 0;
    float dispWidth  = (float) this->windowHandler.GetDisplayMode().GetWidth();
    float dispHeight = (float) this->windowHandler.GetDisplayMode().GetHeight();

    // Note: A temporary buffer (extended by `.') is being used to
    // determine the extents of `text' to work around the problem that
    // `DrawTextA' seems to ignore trailing spaces in `text' if exist.
    // However this hack slows things down...
    if (this->refFont.isvalid())
    {
        if (!this->refFont->IsLoaded())
        {
            this->refFont->Load();
        }
        ID3DXFont* d3dFont = ((nD3D9Font*)this->refFont.get())->GetD3DFont();

        // Make a copy of `text' and extend it by `.'.
        int textLength = strlen(text);
        char* tmp = n_new_array(char, textLength + 2);
        strcpy(tmp, text);
        tmp[textLength] = '.';
        tmp[textLength + 1] = '\0';

        // Determine extents of `.'.
        RECT dotRect = { 0 };
        int h = d3dFont->DrawTextA(NULL, ".", -1, &dotRect, DT_LEFT | DT_NOCLIP | DT_CALCRECT, 0);
        int dotWidth = dotRect.right - dotRect.left;

        RECT rect = { 0 };
        h = d3dFont->DrawTextA(NULL, tmp, -1, &rect, DT_LEFT | DT_NOCLIP | DT_CALCRECT, 0);
    
        width = rect.right - rect.left - dotWidth;
        height = rect.bottom - rect.top;

        n_delete_array(tmp);
    }
#if __NEBULA_STATS__
    this->profGUIGetTextExtent.StopAccum();
#endif
    return vector2((float(width) / dispWidth), (float(height) / dispHeight));
}
