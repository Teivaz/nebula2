//------------------------------------------------------------------------------
/**
    @page NebulaToolsnremoteshell nremoteshell

    nremoteshell

    The Nebula2 Remote Shell (nremoteshell.exe) allows to remote-control active 
    Nebula2 applications much like Telnet allows to remote-control a Unix machine.
    Most Nebula2 applications open a remote port which nremoteshell can connect 
    to and issue commands.

    <dl>
     <dt>-help</dt>
       <dd>show this help</dd>
     <dt>-host</dt>
       <dd>defines host name or tcp/ip address</dd>
     <dt>-port</dt>
       <dd>defines port name or number</dd>
    </dl>

    Examples:

    To connect to a running nviewer instance on the local machine:
    @verbatim
    nremoteshell -host localhost -port nviewer
    @endverbatim

    To connect to a running nviewer instance on a remote machine with TCP/IP 
    number 192.168.0.1
    @verbatim
    nremoteshell -host 192.168.0.1 -port nviewer
    @endverbatim

    To connect to a running nviewer instance on a remote machine with the DNS or 
    SMB name @e zeus :
    @verbatim
    nremoteshell -host zeus -port nviewer
    @endverbatim

    (C) 2003 RadonLabs GmbH
*/
//------------------------------------------------------------------------------
#include <stdio.h>
#include "kernel/nkernelserver.h"
#include "kernel/nipcclient.h"
#include "tools/ncmdlineargs.h"

//------------------------------------------------------------------------------
/**
*/
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    bool helpArg = args.GetBoolArg("-help");
    nString hostArg = args.GetStringArg("-host", "localhost");
    nString portArg = args.GetStringArg("-port", 0);

    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
               "nrsh - Nebula2 remote shell\n"
               "Command line args:\n"
               "------------------\n"
               "-help       show this help\n"
               "-host       a host name\n"
               "-port       a port name\n");
        return 5;
    }
    if (!portArg.IsValid())
    {
        printf("nremoteshell Error: no portname defined ('nremoteshell -help' for help)!\n");
        return 5;
    }

    // create an ipc client and try to connect
    printf("*** Nebula2 remote shell *** \n");
    printf("(C) 2003 RadonLabs GmbH\n");
    printf("Trying host %s port %s...\n", hostArg, portArg);
    
    nIpcAddress ipcAddress(hostArg.Get(), portArg.Get());
    nIpcClient ipcClient;
    ipcClient.SetBlocking(true);
    if (ipcClient.Connect(ipcAddress))
    {
        printf("...connected\n");
    }
    else
    {
        printf("...connection failed!\n");
        return 5;
    }

    bool running = true;
    bool lineOk = true;
    nIpcBuffer msg(4096);
    while (running && lineOk)
    {
        char line[2048];

        // generate prompt
        msg.SetString("psel");
        if (ipcClient.Send(msg))
        {
            // wait for answer from server
            if (!ipcClient.Receive(msg))
            {
                printf("Error receiving prompt string!\n");
                running = false;
            }
            else
            {
                // display prompt string
                printf("%s>", msg.GetString());
                fflush(stdout);

                // get command from user
                lineOk = (gets(line) > 0);
		
                if (strcmp("exit", line) == 0)
                {
                    running = false;
                }
                else
                {
                    // send user command and wait for answer from server
                    msg.SetString(line);
                    if (ipcClient.Send(msg))
                    {
                        if (!ipcClient.Receive(msg))
                        {
                            printf("Error receiving reply!\n");
                            running = false;
                        }
                        else
                        {
                            // display answer from server
                            if (msg.GetString()[0] != 0)
                            {
                                puts(msg.GetString());
                            }
                        }
                    }
                }
            }
        }
    }
    if (ipcClient.IsConnected())
    {
        ipcClient.Disconnect();
    }
    return 0;
}
