#define N_IMPLEMENTS nResourceLoader
//------------------------------------------------------------------------------
//  (C) 2003	Megan Fox
//------------------------------------------------------------------------------
#include "resource/nresourceloader.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nresourceloader

    @cppclass
    nResourceLoader
    
    @superclass
    nroot

    @classinfo
    Assists in loading resources, when the default behavior is not the desired
    behavior.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps writes the nCmd object contents out to a file.
    @return    success or failure
*/
bool
nResourceLoader::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

