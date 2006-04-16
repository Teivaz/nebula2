//------------------------------------------------------------------------------
/**
    @page NebulaToolsngfxtool ngfxtool

    ngfxtool

    (C) 2003 RadonLabs GmbH
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "tools/ncmdlineargs.h"
#include "sql/nsqlserver.h"
#include "resource/nresourceserver.h"
#include "variable/nvariableserver.h"

nNebulaUsePackage(nnebula);

//------------------------------------------------------------------------------
/**
    Main function.
*/
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    // get cmd line args
    bool helpArg            = args.GetBoolArg("-help");
    nString inArg           = args.GetStringArg("-in", 0);
    nString outArg          = args.GetStringArg("-out", 0);
    nString scriptServerArg = args.GetStringArg("-scriptserver", "ntclserver");
    nString sqlServerArg    = args.GetStringArg("-sqlserver", "nsqlite3server");

    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
            "ngfxtool - Nebula2 gfx tool\n"
            "Command line args:\n"
            "------------------\n"
            "-help                   show this help\n"
            "-in [filename]          input gfx file (.n2 extension)\n"
            "-out [filename]         output gfx file (.n2 extension)\n"
            "-scriptserver           define an alternative script server class (default is ntclserver)\n"
            "-sqlserver              define an alternative sql server class (default is nsqlite3server)\n");
        return 5;
    }

    // create minimal Nebula runtime
    nKernelServer kernelServer;
    kernelServer.AddPackage(nnebula);

    nScriptServer* scriptServer = (nScriptServer*) kernelServer.New(scriptServerArg.Get(), "/sys/servers/script");
    if (0 == scriptServer)
    {
        n_printf("Could not create script server of class '%s'\n", scriptServerArg.Get());
        return 10;
    }
    nSqlServer* sqlServer = (nSqlServer*) kernelServer.New(sqlServerArg.Get(), "/sys/servers/sql");
    if (0 == sqlServer)
    {
        n_printf("Could not create sql server of class '%s'\n", sqlServerArg.Get());
        return 10;
    }
    nVariableServer* variableServer = (nVariableServer*) kernelServer.New("nvariableserver", "/sys/servers/variable");
    nResourceServer* resServer = (nResourceServer*) kernelServer.New("nresourceserver", "/sys/servers/resource");
    if (inArg.IsValid()) {
        nObject* obj = kernelServer.Load(inArg.Get());
        if (outArg.IsValid() && 0 != obj) {
            obj->SaveAs(outArg.Get());
            obj->Release();
        }
    }
    resServer->Release();
    variableServer->Release();
    sqlServer->Release();
    scriptServer->Release();

    return 0;
}
