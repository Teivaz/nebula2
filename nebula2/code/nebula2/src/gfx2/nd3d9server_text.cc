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
    n_assert(!this->d3dFont);

    // create the CD3DFont object
    this->d3dFont = new CD3DFont9((const unsigned char*) "Arial", 10, D3DFONT_BOLD);
    n_assert(this->d3dFont);

    // initialize the font object
    this->d3dFont->InitDeviceObjects(this->d3d9Device);
    this->d3dFont->RestoreDeviceObjects();
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
        delete textNode;
    }

    // kill the D3DFont object
    if (this->d3dFont)
    {
        delete this->d3dFont;
        this->d3dFont = 0;
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
        n_assert(this->d3dFont);

        // flush the mesh at stream 0 because we cannot preserve its state
        this->SetMesh(0, 0);

        TextNode* textNode;
        while (textNode = (TextNode*) this->textNodeList.RemHead())
        {
            int width = this->displayMode.GetWidth();
            int height = this->displayMode.GetHeight();

            this->d3dFont->DrawText(((textNode->xpos + 1.0f) * 0.5f * width) + 1.0f, 
                                    ((textNode->ypos + 1.0f) * 0.5f * height) + 1.0f,
                                    D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, textNode->color.w),
                                    (const unsigned char*) textNode->string.Get(), 0);
            this->d3dFont->DrawText((textNode->xpos + 1.0f) * 0.5f * width, 
                                    (textNode->ypos + 1.0f) * 0.5f * height,
                                    D3DCOLOR_COLORVALUE(textNode->color.x, textNode->color.y, textNode->color.z, textNode->color.w),
                                    (const unsigned char*) textNode->string.Get(), 0);
            n_delete textNode;
        }
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
    n_assert(this->refFont.isvalid());
    if (!this->refFont->IsValid())
    {
        this->refFont->Load();
    }
    float dispWidth  = (float) this->displayMode.GetWidth();
    float dispHeight = (float) this->displayMode.GetHeight();

    float x = xPos * dispWidth;
    float y = yPos * dispHeight;
    int d3dFlags = 0;
    if (this->refFont->GetFontDesc().GetAntiAliased())
    {
        d3dFlags |= D3DFONT_FILTERED;
    }
    DWORD d3dColor = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);

    CD3DFont9* d3dFont = ((nD3D9Font*)this->refFont.get())->GetD3DFont();
    n_assert(d3dFont);
    d3dFont->DrawTextA(x, y, d3dColor, (const unsigned char*) text, d3dFlags);
}

//------------------------------------------------------------------------------
/**
    Get text extents.
*/
vector2
nD3D9Server::GetTextExtent(const char* text)
{
    n_assert(text);
    n_assert(this->refFont.isvalid());
    if (!this->refFont->IsValid())
    {
        this->refFont->Load();
    }
    CD3DFont9* d3dFont = ((nD3D9Font*)this->refFont.get())->GetD3DFont();

    // get the current width and height of the display
    float dispWidth  = (float) this->displayMode.GetWidth();
    float dispHeight = (float) this->displayMode.GetHeight();

    SIZE intExtent;
    d3dFont->GetTextExtent((const unsigned char*) text, &intExtent);

    vector2 extent((float(intExtent.cx) / dispWidth), (float(intExtent.cy) / dispHeight));
    return extent;
}
