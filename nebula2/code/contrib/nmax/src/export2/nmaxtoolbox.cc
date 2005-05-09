//------------------------------------------------------------------------------
//  nmaxtoolbox.cc
//
//  (c)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#include "base/nmaxdll.h"
#include "export2/nmaxoptions.h"

//------------------------------------------------------------------------------
/**
    called when the plugin dll is loaded.
*/
bool nMaxPluginInitialize()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    called when the plugin dll is unloaded.
*/
void nMaxPluginUninitialize()
{
    // release option module.
    nMaxOptions* options = nMaxOptions::Instance();
    if (options)
    {
        n_delete(options);
    }
}
