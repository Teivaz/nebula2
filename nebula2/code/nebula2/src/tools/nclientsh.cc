//------------------------------------------------------------------------------
/**
    @page NebulaClientShellTest

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "network/nbuddyclient.h"
#include "tools/ncmdlineargs.h"
#include "resource/nresourceserver.h"


nNebulaUsePackage(nnebula);
nNebulaUsePackage(nnetwork);

//------------------------------------------------------------------------------
/**
    Trigger Thread Function
*/
static
int
N_THREADPROC
TriggerFunc(nThread *thread)
{
    // tell thread object that we have started
    thread->ThreadStarted();

    nBuddyClient* client = (nBuddyClient*) thread->LockUserData();
    thread->UnlockUserData();

    while (client->IsOpen())
    {
        static int i = 0;
        i++;
        client->SetTime(i);
        client->Trigger();
        n_sleep(0); // call the sheduler to be multitask friendly
    } //while (!thread->ThreadStopRequested());
    thread->ThreadHarakiri();
    return 0;
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
    nString scriptServerArg = args.GetStringArg("-scriptserver", "ntclserver");
    nString serverArg       = args.GetStringArg("-server", "192.168.0.105");



    if (helpArg)
    {
        printf("(C) 2006 RadonLabs GmbH\n"
               "nmasterserver - Nebula2 buddymasterserver\n"
               "Command line args:\n"
               "------------------\n"
               "-help                   show this help\n"
               "-startup                run script and go into interactive mode\n"
               "-run                    run script and exit\n"
               "-scriptserver           define an alternative script server class (default is ntclserver)\n"
               "-server                 define an alternative masterserver (default is 192.168.0.105)\n");
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

    nResourceServer* resServer = (nResourceServer*) kernelServer.New("nresourceserver", "/sys/servers/resource");

    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,0),&wsa) == SOCKET_ERROR)
    {
        printf("Error %d returned by WSAStartup\n", GetLastError());
        exit(1);
    }


    nBuddyClient* buddyClient = (nBuddyClient*) kernelServer.New("nbuddyclient", "/sys/servers/buddyclient");
    buddyClient->SetServerHostName(serverArg.Get());
    buddyClient->SetServerPortNum(5326);
    buddyClient->Open();

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




         nThread *Thread;
         Thread = n_new(nThread(TriggerFunc,
                                         nThread::Normal,
                                         0,
                                         NULL,
                                         NULL,
                                         (void*)buddyClient));


        // interactively execute commands
        bool lineOk = true;
        scriptServer->SetFailOnError(false);
        nString result = 0;
        scriptServer->Run("sel sys/servers/buddyclient", result);

        while (!scriptServer->GetQuitRequested() && lineOk)
        {

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
            }
        }
    }
    resServer->Release();
    scriptServer->Release();
    buddyClient->Close();
    buddyClient->Release();


    // Shutdown WinSock subsystem.
    WSACleanup();

    return 0;
}
