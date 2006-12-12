//------------------------------------------------------------------------------
/**
    @page Nebula MasterBox

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "network/nbuddyserver.h"
#include "tools/ncmdlineargs.h"
#include "sql/nsqlserver.h"
#include "network/nbuddydatabase.h"
#include "network/nUserController.h"
#include "resource/nresourceserver.h"
#include "kernel/ncrc.h"

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
    nString startupArg      = args.GetStringArg("-startup");
    nString runArg          = args.GetStringArg("-run");
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

    nScriptServer* scriptServer = (nScriptServer*)kernelServer.New(scriptServerArg.Get(), "/sys/servers/script");
    if (0 == scriptServer)
    {
        n_printf("Could not create script server of class '%s'\n", scriptServerArg.Get());
        return 10;
    }

    nResourceServer* resServer = (nResourceServer*)kernelServer.New("nresourceserver", "/sys/servers/resource");
    nSqlServer* sqlServer = (nSqlServer*)kernelServer.New("nsqlite3server", "/sys/servers/sql");
    nBuddyDatabase* buddydatabase = (nBuddyDatabase*)kernelServer.New("nbuddydatabase", "/sys/servers/buddydatabase");
    nUserController usercontroller;

    buddydatabase->Open();

     nBuddyServer* buddyServer = (nBuddyServer*) kernelServer.New("nbuddyserver", "/sys/servers/buddyserver");
     buddyServer->SetPortNum(5326);
     buddyServer->Open();

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

            /*
            char line[1024];
            line[0] = '\0';

            // generate prompt string
            nString prompt = scriptServer->Prompt();
            printf("%s", prompt.Get());
            fflush(stdout);

            // get user input
            bool lineOk = (gets(line) > 0);
            if (strlen(line) > 0)
            {
                nString result = 0;
                scriptServer->Run(line, result);
                if (result.IsValid())
                {
                    printf("%s\n", result.Get());
                }
            }*/

             buddyServer->Trigger();

             n_sleep(0);

        }
    }

    buddydatabase->Release();
    sqlServer->Release();
    resServer->Release();
    scriptServer->Release();
    buddyServer->Release();

    return 0;
}
