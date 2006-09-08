//------------------------------------------------------------------------------
//  nmaxtoolbox.cc
//
//  (c)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#include "base/nmaxdll.h"
#include "export2/nmaxoptions.h"

nNebulaUsePackage(nnebula);

//------------------------------------------------------------------------------
/**
    called when the plugin dll is loaded.

    -17-Aug-06  kims  Modified to read INI file before the plugin generates
                      max script code.
                      
*/
bool nMaxPluginInitialize()
{
    nKernelServer::Instance()->AddPackage(nnebula);

    // load settings at the startup time cause utility panel is independent
    // to the exporting task. (same reason for saving the utility options too)
    nMaxOptions* options = nMaxOptions::Instance();
    if (!options->LoadUtilityOptions())
    {
        return false;
    }

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
        // save utility panel options.
        options->SaveUtilityOptions();

        n_delete(options);
    }
}
