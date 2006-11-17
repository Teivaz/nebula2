//-----------------------------------------------------------------------------
//  nmaxoptionscriptext.cc
//
//  (c)2005 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "base/nmaxlistener.h"
#include "export2/nmax.h"
#include "export2/nmaxoptionscriptext.h"
#include "export2/nmaxoptions.h"

//-----------------------------------------------------------------------------
/**
    Wrapper function to prevent redefinition errors which occur when including
    max script specific header files.
*/
const char* GetProjDir()
{
    // read paths from .ini file.
    nMaxOptions* options = nMaxOptions::Instance();
    if (!options->Initialize())
    {
        n_listener("Failed to read '%s' file from 'plugcfg' directory.", N_MAXEXPORT_INIFILE);

        return NULL;
    }

    nString projDir = options->GetHomePath();

    return strdup(projDir.Get());
}
