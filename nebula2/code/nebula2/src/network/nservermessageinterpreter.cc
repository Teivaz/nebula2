//------------------------------------------------------------------------------
//  nServerMessageInterpreter.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nservermessageinterpreter.h"
#include "network/nbuddydatabase.h"
#include "network/nusercontroller.h"
#include "network/nbuddyclient.h"
#include "network/nmessage.h"

//------------------------------------------------------------------------------
/**
*/
nServerMessageInterpreter::nServerMessageInterpreter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nServerMessageInterpreter::~nServerMessageInterpreter()
{
    // empty
}


//------------------------------------------------------------------------------
/**
    Executes a query and returns the result
*/
void nServerMessageInterpreter::Execute(nStream& query)
{
    nString Result;
    if (query.HasAttr("ver"))
    {
        // version 1.0 of command system
        if (query.GetFloat("ver")==1.0)
        {
             if (query.HasAttr("type"))
             {
                 nString type = query.GetString("type");


                 // requested stuff
                 if (query.HasAttr("id"))
                 {
                     int MessageId = query.GetInt("id");

                     HandleResponse(query,MessageId);
                 }

                 // unrequested stuff here

                 //update
                 else if (type=="update")
                 {
                     query.SetToFirstChild();

                     nString queryname = query.GetCurrentNodeName();

                     if (queryname == "message")
                     {
                        this->HandleMessage(query);
                     }
                 }
             }
        }
    }
}


void nServerMessageInterpreter::HandleMessage(nStream& query)
{
    nString user = query.GetString("user");
    nString text = query.GetString("text");

    text.UTF8toANSI();


    nMessage* message = n_new(nMessage);

    message->messageText = text;
    message->buddyName = user;

    nBuddyClient::Instance()->AddUpdate(message);

     SetConsoleTextAttribute(
     GetStdHandle(STD_OUTPUT_HANDLE),
     FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);

     nString message_str = nString("\nMessage from ") + user + nString(": ") + text;
     printf(message_str.Get());
}

void nServerMessageInterpreter::AddPendingCommand(nCommand* command)
{
    this->PendingCommands.PushBack(command);
}

void nServerMessageInterpreter::HandleResponse(nStream& query,int& MessageId)
{
    for (int i = 0; i < this->PendingCommands.Size(); i++)
    {
        if (this->PendingCommands[i]->GetId()==MessageId)
        {
            this->PendingCommands[i]->EvaluateResult(query);
            this->PendingCommands.EraseQuick(i);
        }

    }
}