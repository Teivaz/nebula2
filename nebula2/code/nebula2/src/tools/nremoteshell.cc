//------------------------------------------------------------------------------
//  nremoteshell.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include <stdio.h>
#include "kernel/nkernelserver.h"
#include "kernel/nipcclient.h"
#include "tools/ncmdlineargs.h"

//------------------------------------------------------------------------------
/**
    Extracts a string from a nMsgNode.
*/
const char*
getMsg(nMsgNode* msgNode)
{
    const char* str = "";
    if (msgNode)
    {
        const char* msgString = (const char*) msgNode->GetMsgPtr();
        int msgSize = msgNode->GetMsgSize();
        if (msgString && (msgSize > 0) && (msgString[msgSize - 1] == 0))
        {
            str = msgString;
        }
    }
    return str;
}

//------------------------------------------------------------------------------
/**
*/
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    bool helpArg = args.GetBoolArg("-help");
    const char* portArg = args.GetStringArg("-port", 0);

    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
               "nrsh - Nebula2 remote shell\n"
               "Command line args:\n"
               "------------------\n"
               "-help       show this help\n"
               "-port       defines target port in the form address:portname (see below)\n\n"
               "Examples:\n"
               "---------\n"
               "nrsh -port localhost:nviewer\n"
               "nrsh -port 192.168.0.1:test\n"
               "nrsh -port zeus:nviewer\n");
        return 5;
    }
    if (!portArg)
    {
        printf("nremoteshell Error: no portname defined ('nremoteshell -help' for help)!\n");
        return 5;
    }

    // create an ipc client and try to connect
    printf("*** Nebula2 remote shell *** \n");
    printf("(C) 2003 RadonLabs GmbH\n");
    printf("Trying %s...\n", portArg);
    
    nIpcClient ipcClient;
    if (ipcClient.Connect(portArg))
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
    while (running && lineOk)
    {
        char line[2048];

        // generate prompt
        const char* pselCmd = "psel";
        int pselLen = strlen(pselCmd) + 1;
        nMsgNode* prompt = ipcClient.SendMsg((void*) pselCmd, pselLen);
        printf("%s>", getMsg(prompt));
        fflush(stdout);
        ipcClient.FreeReplyMsgNode(prompt);

        // get command
        lineOk = (gets(line) > 0);

        if (strcmp("exit", line) == 0)
        {
            running = false;
        }
        else
        {
            int msgLen = strlen(line) + 1;
            nMsgNode* reply = ipcClient.SendMsg(line, msgLen);
            const char* msg = getMsg(reply);
            if (*msg != 0)
            {
                puts(msg);
            }
            ipcClient.FreeReplyMsgNode(reply);
        }
    }
    return 0;
}