//------------------------------------------------------------------------------
//  nBuddyServer_main.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nbuddyserver.h"
#include "util/nstream.h"
#include "network/nusercontroller.h"


nNebulaClass(nBuddyServer, "nroot");

nBuddyServer* nBuddyServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nBuddyServer::nBuddyServer() :
    isOpen(false),
    ipcServer(0),
    portNum(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nBuddyServer::~nBuddyServer()
{
    n_assert(Singleton);
    if (this->IsOpen())
    {
        this->Close();
    }
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nBuddyServer::Open()
{
    n_assert(!this->isOpen);
    n_assert(0 == this->ipcServer);


    #ifdef __WIN32__
    struct WSAData wsa_data;
    WSAStartup(MAKEWORD(2,2), &wsa_data);
    #endif

    // initialize the ipc server object
    nIpcAddress ipcServerAddress("any", this->portNum);
    this->ipcServer = n_new(nIpcServer(ipcServerAddress));

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nBuddyServer::Close()
{
    n_assert(this->isOpen);
    n_assert(this->ipcServer);

    // send the "~closesession" msg to all connected clients
    this->ipcServer->SendAll(nIpcBuffer("~closesession"));

    // kill ipc server object
    n_delete(this->ipcServer);
    this->ipcServer = 0;

    this->isOpen = false;

    #ifdef __WIN32__
    WSACleanup();
    #endif
}

//------------------------------------------------------------------------------
/**
    Process pending messages. This must be called frequently (i.e.
    once per frame) while the network server is open.
*/
void
nBuddyServer::Trigger()
{
    n_assert(this->isOpen);
    n_assert(this->ipcServer);

    nUserController::Instance()->Trigger();

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
                n_printf("\nnBuddyServer: query received: %s\n", curMsg);


                nString message = curMsg;
                message.ANSItoUTF8();
                nStream query("MyData");

                //GetCurrentNodeLineNumber() is to check if root is valid
                if (message.IsEmpty() || !query.OpenString(message) || !query.GetCurrentNodeLineNumber())
                {
                   n_printf("\nnBuddyServer: invalid query\n");
                }
                else
                {
                   n_printf("\nnBuddyServer: execute query\n");

                   nString result;
                   CommandInterpreter.Execute(result,query,fromClientId);

                   // send response
                   if (!result.IsEmpty())
                   {
                       this->ipcServer->Send(fromClientId, nIpcBuffer(result.Get()));
                   }
                }
            }
            while (curMsg = recvMsg.GetNextString());
        }
    }
}



bool nBuddyServer::SendMessage(int& IpcClientID,nString& Message)
{
    return this->ipcServer->Send(IpcClientID,nIpcBuffer(Message.Get()));
}