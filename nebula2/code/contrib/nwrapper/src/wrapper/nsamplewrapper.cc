//------------------------------------------------------------------------------
/**
    This file is licensed under the terms of the Nebula License.
    (C) 2004 Bruce Mitchener, Jr.
*/
//------------------------------------------------------------------------------
#include "wrapper/nwrapperapp.h"
#include "kernel/nkernelserver.h"
#include "tools/ncmdlineargs.h"

int
main(int argc, const char* argv[])
{
    nCmdLineArgs args(argc, argv);

    // Create the kernel server and a minimal nebula runtime
    nKernelServer kernelServer;

    // get cmd line args
    bool helpArg                 = args.GetBoolArg("-help");
    const char * backendArg      = args.GetStringArg("-backend",
                                                     "nlanguagewrapper");

    if (helpArg)
    {
        n_printf("(C) 2004 Bruce Mitchener, Jr.\n"
                 "nsamplewrapper - Sample Nebula2 wrapper generator\n"
                 "Command line args:\n"
                 "------------------\n"
                 "-help                   show this help\n"
                 "-backend                the language backend to use\n");
        return 5;
    }

    nWrapperApp wrapperApp;
    wrapperApp.SetBackendClass(backendArg);
    wrapperApp.Run();
}

