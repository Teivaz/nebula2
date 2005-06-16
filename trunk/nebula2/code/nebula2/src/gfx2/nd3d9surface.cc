//-----------------------------------------------------------------------------
//  nd3d9surface.cc
//
//  (c) 2003, 2004 George McBay, Kim, Hyoun Woo
//-----------------------------------------------------------------------------

#include <d3d9.h>
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9texture.h"
#include "gfx2/nd3d9surface.h"

nNebulaClass(nD3D9Surface, "nsurface");

//-----------------------------------------------------------------------------
/**
*/
nD3D9Surface::nD3D9Surface() : 
    baseSurface(NULL)
{
}

//-----------------------------------------------------------------------------
/**
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
    Load surface data from memory.

    @note
    entire surface is specified for the destination surface.

    @param data    pointer to the upper left corner of the source image in memory.
    @param format  the pixel format of the source image.
    @param width   width of the source image.
    @param height  height of the source image.
    @param pitch   pitch of source image, in bytes.
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

    D3DFORMAT d3dFormat = nD3D9Texture::FormatToD3DFormat(format);

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
