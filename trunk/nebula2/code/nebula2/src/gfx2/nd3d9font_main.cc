//------------------------------------------------------------------------------
//  nd3d9font_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9font.h"
#include "kernel/nfileserver2.h"
#include "gfx2/nd3d9server.h"

nNebulaClass(nD3D9Font, "nfont2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Font::nD3D9Font() :
    refD3D9Server("/sys/servers/gfx"),
    d3dFont(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nD3D9Font::~nD3D9Font()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9Font::LoadResource()
{
    n_assert(!this->IsValid());
    n_assert(0 == this->d3dFont);

    // check if the font resource must be created...
    if (this->fontDesc.GetFilename())
    {
        // mangle path
        nFileServer2* fileServer = kernelServer->GetFileServer();
        char mangledPath[N_MAXPATH];
        fileServer->ManglePath(this->fontDesc.GetFilename(), mangledPath, sizeof(mangledPath));

        int numFonts = AddFontResource(mangledPath);
        n_assert(numFonts > 0);
    }

    // create a CD3DFont object
    int d3dFlags = 0;
    if ((nFontDesc::Bold == this->fontDesc.GetWeight()) ||
        (nFontDesc::ExtraBold == this->fontDesc.GetWeight()))
    {
        d3dFlags |= D3DFONT_BOLD;
    }
    if (this->fontDesc.GetItalic())
    {
        d3dFlags |= D3DFONT_ITALIC;
    }
    this->d3dFont = new CD3DFont9((const unsigned char*) this->fontDesc.GetTypeFace(), this->fontDesc.GetHeight(), d3dFlags);

    n_assert(0 != this->refD3D9Server->d3d9Device);
    this->d3dFont->InitDeviceObjects(this->refD3D9Server->d3d9Device);
    this->d3dFont->RestoreDeviceObjects();

    n_assert(this->d3dFont);
    this->SetValid(true);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Font::UnloadResource()
{
    n_assert(this->IsValid());
    n_assert(this->d3dFont);

    // delete d3d font object
    delete this->d3dFont;
    this->d3dFont = 0;

    // remove optional Win32 font resource (if font loaded from file)
    if (this->fontDesc.GetFilename())
    {
        nFileServer2* fileServer = kernelServer->GetFileServer();
        char mangledPath[N_MAXPATH];
        fileServer->ManglePath(this->fontDesc.GetFilename(), mangledPath, sizeof(mangledPath));

        BOOL res = RemoveFontResource(mangledPath);
        n_assert(0 != res);
    }
    this->SetValid(false);
}


            


