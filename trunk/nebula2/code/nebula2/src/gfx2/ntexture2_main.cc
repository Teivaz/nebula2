#define N_IMPLEMENTS nTexture2
//------------------------------------------------------------------------------
//  ntexture2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/ntexture2.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nTexture2, "nresource");

//------------------------------------------------------------------------------
/**
*/
nTexture2::nTexture2() :
    type(TEXTURE_NOTYPE),
    format(NOFORMAT),
    width(0),
    height(0),
    depth(0),
    numMipMaps(0),
    renderTargetFlags(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTexture2::~nTexture2()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}
