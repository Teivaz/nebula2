//------------------------------------------------------------------------------
//  nd3d9font_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9font.h"
#include "kernel/nfileserver2.h"
#include "gfx2/nd3d9server.h"

nNebulaClass(nD3D9Font, "nfont2");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nd3d9font

    @cppclass
    nD3D9Font
    
    @superclass
    nfont2
    
    @classinfo
    Docs needed.
*/

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
    if (this->IsValid())
    {
        this->Unload();
    }
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

    int fontFlags = 0;
    switch(this->fontDesc.GetWeight()) 
    {
    case nFontDesc::Thin: 
        fontFlags |= FW_THIN;
      break;
    case nFontDesc::Light:
        fontFlags |= FW_LIGHT;
      break;
    case nFontDesc::Normal:
        fontFlags |= FW_NORMAL;
        break;
    case nFontDesc::Bold:
        fontFlags |= FW_BOLD;
        break;
    case nFontDesc::ExtraBold:
        fontFlags |= FW_EXTRABOLD;
        break;
    }

    HRESULT hr = D3DXCreateFont(this->refD3D9Server->d3d9Device,
        this->fontDesc.GetHeight(), 0,
        fontFlags, 0,
        this->fontDesc.GetItalic(),
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        this->fontDesc.GetAntiAliased() ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE,
        this->fontDesc.GetTypeFace(),
        &this->d3dFont);

    n_assert(SUCCEEDED(hr));        
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
    nD3D9Server* gfxServer = this->refD3D9Server.get();

    // if this is the currently set font, unlink from gfx server
    if (gfxServer->GetFont() == this)
    {
        gfxServer->SetFont(0);
    }

    // delete d3d font object
    this->d3dFont->Release();
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

