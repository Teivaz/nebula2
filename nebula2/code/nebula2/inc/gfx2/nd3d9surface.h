//-----------------------------------------------------------------------------
// nd3d9surface.h
// (c) 2003, 2004 George McBay, Kim, Hyoun Woo
//-----------------------------------------------------------------------------

#ifndef N_D3D9SURFACE_H
#define N_D3D9SURFACE_H

//-----------------------------------------------------------------------------
/**
    @class nD3DSurface	
    @ingroup NGameSwfContribModule
    @brief
*/

#include <d3d9.h>
#include "gfx2/nsurface.h"


class nD3D9Surface : public nSurface
{
public:
    nD3D9Surface();
    virtual ~nD3D9Surface();

    virtual void LoadFromMemory (void* data, nTexture2::Format format, 
                                 int width, int height, int pitch);

    virtual void Lock(int& pitch, void **data);
    virtual void Unlock();

private:
    friend class nD3D9Texture;

    IDirect3DSurface9 *baseSurface;
    
};
#endif /*N_D3D9SURFACE_H*/
