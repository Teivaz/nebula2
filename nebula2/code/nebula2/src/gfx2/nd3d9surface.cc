//-----------------------------------------------------------------------------
// nd3d9surface.cc
// (c) 2003, 2004 George McBay, Kim, Hyoun Woo
//-----------------------------------------------------------------------------

#include <d3d9.h>
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9texture.h"
#include "gfx2/nd3d9surface.h"

nNebulaClass(nD3D9Surface, "nsurface");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nd3d9surface

    @cppclass
    nD3D9Surface
    
    @superclass
    nsurface
    
    @classinfo
    Docs needed.
*/


D3DFORMAT getD3DFormat(nTexture2::Format format);

//-----------------------------------------------------------------------------
/**
    Constructor.
*/
nD3D9Surface::nD3D9Surface() : baseSurface(NULL)
{
}

//-----------------------------------------------------------------------------
/**
    Destructor.
*/
nD3D9Surface::~nD3D9Surface()
{
    if (this->baseSurface != NULL)
    {
        this->baseSurface->Release();
    }
}

//-----------------------------------------------------------------------------
/**
    Convert nTexture2::Fromat to D3DFORMAT
*/
D3DFORMAT getD3DFormat(nTexture2::Format format)
{
    D3DFORMAT d3dFormat = D3DFMT_UNKNOWN;

    switch (format)
    {
        case nTexture2::X8R8G8B8:      d3dFormat = D3DFMT_X8R8G8B8; break;
        case nTexture2::A8R8G8B8:      d3dFormat = D3DFMT_A8R8G8B8; break;
        case nTexture2::R5G6B5:        d3dFormat = D3DFMT_R5G6B5; break;
        case nTexture2::A1R5G5B5:      d3dFormat = D3DFMT_A1R5G5B5; break;
        case nTexture2::A4R4G4B4:      d3dFormat = D3DFMT_A4R4G4B4; break;
        case nTexture2::P8:            d3dFormat = D3DFMT_P8; break;
        case nTexture2::DXT1:          d3dFormat = D3DFMT_DXT1; break;
        case nTexture2::DXT2:          d3dFormat = D3DFMT_DXT2; break;
        case nTexture2::DXT3:          d3dFormat = D3DFMT_DXT3; break;
        case nTexture2::DXT4:          d3dFormat = D3DFMT_DXT4; break;
        case nTexture2::DXT5:          d3dFormat = D3DFMT_DXT5; break;
        case nTexture2::R16F:          d3dFormat = D3DFMT_R16F; break;
        case nTexture2::G16R16F:       d3dFormat = D3DFMT_G16R16F; break;
        case nTexture2::A16B16G16R16F: d3dFormat = D3DFMT_A16B16G16R16F; break;
        case nTexture2::R32F:          d3dFormat = D3DFMT_R32F; break;
        case nTexture2::G32R32F:       d3dFormat = D3DFMT_G32R32F; break;
        case nTexture2::A32B32G32R32F: d3dFormat = D3DFMT_A32B32G32R32F; break;
        case nTexture2::A8:            d3dFormat = D3DFMT_A8; break;
        default:
            // can't happen
            n_assert(false);
    }

    n_assert(d3dFormat != D3DFMT_UNKNOWN);

    return d3dFormat;
}
//-----------------------------------------------------------------------------
/**
  Load surface data from memory.
*/
void nD3D9Surface::LoadFromMemory(void* data, nTexture2::Format format,
                                  int width, int height, int pitch)
{
    n_assert(this->baseSurface != NULL);

    HRESULT hr;

    RECT srcRect;
    srcRect.left   = 0;
    srcRect.top    = 0;
    srcRect.right  = width;
    srcRect.bottom = height;

    D3DFORMAT d3dFormat = getD3DFormat(format);

    hr = D3DXLoadSurfaceFromMemory (this->baseSurface, NULL, 
                                    NULL, data, d3dFormat, pitch, 
                                    NULL, &srcRect, D3DX_FILTER_NONE, 0);

    if (FAILED(hr))
    {
        n_printf ("nD3D9Surface::LoadFromMemory: D3DXLoadSurfaceFromMemory failed.\n");
        n_assert(SUCCEEDED(hr));
    }
}

//-----------------------------------------------------------------------------
/**
    Lock surface. (Don't forget to call Unlock)

    @param pitch the pitch of surface which to be locked.
    @param data pointer to data of surface which to be locked.
*/
void nD3D9Surface::Lock(int &pitch, void** data)
{
    n_assert(this->baseSurface != NULL);

    HRESULT hr;
    D3DLOCKED_RECT lockedRect;

    hr = this->baseSurface->LockRect(&lockedRect, 0, 0);

    n_assert(SUCCEEDED(hr));

    pitch = lockedRect.Pitch;
    *data = lockedRect.pBits;
}

//-----------------------------------------------------------------------------
/**
    Unlock surface.
*/
void nD3D9Surface::Unlock()
{
    n_assert(this->baseSurface != NULL);

    HRESULT hr;

    hr = this->baseSurface->UnlockRect();

    n_assert(SUCCEEDED(hr));
}


