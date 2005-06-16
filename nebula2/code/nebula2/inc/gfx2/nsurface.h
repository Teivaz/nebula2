//-----------------------------------------------------------------------------
//  nsurface.h
//
//  (c) 2003, 2004 George McBay, Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#ifndef N_SURFACE_H
#define N_SURFACE_H

//-----------------------------------------------------------------------------
/**
    @class nSurface
    @ingroup NebulaGraphicsSystem

    A surface represents a linear area of display memory and usually exists in
    the display memory of the display card, although surface can exist in system
    memory.

    The following example show that how to retrieve the data from the texture 
    in memory and access to it:
    @code
    int destPitch;
    unsigned char *surfaceData;

    nSurface *surface; 

    // get the surface of level 0.
    refTexture->GetSurfaceLevel("/tmp/surface", 0, &surface);

    unsigned char *src;
    unsigned char *dst;

    // lock the retrieved surface to write pixel to it.
    surface->Lock(destPitch, (void**)&surfaceData); 		

    for(int y = 0; y<imageHeight; y++)
    {
        // get the pointer to the scan line of the source image.
        src = imageData + (y * imagePitch);
        // get the pointer to the scan line of the destination surface.
        dst = surfaceData + (y * destPitch);

        // assume that source image is 24bit and destination surface is 32bit.
        for(int srcX=0, dstX=0; srcX<imagePitch; srcX += 3, dstX += 4)
        {
            dst[dstX]     = src[srcX + 2];
            dst[dstX + 1] = src[srcX + 1];
            dst[dstX + 2] = src[srcX]; 
        }
    } 
    // unlock should be called.
    surface->Unlock();
    @endcode

*/
#include "kernel/nroot.h"
#include "gfx2/ntexture2.h"

class nSurface : public nRoot
{
public:
    /// constructor.
    nSurface();
    /// destructor.
    virtual ~nSurface();
    /// load data from the memory.
    virtual void LoadFromMemory(void* data, nTexture2::Format format,
                                int wdith, int height, int pitch);

    /// lock the retrieved surfaces.
    virtual void Lock(int& pitch, void **data);
    /// unlock the surface.
    virtual void Unlock();

};
#endif /*N_SURFACE_H*/
