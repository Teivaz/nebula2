#define N_IMPLEMENTS nRemoteServer
#define N_KERNEL
//------------------------------------------------------------------------------
//  nremoteserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nremoteserver.h"
#include "kernel/nscriptserver.h"

nNebulaClass(nRemoteServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nRemoteServer::nRemoteServer() :
    refScriptServer(kernelServer),
    isOpen(false),
    ipcServer(0)
{
    this->refScriptServer = "/sys/servers/script";
}

//------------------------------------------------------------------------------
/**
*/
nRemoteServer::~nRemoteServer()
{
    if (this->isOpen)
    {
        this->Close();
    }
    n_assert(0 == this->ipcServer);
}

//------------------------------------------------------------------------------
/**
*/
bool
nRemoteServer::Open(const char* portName)
{
    n_assert(!this->isOpen);
    n_assert(portName);
    n_assert(0 == this->ipcServer);

#ifdef __WIN32__
    #ifdef __XBxX__
        xbNetStartup();
    #endif    
    struct WSAData wsa_data;
    WSAStartup(0x101, &wsa_data);
#endif

    // create an ipc server object
    this->ipcServer = n_new nIpcServer(portName);
    n_assert(this->ipcServer);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nRemoteServer::Close()
{
    n_assert(this->isOpen);
    n_assert(this->ipcServer);

    // release all client context objects
    nClientContext* curContext;
    while ((curContext = (nClientContext*) this->clientContexts.RemHead()))
    {
        n_delete curContext;
    }

    // kill ipc server object
    n_delete this->ipcServer;

    this->ipcServer = 0;
    this->isOpen = false;

#ifdef __WIN32__
    #ifdef __XBxX__
        xbNetShutdown();
    #endif
    WSACleanup();
#endif

}

//------------------------------------------------------------------------------
/**
*/
void
nRemoteServer::Broadcast(const char* str)
{
    n_assert(str);
    if (this->isOpen)
    {
        n_assert(this->ipcServer);
        this->ipcServer->BroadcastMsg((void*) str, strlen(str) + 1);
    }
}

//------------------------------------------------------------------------------
/**
    Find client context matching the given client id. If no matching client
    context exists, create and initialize a new one.
*/
nRemoteServer::nClientContext*
nRemoteServer::GetClientContext(int clientId)
{
    n_assert(-1 != clientId);

    // find existing client context
    nClientContext* curContext;
    for (curContext = (nClientContext*) this->clientContexts.GetHead();
         curContext;
         curContext = (nClientContext*) curContext->GetSucc())
    {
        if (clientId == curContext->GetClientId())
        {
            return curContext;
        }
    }

    // fallthrough: create a new client context
    n_printf("nRemoteServer: creating new client context!\n");
    nClientContext* newContext = n_new nClientContext(clientId, this);
    n_assert(newContext);
    newContext->SetCwd(kernelServer->GetCwd());
    
    this->clientContexts.AddTail(newContext);
    return newContext;
}

//------------------------------------------------------------------------------
/**
    Return the cwd object for the client id (creates a new client context
    if no matching context exists yet). If the client object's cwd no longer
    exists, return the kernel's current cwd.

    @param  clientId    a client id from nIpcServer::GetMsg()
    @return             a cwd object (always valid)
*/
nRoot*
nRemoteServer::GetClientCwd(int clientId)
{
    nClientContext* context = this->GetClientContext(clientId);
    nRoot* cwd = context->GetCwd();
    if (!cwd)
    {
        // old cwd has vanished, replace with kernelserver cwd
        cwd = kernelServer->GetCwd();
        context->SetCwd(cwd);
    }
    return cwd;
}

//------------------------------------------------------------------------------
/**
    Update the cwd in a client context defined by its cliend id.
*/
void
nRemoteServer::SetClientCwd(int clientId, nRoot* cwd)
{
    nClientContext* context = this->GetClientContext(clientId);
    context->SetCwd(cwd);
}

//------------------------------------------------------------------------------
/**
    Process pending messages. Should either be called frequently (ie once
    per frame), or after a call to WaitMsg().

    @todo implement quitRequested functionality

    @return     true if a client has requested the remote server to quit
*/
bool
nRemoteServer::Trigger()
{
    if (this->isOpen)
    {
        n_assert(this->ipcServer);
        nScriptServer* scriptServer = this->refScriptServer.get();

        // turn off FailOnError mode in script server
        bool origFailOnError = scriptServer->GetFailOnError();
        scriptServer->SetFailOnError(false);

        // poll ipc server and check if any messages are pending
        if (this->ipcServer->Poll())
        {
            // for each pending message...
            nMsgNode* msg;
            int msgClientId;
            while ((msg = this->ipcServer->GetMsg(msgClientId)))
            {
                // make sure the message is a valid string
                const char* msgPtr = (const char*) msg->GetMsgPtr();
                int msgSize  = msg->GetMsgSize();
                n_assert(msgPtr);
                if ((msgSize > 0) && (msgPtr[msgSize - 1] == 0))
                {
                    // set the client's cwd
                    kernelServer->PushCwd(this->GetClientCwd(msgClientId));

                    // this seems to be a valid message, send the message
                    // to the scriptserver for validation
                    const char* result = 0;
                    bool success = scriptServer->Run(msgPtr, result);

                    // release the msg node
                    this->ipcServer->ReplyMsg(msg);

                    // send the result string back to the client
                    const char* answerMsg = "";
                    if (result && (result[0] != 0))
                    {
                        answerMsg = result;
                    }
                    this->ipcServer->AnswerMsg((void*)answerMsg, strlen(answerMsg) + 1, msgClientId);

                    // store new cwd in client context
                    this->SetClientCwd(msgClientId, kernelServer->GetCwd());

                    // restore the original cwd
                    kernelServer->PopCwd();
                }
                else
                {
                    n_printf("nRemoteServer: BROKEN MESSAGE RECEIVED!\n");
                    const char* msg = "### comm error!";
                    this->ipcServer->AnswerMsg((void*) msg, strlen(msg) + 1, msgClientId);
                }
            }
        }

        // restore fail on error mode
        scriptServer->SetFailOnError(origFailOnError);
    }
    return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
