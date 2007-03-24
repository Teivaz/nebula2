//------------------------------------------------------------------------------
//  nBuddyClient_main.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nbuddyclient.h"
#include "kernel/nipcbuffer.h"
#include "util/nstream.h"


nNebulaScriptClass(nBuddyClient, "nroot");

nBuddyClient* nBuddyClient::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nBuddyClient::nBuddyClient() :
    ipcClient(0),
    isOpen(false),
    clientStatus(Invalid),
    time(0.0),
    retryTime(0.0),
    numRetries(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nBuddyClient::~nBuddyClient()
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
    Open the client. The client will NOT try to connect the server here, this
    will happen periodically inside Trigger() until a connection could
    be established (the server may not be ready yet to accept client connections).
*/
bool
nBuddyClient::Open()
{
    n_assert(Invalid == this->clientStatus);
    n_assert(0 == this->ipcClient);
    // initialize the ipc client object (but do not connect yet)
    this->ipcClient = n_new(nIpcClient);
    this->ipcClient->SetBlocking(false);
    this->retryTime = 0.0;
    this->numRetries = 0;
    this->clientStatus = Connecting;
    this->onlineStatus = Offline;

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the client. If the client is currently in started state, it will
    send a "~closesession" message to the server.
*/
void
nBuddyClient::Close()
{
    n_assert(this->isOpen);
    n_assert(this->ipcClient);

    // release ipc client
    if (this->ipcClient->IsConnected())
    {

       // this->ipcClient->Send(nIpcBuffer("~closesession"));

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
nBuddyClient::Trigger()
{
    n_assert(this->isOpen);
    n_assert(this->ipcClient);
    n_assert(Invalid != this->GetClientStatus());

    #ifdef __WIN32__
    SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE),
            FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    #endif // __WIN32__

     //alle paar millisecs

    /*
    static int bla = 0;
    bla++;
    static int bla2 = 0;

    nString str;
    str.Format("user%d",bla2);
if (!(bla % 100))
{
    if (bla2 <= 5000)
    {
        bla2++;
        this->Login(str.Get(),"pass");
    }
}
*/

    // delete outdated updates
    this->DeleteLastUpdates();

    // connection lost ? try to reconnect
    //if (!this->ipcClient->IsConnected()) this->SetClientStatus(Connecting);

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
                #ifdef __WIN32__
                SetConsoleTextAttribute(
                GetStdHandle(STD_OUTPUT_HANDLE),
                FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
                #endif // __WIN32__

                nString message = curMsg;

                message.ANSItoUTF8();
                n_printf("\nMessage from server: %s\n", message.Get());

                nStream query("MyData");
                if (!query.OpenString(message))
                {
                   n_printf("\nnBuddyClient: invalid query\n");
                }
                else
                {
                   n_printf("\nnBuddyClient: execute query\n");

                   nString result;
                   CommandInterpreter.Execute(query);
                }

            }
            while (curMsg = msgBuffer.GetNextString());
        }
    }
}



//------------------------------------------------------------------------------
/**
    Sends a create user message
*/
void
nBuddyClient::CreateUser(const char* user,const char* pass)
{
    this->createuser = n_new(nCreateUser);

    this->createuser->SetUsername(nString(user));
    this->createuser->SetPassword(nString(pass));

    this->createuser->Execute();
}



void
nBuddyClient::Login(const char* user,const char* pass)
{
    this->login = n_new(nLogin);

    this->login->SetUsername(nString(user));
    this->login->SetPassword(nString(pass));
    nGuid guid;
    guid.Set("11111111-2222-3333-4444-555555555555");
    this->login->SetGameGuid(guid);

    this->login->Execute();

    //n_delete(this->login);

}

void
nBuddyClient::SendMessage(const char* user,const char* message)
{
    this->sendmessage = n_new(nSendMessage);

    this->sendmessage->SetBuddy(nString(user));
    this->sendmessage->SetMessageText(nString(message));

    this->sendmessage->Execute();
}



void nBuddyClient::AddBuddy(const char* buddy)
{
    this->addbuddy = n_new(nAddBuddy);
    this->addbuddy->SetBuddy(nString(buddy));
    this->addbuddy->Execute();
}

void nBuddyClient::GetBuddylist()
{
    this->buddylist = n_new(nGetBuddylist);
    this->buddylist->Execute();
}



void nBuddyClient::AddUpdate(nUpdate* update)
{
    this->Updates.PushBack(update);
}

void nBuddyClient::DeleteLastUpdates()
{
   nArray<nUpdate*>::iterator it;

   for (it = this->Updates.Begin();it != this->Updates.End();)
   {
        //temp = it;
        it = this->Updates.Erase(it);
        //n_delete((*temp));
   }

}


bool nBuddyClient::SendCommand(nCommand* command)
{
    if (this->clientStatus != Connected) return false;

    if (this->ipcClient->Send(nIpcBuffer(command->GetRequest().Get())))
    {
        command->curStatus = nCommand::COMMAMD_PENDING;
        this->CommandInterpreter.AddPendingCommand(command);
        return true;
    }

    return false;
}

