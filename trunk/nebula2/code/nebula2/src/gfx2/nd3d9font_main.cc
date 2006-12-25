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
    if (this->IsLoaded())
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
    n_assert(Unloaded == this->GetState());
    n_assert(0 == this->d3dFont);

    // check if the font resource must be created...
    if (this->fontDesc.GetFilename())
    {
        // mangle path
        nString mangledPath = nFileServer2::Instance()->ManglePath(this->fontDesc.GetFilename());
        int numFonts = AddFontResource(mangledPath.Get());
        n_assert(numFonts > 0);
    }

    int fontFlags = 0;
    switch (this->fontDesc.GetWeight())
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

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    UINT height = UINT(float(this->fontDesc.GetHeight()) * gfxServer->GetFontScale());
    if (height < UINT(gfxServer->GetMinFontHeight()))
    {
        height = gfxServer->GetMinFontHeight();
    }
    HRESULT hr = D3DXCreateFont(this->refD3D9Server->d3d9Device,
        height, 0,
        fontFlags, 0,
        this->fontDesc.GetItalic(),
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        this->fontDesc.GetAntiAliased() ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE,
        this->fontDesc.GetTypeFace(),
        &this->d3dFont);

    n_dxtrace(hr, "D3DXCreateFont() failed!");
    n_assert(this->d3dFont);
    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Font::UnloadResource()
{
    n_assert(Unloaded != this->GetState());
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
        nString mangledPath = nFileServer2::Instance()->ManglePath(this->fontDesc.GetFilename());
        BOOL res = RemoveFontResource(mangledPath.Get());
        n_assert(0 != res);
    }
    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device is going into lost state.
*/
void
nD3D9Font::OnLost()
{
    if (this->d3dFont)
    {
        n_assert(Lost != this->GetState());
        this->d3dFont->OnLostDevice();
        this->SetState(Lost);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device is available again.
*/
void
nD3D9Font::OnRestored()
{
    if (this->d3dFont)
    {
        n_assert(Lost == this->GetState());
        this->d3dFont->OnResetDevice();
        this->SetState(Valid);
    }
}
