//-----------------------------------------------------------------------------
// nsurface.h
// (c) 2003, 2004 Gerge Mcbay, Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_SURFACE_H
#define N_SURFACE_H

//-----------------------------------------------------------------------------
/**
    @class nSurface
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

    static nKernelServer* kernelServer;

};
#endif /*N_SURFACE*/
