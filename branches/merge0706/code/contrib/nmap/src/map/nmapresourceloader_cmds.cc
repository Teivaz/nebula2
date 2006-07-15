#define N_IMPLEMENTS nMapResourceLoader
//------------------------------------------------------------------------------
//  (C) 2004	Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "map/nmapresourceloader.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmapresourceloader
    
    @superclass
    nresourceloader

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
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
nMapResourceLoader::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

