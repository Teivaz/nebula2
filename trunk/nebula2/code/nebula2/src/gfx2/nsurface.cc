//-----------------------------------------------------------------------------
// nsurface.cc
// (c) 2003, 2004 George McBay, Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "kernel/nkernelserver.h"

#include "gfx2/nsurface.h"

nNebulaClass(nSurface, "nroot");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nsurface

    @cppclass
    nSurface
    
    @superclass
    nresource
    
    @classinfo
    Docs needed.
*/

//-----------------------------------------------------------------------------
/**
    Constructor.
*/
nSurface::nSurface ()
{
}

//-----------------------------------------------------------------------------
/**
    Destructor.
*/
nSurface::~nSurface ()
{
}

//-----------------------------------------------------------------------------
void nSurface::LoadFromMemory (void* data, nTexture2::Format format, 
                               int wdith, int height, int pitch)
{
    // immplemented in overrided class
}

//-----------------------------------------------------------------------------
void nSurface::Lock (int& pitch, void **data)
{
    // immplemented in overrided class
}

//-----------------------------------------------------------------------------
void nSurface::Unlock ()
{
    // immplemented in overrided class
}