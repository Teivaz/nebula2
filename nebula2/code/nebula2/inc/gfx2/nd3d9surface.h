//-----------------------------------------------------------------------------
// nd3d9surface.h
//
// (c) 2003, 2004 George McBay, Kim, Hyoun Woo
//-----------------------------------------------------------------------------

#ifndef N_D3D9SURFACE_H
#define N_D3D9SURFACE_H

//-----------------------------------------------------------------------------
/**
    @class nD3DSurface	
    @ingroup NebulaD3D9GraphicsSystem

    A class which manages Direct3D surfaces. 
    This class provides methods to access directly the memory of the surface.

    See <tt>Direct3D Surfaces</tt> section in the document provided by D3D SDK
    for more details.

*/
#include <d3d9.h>
#include "gfx2/nsurface.h"

class nD3D9Surface : public nSurface
{
public:
    /// Constructor.
    nD3D9Surface();
    /// Destructor.
    virtual ~nD3D9Surface();

    virtual void LoadFromMemory (void* data, nTexture2::Format format, 
                                 int width, int height, int pitch);

    virtual void Lock(int& pitch, void **data);
    virtual void Unlock();

private:
    friend class nD3D9Texture;

    /// pointer to the surface which retrieved from the memory.
    IDirect3DSurface9 *baseSurface;
    
};
#endif /*N_D3D9SURFACE_H*/
