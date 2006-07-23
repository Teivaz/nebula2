//------------------------------------------------------------------------------
//  nremoteserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nremoteserver.h"
#include "kernel/nscriptserver.h"

nNebulaClass(nRemoteServer, "kernel::nroot");
nRemoteServer* nRemoteServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nRemoteServer::nRemoteServer() :
    refScriptServer("/sys/servers/script"),
    isOpen(false),
    ipcServer(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
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

    n_assert(0 != Singleton);
    Singleton = 0;
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
    nIpcAddress ipcAddress("any", portName);
    this->ipcServer = n_new(nIpcServer(ipcAddress));
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
        n_delete(curContext);
    }

    // kill ipc server object
    n_delete(this->ipcServer);

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
        nIpcBuffer msg(str, strlen(str) + 1);
        this->ipcServer->SendAll(msg);
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
    nClientContext* newContext = n_new(nClientContext(clientId, this));
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
            int fromClientId;
            nIpcBuffer recvMsg(4096);
            while (this->ipcServer->GetMsg(recvMsg, fromClientId))
            {
                // make sure the message is a valid string
                const char* curString = recvMsg.GetFirstString();
                if (curString)
                {
                    do
                    {
                        // set the client's cwd
                        kernelServer->PushCwd(this->GetClientCwd(fromClientId));

                        // this seems to be a valid message, send the message
                        // to the scriptserver for validation
                        nString result;
                        scriptServer->Run(curString, result);

                        // send the result string back to the client
                        nIpcBuffer resultMsg(4096);
                        resultMsg.SetString("");
                        if (false == result.IsEmpty())
                        {
                            resultMsg.SetString(result.Get());
                        }
                        this->ipcServer->Send(fromClientId, resultMsg);

                        // store new cwd in client context
                        this->SetClientCwd(fromClientId, kernelServer->GetCwd());

                        // restore the original cwd
                        kernelServer->PopCwd();
                    }
                    while ((curString = recvMsg.GetNextString()));
                }
                else
                {
                    n_printf("nRemoteServer: BROKEN MESSAGE RECEIVED!\n");
                    nIpcBuffer errorMsg("### comm error!");
                    this->ipcServer->Send(fromClientId, errorMsg);
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
