#define N_IMPLEMENTS nD3D9Server
//------------------------------------------------------------------------------
//  nd3d9server_text.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"

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
    this->d3dFont = new CD3DFont9("Arial", 10, D3DFONT_BOLD);
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
nD3D9Server::Text(const char* text, float x, float y)
{
    TextNode* newTextNode = new TextNode(text, x, y);
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

            this->d3dFont->DrawText(((textNode->xpos + 1.0f) * 0.5f * width) + 1, 
                                    ((textNode->ypos + 1.0f) * 0.5f * height) + 1,
                                    D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f),
                                    (char*) textNode->string.Get(), 0);
            this->d3dFont->DrawText((textNode->xpos + 1.0f) * 0.5f * width, 
                                    (textNode->ypos + 1.0f) * 0.5f * height,
                                    D3DCOLOR_COLORVALUE(1.0f, 1.0f, 0.0f, 1.0f),
                                    (char*) textNode->string.Get(), 0);
            delete textNode;
        }
    }
}
