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
        fontDesc.SetHeight(18);
        fontDesc.SetWeight(nFontDesc::Normal);
        fontDesc.SetTypeFace("Arial");
        nFont2* font = this->NewFont("Default", fontDesc);
        if (!font->Load())
            n_error("nD3D9Server: Failed to load default font!");

        this->refDefaultFont = font;
    }

    // create sprite object for batched rendering
    HRESULT hr = D3DXCreateSprite(this->d3d9Device, &this->d3dSprite);
    n_assert(SUCCEEDED(hr));
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
        n_delete textNode;
    }

    if (this->d3dSprite)
        this->d3dSprite->Release();
}

//------------------------------------------------------------------------------
/**
    Add a new text node to the text node list. Text node are accumulated 
    until EndScene(), where they are rendered and flushed.
*/
void
nD3D9Server::Text(const char* text, const vector4& color, float x, float y)
{
    TextNode* newTextNode = new TextNode(text, color, x, y);
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
    if (!this->textNodeList.IsEmpty())
    {
        n_assert(this->d3d9Device);

        // flush the mesh at stream 0 because we cannot preserve its state
        this->SetMesh(0, 0);

        ID3DXFont* d3dFont = ((nD3D9Font*)this->refDefaultFont.get())->GetD3DFont();
        this->d3dSprite->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_SORT_TEXTURE);
        TextNode* textNode;
        while (textNode = (TextNode*) this->textNodeList.RemHead())
        {
            int width = this->displayMode.GetWidth();
            int height = this->displayMode.GetHeight();
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

            n_delete textNode;
        }
        this->d3dSprite->End();
    }
}

//------------------------------------------------------------------------------
/**
    New style immediate text rendering method.
*/
void
nD3D9Server::DrawText(const char* text, const vector4& color, float xPos, float yPos)
{
    n_assert(text);
    if (this->refFont.isvalid())
    {
        if (!this->refFont->IsValid())
        {
            this->refFont->Load();
        }        
        float dispWidth  = (float) this->displayMode.GetWidth();
        float dispHeight = (float) this->displayMode.GetHeight();
        RECT r;
        r.left = (LONG) (xPos * dispWidth);
        r.top = (LONG) (yPos * dispHeight);
    
        DWORD d3dColor = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
        ID3DXFont* d3dFont = ((nD3D9Font*)this->refFont.get())->GetD3DFont();
        n_assert(d3dFont);
        this->d3dSprite->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_SORT_TEXTURE);
        d3dFont->DrawText(this->d3dSprite,
            text,
            -1,&r,
            DT_LEFT|DT_NOCLIP|DT_SINGLELINE,
            d3dColor);
        this->d3dSprite->End();
    }
}

//------------------------------------------------------------------------------
/**
    Get text extents.
*/
vector2
nD3D9Server::GetTextExtent(const char* text)
{
    n_assert(text);
    int width = 0;
    int height = 0;
    float dispWidth  = (float) this->displayMode.GetWidth();
    float dispHeight = (float) this->displayMode.GetHeight();

    if (this->refFont.isvalid())
    {
        if (!this->refFont->IsValid())
        {
            this->refFont->Load();
        }        
        RECT r;
        r.left = 0;
        r.top = 0;
        r.bottom = 0;
        r.right = 0;

        ID3DXFont* d3dFont = ((nD3D9Font*)this->refFont.get())->GetD3DFont();
        n_assert(d3dFont);
        height = d3dFont->DrawText(0,
            text,
            -1,&r,
            DT_LEFT|DT_NOCLIP|DT_SINGLELINE|DT_CALCRECT,
            0);
        width = r.right;
    }
    return vector2((float(width) / dispWidth), (float(height) / dispHeight));
}
