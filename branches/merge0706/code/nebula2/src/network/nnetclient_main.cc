//------------------------------------------------------------------------------
//  nnetclient_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "network/nnetclient.h"
#include "kernel/nipcbuffer.h"

nNebulaScriptClass(nNetClient, "nroot");

//------------------------------------------------------------------------------
/**
*/
nNetClient::nNetClient() :
    ipcClient(0),
    isOpen(false),
    clientStatus(Invalid),
    time(0.0),
    retryTime(0.0),
    numRetries(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNetClient::~nNetClient()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open the client. The client will NOT try to connect the server here, this
    will happen periodically inside Trigger() until a connection could
    be established (the server may not be ready yet to accept client connections).
*/
bool
nNetClient::Open()
{
    n_assert(Invalid == this->clientStatus);
    n_assert(0 == this->ipcClient);

    // initialize the ipc client object (but do not connect yet)
    this->ipcClient = n_new(nIpcClient);
    this->ipcClient->SetBlocking(false);
    this->retryTime = 0.0;
    this->numRetries = 0;
    this->clientStatus = Connecting;

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the client. If the client is currently in started state, it will
    send a "~closesession" message to the server.
*/
void
nNetClient::Close()
{
    n_assert(this->isOpen);
    n_assert(this->ipcClient);

    // release ipc client
    if (this->ipcClient->IsConnected())
    {
        // send a close message to the server (may fail if the server is already closed)
        this->ipcClient->Send(nIpcBuffer("~closesession"));

        // disconnect from the server
        this->ipcClient->Disconnect();
    }
    n_delete(this->ipcClient);
    this->ipcClient = 0;

    this->clientStatus = Invalid;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Per-Frame-Trigger method. If not yet connected, the client will 
    periodically try to connect to the server. If a connection is established,
    the client will wait for the "~start" message from the server, and
    set the IsStarted() flag to true.
*/
void
nNetClient::Trigger()
{
    n_assert(this->isOpen);
    n_assert(this->ipcClient);
    n_assert(Invalid != this->GetClientStatus());

    // if not connected, periodically try to connect
    if (this->GetClientStatus() == Connecting)
    {
        // not yet connected: time for a new connection trial?
        double timeDiff = this->time - this->retryTime;
        if (timeDiff > 1.0f)
        {
            this->retryTime = this->time;
            this->numRetries++;
            if (this->ipcClient->Connect(this->ipcAddress))
            {
                // lowlevel connection established, authenticate with server
                char msgString[1024];
                sprintf(msgString, "~joinsession %s", this->clientGuid.Get());
                this->ipcClient->Send(nIpcBuffer(msgString));
                this->SetClientStatus(Connected);
            }
            else
            {
                // failed to establish connection
            }
        }
    }
    else
    {
        // handle pending messages
        nIpcBuffer msgBuffer(4096);
        while (this->ipcClient->Receive(msgBuffer))
        {
            // there may be multiple strings in one received msg buffer
            const char* curMsg = msgBuffer.GetFirstString();
            if (curMsg) do
            {
                // n_printf("Message from server: %s\n", curMsg);

                nString tokenString = curMsg;
                const char* cmd = tokenString.GetFirstToken(" ");
                if (cmd)
                {
                    if (0 == strcmp(cmd, "~joinaccepted"))
                    {
                        if (this->GetClientStatus() == Connected)
                        {
                            this->SetClientStatus(JoinAccepted);
                        }
                    }
                    else if (0 == strcmp(cmd, "~joindenied"))
                    {
                        if (this->GetClientStatus() == Connecting)
                        {
                            this->SetClientStatus(JoinDenied);
                        }
                    }
                    else if (0 == strcmp(cmd, "~start"))
                    {
                        /* MUST BE HANDLED BY SUBCLASS WHEN GAME INITIALIZED!
                        if (this->GetClientStatus() == JoinAccepted)
                        {
                            this->SetClientStatus(Started);
                        }
                        */
                    }
                    else if (0 == strcmp(cmd, "~closesession"))
                    {
                        this->Close();
                        return;
                    }
                    else
                    {
                        this->HandleMessage(curMsg);
                    }
                }
            }
            while ((curMsg = msgBuffer.GetNextString()));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Handle a custom message. Should be implemented by subclasses.
*/
void
nNetClient::HandleMessage(const char* msg)
{
    n_assert(msg);
    // n_printf("nNetClient::HandleMessage(%s)\n", msg);
}
