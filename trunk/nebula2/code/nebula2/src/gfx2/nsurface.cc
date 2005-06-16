//-----------------------------------------------------------------------------
//  nsurface.cc
//
//  (c) 2003, 2004 George McBay, Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "kernel/nkernelserver.h"

#include "gfx2/nsurface.h"

nNebulaClass(nSurface, "nroot");

//-----------------------------------------------------------------------------
/**
*/
nSurface::nSurface ()
{
}

//-----------------------------------------------------------------------------
/**
*/
nSurface::~nSurface ()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nSurface::LoadFromMemory (void* /*data*/, nTexture2::Format /*format*/, 
                               int /*wdith*/, int /*height*/, int /*pitch*/)
{
    // implemented in the overrided class.
}

//-----------------------------------------------------------------------------
/**
*/
void nSurface::Lock (int& /*pitch*/, void** /*data*/)
{
    // implemented in the overrided class.
}

//-----------------------------------------------------------------------------
/**
*/
void nSurface::Unlock ()
{
    // implemented in the overrided class.
}
