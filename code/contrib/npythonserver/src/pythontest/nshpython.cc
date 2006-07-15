//------------------------------------------------------------------------------
/**
    @page NebulaToolsnshpython nshpython

    nsh

    <dl>
     <dt>-help</dt>
       <dd>show this help</dd>
     <dt>-startup</dt>
       <dd>run script and go into interactive mode</dd>
     <dt>-run</dt>
       <dd>run script and exit</dd>
    </dl>

    (C) 2003 RadonLabs GmbH
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "tools/ncmdlineargs.h"
#include "network/nhttpserver.h"
#include "variable/nvariableserver.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(nnetwork);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(npythonserver);

/**
    Package registration hook
*/
void
nPythonRegisterPackages(nKernelServer * kernelServer)
{
    kernelServer->AddPackage(nnebula);
    kernelServer->AddPackage(nnetwork);
    kernelServer->AddPackage(ndirect3d9);
    kernelServer->AddPackage(ndinput8);
    kernelServer->AddPackage(npythonserver);
}

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

    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
               "nsh - Nebula2 shell\n"
               "Command line args:\n"
               "------------------\n"
               "-help                   show this help\n"
               "-startup                run script and go into interactive mode\n"
               "-run                    run script and exit\n");
        return 5;
    }

    // create minimal Nebula runtime
    nKernelServer kernelServer;
    nPythonRegisterPackages(&kernelServer);

    nScriptServer* scriptServer = (nScriptServer*) kernelServer.New("npythonserver", "/sys/servers/script");
    if (0 == scriptServer)
    {
        n_printf("Could not create script server of class 'npythonserver'\n");
        return 10;
    }
    nVariableServer* variableServer = (nVariableServer*) kernelServer.New("nvariableserver", "/sys/servers/variable");
    nHttpServer* httpServer = (nHttpServer*) kernelServer.New("nhttpserver", "/sys/servers/http");
    n_assert(httpServer);

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
                nString result;
                scriptServer->Run(line, result);
                if (false == result.IsEmpty())
                {
                    printf("%s\n", result.Get());
                }
            }
        }
    }
    variableServer->Release();
    httpServer->Release();
    scriptServer->Release();
    return 0;
}
