//------------------------------------------------------------------------------
//  nnetserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "network/nnetserver.h"

nNebulaScriptClass(nNetServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nNetServer::nNetServer() :
    isOpen(false),
    isStarted(false),
    ipcServer(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNetServer::~nNetServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nNetServer::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->IsStarted());
    n_assert(0 == this->ipcServer);

    // reset the client info array
    int numClients = this->clientArray.Size();
    int clientIndex;
    for (clientIndex = 0; clientIndex < numClients; clientIndex++)
    {
        this->clientArray[clientIndex].SetIpcClientId(-1);
        this->clientArray[clientIndex].SetClientStatus(Waiting);
    }

    // initialize the ipc server object
    nIpcAddress ipcServerAddress("any", this->GetPortName());
    this->ipcServer = n_new(nIpcServer(ipcServerAddress));

    this->isStarted = false;
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nNetServer::Close()
{
    n_assert(this->isOpen);
    n_assert(this->ipcServer);

    // send the "~closesession" msg to all connected clients
    this->ipcServer->SendAll(nIpcBuffer("~closesession"));

    // kill ipc server object
    n_delete(this->ipcServer);
    this->ipcServer = 0;

    // reset the client info array
    int numClients = this->clientArray.Size();
    int clientIndex;
    for (clientIndex = 0; clientIndex < numClients; clientIndex++)
    {
        this->clientArray[clientIndex].SetIpcClientId(-1);
        this->clientArray[clientIndex].SetClientStatus(Invalid);
    }

    this->isStarted = false;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Process pending messages. This must be called frequently (i.e.
    once per frame) while the network server is open.
*/
void
nNetServer::Trigger()
{
    n_assert(this->isOpen);
    n_assert(this->ipcServer);

    // poll ipc server and get pending messages
    if (this->ipcServer->Poll())
    {
        nIpcBuffer recvMsg(4096);
        int fromClientId;
        while (this->ipcServer->GetMsg(recvMsg, fromClientId))
        {
            // one received message could contain several strings
            const char* curMsg = recvMsg.GetFirstString();
            if (curMsg) do
            {
                // n_printf("nNetServer: msg received: %s\n", curMsg);

                nString tokenString = curMsg;
                const char* cmd = tokenString.GetFirstToken(" ");
                if (cmd)
                {
                    if (0 == strcmp(cmd, "~joinsession"))
                    {
                        // handle a join session request
                        this->HandleJoinSessionRequest(fromClientId, tokenString.GetNextToken(" "));
                    }
                    else if (0 == strcmp(cmd, "~closesession"))
                    {
                        // handle a close session request
                        this->Close();
                        return;
                    }
                    else
                    {
                        // an unknown message, let subclass handle it
                        this->HandleMessage(fromClientId, curMsg);
                    }
                }
            } 
            while ((curMsg = recvMsg.GetNextString()));
        }
    }

    // as soon as all clients have connected, send a "~start" message to all clients
    if (!this->isStarted)
    {
        bool start = true;
        int numClients = this->GetNumClients();
        int clientIndex;
        for (clientIndex = 0; clientIndex < numClients; clientIndex++)
        {
            if (this->clientArray[clientIndex].GetClientStatus() != Connected)
            {
                start = false;
                break;
            }
        }
        if (start)
        {
            this->isStarted = true;
            nIpcBuffer startMsg("~start");
            for (clientIndex = 0; clientIndex < numClients; clientIndex++)
            {
                this->ipcServer->Send(this->clientArray[clientIndex].GetIpcClientId(), startMsg);
            }
            this->OnStart();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Handle a join session request message from a client. This checks if the 
    provided guid matches one of the pre-configured guid's, and if this
    is the case, a "~joinaccepted" message is returned, otherwise, a
    "~joindenied".
*/
void
nNetServer::HandleJoinSessionRequest(int clientId, const char* clientGuid)
{
    n_assert(this->isOpen);
    n_assert(this->ipcServer);

    nGuid guid(clientGuid);

    int numClients = this->clientArray.Size();
    int clientIndex;
    for (clientIndex = 0; clientIndex < numClients; clientIndex++)
    {
        if ((this->clientArray[clientIndex].GetClientGuid() == guid) &&
            (Connected != this->clientArray[clientIndex].GetClientStatus()))
        {
            this->clientArray[clientIndex].SetClientStatus(Connected);
            this->clientArray[clientIndex].SetIpcClientId(clientId);
            this->ipcServer->Send(clientId, nIpcBuffer("~joinaccepted"));
            return;
        }
    }

    // fallthrough: invalid join request
    this->ipcServer->Send(clientId, nIpcBuffer("~joindenied"));
}

//------------------------------------------------------------------------------
/**
    Handle a custom message. This method should be overwritten by 
    subclasses to check to process specific custom messages.
*/
bool
nNetServer::HandleMessage(int /*fromClientId*/, const char* /*msg*/)
{
    // n_printf("nNetServer::HandleMessage(%d, '%s'\n", fromClientId, msg);
    return true;
}

//------------------------------------------------------------------------------
/**
    Handle protocol specific OnStart() actions. Should be overwritten by
    subclasses.
*/
void
nNetServer::OnStart()
{
    // n_printf("nNetServer::OnStart() called\n");
}
