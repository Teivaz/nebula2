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
void nMaxPluginInitialize()
{
    // insert tasks which needed when the plugin startup.
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
