//------------------------------------------------------------------------------
/**
    @page NebulaToolsnsh nsh

    nsh

    The Nebula2 Shell (nsh.exe) can be used to execute Nebula2 scripts or to
    interactively browse a minimal Nebula2 environment.
    Nebula2 has a pluggable scripting architecture. The default scripting
    language is <b>MicroTCL</b>, which is a stripped down TCL dialect, with some
    Nebula specific extensions.

    Alternative scripting languages provided by the Open Source community
    include Lua, Python, Ruby, and others...

    The default Nebula2 shell looks very much like a Tcl shell which allows to
    navigate the hierarchy of live Nebula2 C++ objects like a filesystem
    hierarchy and invoke commands on them.

    Use the command <b>exit</b> to quit a running nsh instance.

    <dl>
     <dt>-help</dt>
       <dd>show this help</dd>
     <dt>-startup</dt>
       <dd>run script and go into interactive mode</dd>
     <dt>-run</dt>
       <dd>run script and exit</dd>
     <dt>-scriptserver</dt>
       <dd>define an alternative script server class (default is ntclserver)</dd>
    </dl>

    (C) 2003 RadonLabs GmbH
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "tools/ncmdlineargs.h"
#include "network/nhttpserver.h"
#include "sql/nsqlserver.h"
#include "resource/nresourceserver.h"
#include "variable/nvariableserver.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(nnetwork);

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
    nString startupArg      = args.GetStringArg("-startup", 0);
    nString runArg          = args.GetStringArg("-run", 0);
    nString scriptServerArg = args.GetStringArg("-scriptserver", "ntclserver");
    nString sqlServerArg    = args.GetStringArg("-sqlserver", "nsqlite3server");

    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
               "nsh - Nebula2 shell\n"
               "Command line args:\n"
               "------------------\n"
               "-help                   show this help\n"
               "-startup                run script and go into interactive mode\n"
               "-run                    run script and exit\n"
               "-scriptserver           define an alternative script server class (default is ntclserver)\n"
               "-sqlserver              define an alternative sql server class (default is nsqlite3server)\n");
        return 5;
    }

    // create minimal Nebula runtime
    nKernelServer kernelServer;
    kernelServer.AddPackage(nnebula);
    kernelServer.AddPackage(nnetwork);

    nScriptServer* scriptServer = (nScriptServer*) kernelServer.New(scriptServerArg.Get(), "/sys/servers/script");
    if (0 == scriptServer)
    {
        n_printf("Could not create script server of class '%s'\n", scriptServerArg.Get());
        return 10;
    }
    nVariableServer* variableServer = (nVariableServer*) kernelServer.New("nvariableserver", "/sys/servers/variable");
    nHttpServer* httpServer = (nHttpServer*) kernelServer.New("nhttpserver", "/sys/servers/http");
    nResourceServer* resServer = (nResourceServer*) kernelServer.New("nresourceserver", "/sys/servers/resource");
    nSqlServer* sqlServer = (nSqlServer*) kernelServer.New("nsqlite3server", "/sys/servers/sql");
    if (runArg.IsValid())
    {
        nString result;
        scriptServer->RunScript(runArg.Get(), result);
    }
    else
    {
        if (startupArg.IsValid())
        {
            nString result;
            scriptServer->RunScript(startupArg.Get(), result);
        }

        // interactively execute commands
        bool lineOk = true;
        scriptServer->SetFailOnError(false);
        while (!scriptServer->GetQuitRequested() && lineOk)
        {
            char line[1024];
            line[0] = '\0';

            // generate prompt string
            nString prompt = scriptServer->Prompt();
            printf("%s", prompt.Get());
            fflush(stdout);

            // get user input
            lineOk = (gets(line) > 0);
            if (strlen(line) > 0)
            {
                nString result = 0;
                scriptServer->Run(line, result);
                if (result.IsValid())
                {
                    printf("%s\n", result.Get());
                }
            }
        }
    }
    sqlServer->Release();
    httpServer->Release();
    resServer->Release();
    variableServer->Release();
    scriptServer->Release();

    return 0;
}
