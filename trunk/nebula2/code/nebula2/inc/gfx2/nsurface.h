//-----------------------------------------------------------------------------
// nsurface.h
// (c) 2003, 2004 George McBay, Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_SURFACE_H
#define N_SURFACE_H

//-----------------------------------------------------------------------------
/**
    @class nSurface
    @ingroup NGameSwfConribModule

    @brief
*/
#include "kernel/nroot.h"
#include "gfx2/ntexture2.h"

class nSurface : public nRoot
{
public:
    nSurface();
    virtual ~nSurface();

    virtual void LoadFromMemory(void* data, nTexture2::Format format,
                                int wdith, int height, int pitch);

    virtual void Lock(int& pitch, void **data);
    virtual void Unlock();

};
#endif /*N_SURFACE_H*/
