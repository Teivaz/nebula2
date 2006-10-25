//------------------------------------------------------------------------------
//  nBuddyCommandInterpreter.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nbuddycommandinterpreter.h"
#include "network/nbuddydatabase.h"
#include "network/nusercontroller.h"
#include "network/nbuddyserver.h"
#include "util/nmd5.h"
#include "network/nSendMessage.h"

nNebulaClass(nBuddyCommandInterpreter, "nroot");

//------------------------------------------------------------------------------
/**
*/
nBuddyCommandInterpreter::nBuddyCommandInterpreter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nBuddyCommandInterpreter::~nBuddyCommandInterpreter()
{
    // empty
}


//------------------------------------------------------------------------------
/**
    Executes a query and returns the result
*/
void nBuddyCommandInterpreter::Execute(nString& strResult,nStream& query,int& ClientID)
{
    nString Result;
    int MessageID = 0;
    if (query.HasAttr("ver"))
    {
        // version 1.0 of command system
        if (query.GetFloat("ver")==1.0)
        {
             if (query.HasAttr("id"))
             {
                 MessageID = query.GetInt("id");
             }

             if (query.HasAttr("type"))
             {
                 nString type = query.GetString("type");

                 // message to an user
                 if (type=="message")
                 {
                     query.SetToFirstChild();
                     TransferMessage(strResult,query,ClientID,MessageID);
                 }
                 // a get query
                 else if (type=="get")
                 {
                     if (query.SetToFirstChild())
                     {
                        nString queryname = query.GetCurrentNodeName();

                        if (queryname == "getbuddylist")
                        {
                            this->GetBuddyList(strResult,query,ClientID,MessageID);
                        }
                        else if (queryname == "getprofileattr")
                        {
                            this->GetProfileAttr(strResult,query,ClientID,MessageID);
                        }
                     }
                 }
                 // a set query
                 else if (type=="set")
                 {
                     if (query.SetToFirstChild())
                     {
                        nString queryname = query.GetCurrentNodeName();

                        if (queryname == "createuser")
                        {
                            this->CreateUser(strResult,query,MessageID);
                        }
                        else if (queryname == "login")
                        {
                            this->LoginUser(strResult,query,ClientID,MessageID);
                        }
                        else if (queryname == "addbuddy")
                        {
                            this->AddBuddy(strResult,query,ClientID,MessageID);
                        }
                        else if (queryname == "setprofileattr")
                        {
                            this->SetProfileAttr(strResult,query,ClientID,MessageID);
                        }

                     }
                 }
             }
        }
    }
}



void nBuddyCommandInterpreter::GenerateNotLoggedInMessage(nStream& result,int& MessageID)
{
    result.BeginNode("rlbuddysystem");
    result.SetInt("ver",1);
    result.SetInt("id",MessageID);
    result.SetString("type","error");
    result.BeginNode("servermessage");
    result.SetString("message","notloggedin");
    result.EndNode();
    result.EndNode();
}

void nBuddyCommandInterpreter::CreateUser(nString& strResult,nStream& query,int& MessageID)
{
    nStream result;
    result.SetFilename("MyData");
    result.OpenString("");

    nString name = query.GetString("user");
    nString password = query.GetString("password");

    if (nBuddyDatabase::Instance()->DoesUserExist(name))
    {
        result.BeginNode("rlbuddysystem");
        result.SetInt("ver",1);
        result.SetInt("id",MessageID);
        result.SetString("type","error");
        result.BeginNode("servermessage");
        result.SetString("message","useralreadyexists");
        result.EndNode();
        result.EndNode();
    }
    else
    {
        if (nBuddyDatabase::Instance()->CreateUser(name,password))
        {
            result.BeginNode("rlbuddysystem");
            result.SetInt("ver",1);
            result.SetInt("id",MessageID);
            result.SetString("type","result");
            result.BeginNode("servermessage");
            result.SetString("message","usercreated");
            result.EndNode();
            result.EndNode();
        }
    }

    result.GetDocumentAsString(strResult);
}


void nBuddyCommandInterpreter::LoginUser(nString& strResult,nStream& query,int& ClientID,int& MessageID)
{
    // is this connection already logged in ?
    nUserContext* context;
    if (context = nUserController::Instance()->GetUserContext(ClientID))
    {
        nUserController::Instance()->DeleteUser(ClientID);
    }

    nStream result;
    result.SetFilename("MyData");
    result.OpenString("");

    nString name = query.GetString("user");
    nString password = query.GetString("password");
    nString gameGuid = query.GetString("game");

    if (!nBuddyDatabase::Instance()->DoesUserExist(name))
    {
        result.BeginNode("rlbuddysystem");
        result.SetInt("ver",1);
        result.SetInt("id",MessageID);
        result.SetString("type","error");
        result.BeginNode("servermessage");
        result.SetString("message","userdoesnotexists");
        result.EndNode();
        result.EndNode();
    }
    else if (!nBuddyDatabase::Instance()->DoesGameExist(gameGuid))
    {
        result.BeginNode("rlbuddysystem");
        result.SetInt("ver",1);
        result.SetInt("id",MessageID);
        result.SetString("type","error");
        result.BeginNode("servermessage");
        result.SetString("message","gamedoesnotexists");
        result.EndNode();
        result.EndNode();
    }
    else
    {
        // is user already logged in ?
        if (nUserController::Instance()->DeleteUser(name))
        {
            //found
        }

        nString databasePassword;

        if (nBuddyDatabase::Instance()->GetUserPassword(name,databasePassword))
        {
            nMD5 alg;
            nString encodedPassword = alg.String2MD5(password.Get());

            if (databasePassword == encodedPassword)
            {
                result.BeginNode("rlbuddysystem");
                result.SetInt("ver",1);
                result.SetInt("id",MessageID);
                result.SetString("type","result");
                result.BeginNode("servermessage");
                result.SetString("message","userloggedin");
                result.EndNode();
                result.EndNode();

                // generates / updates game profil
                nBuddyDatabase::Instance()->UpdateUserGameProfile(name,gameGuid);

                nGuid game;
                game.Set(gameGuid.Get());
                nUserController::Instance()->AddUser(ClientID,name,game);
            }
            else
            {
                result.BeginNode("rlbuddysystem");
                result.SetInt("ver",1);
                result.SetInt("id",MessageID);
                result.SetString("type","error");
                result.BeginNode("servermessage");
                result.SetString("message","wrongpassword");
                result.EndNode();
                result.EndNode();
            }
        }
        else
        {
                result.BeginNode("rlbuddysystem");
                result.SetInt("ver",1);
                result.SetInt("id",MessageID);
                result.SetString("type","error");
                result.BeginNode("servermessage");
                result.SetString("message","databaseerror");
                result.EndNode();
                result.EndNode();
        }
    }

    result.GetDocumentAsString(strResult);
}


void nBuddyCommandInterpreter::TransferMessage(nString& strResult,nStream& query,int& ClientID,int& MessageID)
{
    nStream result;
    result.SetFilename("MyData");
    result.OpenString("");


    nString text = query.GetString("text");

    if (text.IsEmpty() || text.Length() > BUDDYMESSAGE_MAX_LENGTH)
    {
        result.BeginNode("rlbuddysystem");
        result.SetInt("ver",1);
        result.SetInt("id",MessageID);
        result.SetString("type","error");
        result.BeginNode("servermessage");
        result.SetString("message","textlength");
        result.EndNode();
        result.EndNode();

        result.GetDocumentAsString(strResult);
        return;
    }



    nUserContext* context;
    if (context = nUserController::Instance()->GetUserContext(ClientID))
    {
        nString name = query.GetString("user");

        int id=0;
        if (nUserController::Instance()->GetClientID(name,id))
        {
            nSendMessage::MaskXMLChars(text);
            nStream messageStream;
            messageStream.SetFilename("MyData");
            messageStream.OpenString("");
            messageStream.BeginNode("rlbuddysystem");
            messageStream.SetInt("ver",1);
            messageStream.SetString("type","update");
            messageStream.BeginNode("message");
            messageStream.SetString("user",context->GetUserName());
            messageStream.SetString("text",text);
            messageStream.EndNode();
            messageStream.EndNode();

            nString message;
            messageStream.GetDocumentAsString(message);
            message.UTF8toANSI();

            if (nBuddyServer::Instance()->SendMessage(id,message))
            {
                result.BeginNode("rlbuddysystem");
                result.SetInt("ver",1);
                result.SetInt("id",MessageID);
                result.SetString("type","result");
                result.BeginNode("servermessage");
                result.SetString("message","messagesend");
                result.EndNode();
                result.EndNode();
            }
            else
            {
                result.BeginNode("rlbuddysystem");
                result.SetInt("ver",1);
                result.SetInt("id",MessageID);
                result.SetString("type","error");
                result.BeginNode("servermessage");
                result.SetString("message","messagesfailed");
                result.EndNode();
                result.EndNode();
            }
        }
        else
        {
            result.BeginNode("rlbuddysystem");
            result.SetInt("ver",1);
            result.SetInt("id",MessageID);
            result.SetString("type","error");
            result.BeginNode("servermessage");
            result.SetString("message","usernotonline");
            result.EndNode();
            result.EndNode();
        }


    }
    else
    {
        this->GenerateNotLoggedInMessage(result,MessageID);
    }

    result.GetDocumentAsString(strResult);
}



void nBuddyCommandInterpreter::AddBuddy(nString& strResult,nStream& query,int& ClientID,int& MessageID)
{
    nStream result;
    result.SetFilename("MyData");
    result.OpenString("");

    nUserContext* context;

    if (context = nUserController::Instance()->GetUserContext(ClientID))
    {
        nString user = context->GetUserName();
        nString buddy = query.GetString("user");

        if (nBuddyDatabase::Instance()->AddBuddy(user,buddy))
        {
            result.BeginNode("rlbuddysystem");
            result.SetInt("ver",1);
            result.SetInt("id",MessageID);
            result.SetString("type","result");
            result.BeginNode("servermessage");
            result.SetString("message","buddyadded");
            result.EndNode();
            result.EndNode();
        }
        else
        {
            result.BeginNode("rlbuddysystem");
            result.SetInt("ver",1);
            result.SetInt("id",MessageID);
            result.SetString("type","error");
            result.BeginNode("servermessage");
            result.SetString("message","userdoesnotexist");
        }
    }
    else
    {
        this->GenerateNotLoggedInMessage(result,MessageID);
    }

    result.GetDocumentAsString(strResult);
}

void nBuddyCommandInterpreter::GetBuddyList(nString& strResult,nStream& query,int& ClientID,int& MessageID)
{
    nStream result;
    result.SetFilename("MyData");
    result.OpenString("");

    nUserContext* context;

    if (context = nUserController::Instance()->GetUserContext(ClientID))
    {
        nString user = context->GetUserName();
        nArray<nString> buddylist;
        nBuddyDatabase::Instance()->GetBuddyList(user,buddylist);

        result.BeginNode("rlbuddysystem");
        result.SetInt("ver",1);
        result.SetInt("id",MessageID);
        result.SetString("type","result");
        result.BeginNode("servermessage");
        result.SetString("message","buddylist");

        for (int i = 0; i < buddylist.Size(); i++)
        {
            result.BeginNode("buddy");
            result.SetString("name",buddylist[i]);
            result.EndNode();
        }

        result.EndNode();
        result.EndNode();

    }
    else
    {
        this->GenerateNotLoggedInMessage(result,MessageID);
    }

    result.GetDocumentAsString(strResult);

}

void nBuddyCommandInterpreter::SetProfileAttr(nString& strResult,nStream& query,int& ClientID,int& MessageID)
{
    nStream result;
    result.SetFilename("MyData");
    result.OpenString("");

    nUserContext* context;

    if (context = nUserController::Instance()->GetUserContext(ClientID))
    {
        nString user = context->GetUserName();
        nString game = context->GetGameGuid().Get();
        nString key = query.GetString("key");
        nString value = query.GetString("value");

        if (nBuddyDatabase::Instance()->SetProfileAttr(user,game,key,value))
        {
            result.BeginNode("rlbuddysystem");
            result.SetInt("ver",1);
            result.SetInt("id",MessageID);
            result.SetString("type","result");
            result.BeginNode("servermessage");
            result.SetString("message","profileattrset");
            result.EndNode();
            result.EndNode();
        }
        else
        {
            result.BeginNode("rlbuddysystem");
            result.SetInt("ver",1);
            result.SetInt("id",MessageID);
            result.SetString("type","error");
            result.BeginNode("servermessage");
            result.SetString("message","profileattrfailed");
        }
    }
    else
    {
        this->GenerateNotLoggedInMessage(result,MessageID);
    }

    result.GetDocumentAsString(strResult);
}


void nBuddyCommandInterpreter::GetProfileAttr(nString& strResult,nStream& query,int& ClientID,int& MessageID)
{
    nStream result;
    result.SetFilename("MyData");
    result.OpenString("");

    nUserContext* context;

    if (context = nUserController::Instance()->GetUserContext(ClientID))
    {
        nString user = context->GetUserName();
        nString game = context->GetGameGuid().Get();
        nString key = query.GetString("key");
        nString value;

        if (nBuddyDatabase::Instance()->GetProfileAttr(user,game,key,value))
        {
            nSendMessage::MaskXMLChars(value);

            result.BeginNode("rlbuddysystem");
            result.SetInt("ver",1);
            result.SetInt("id",MessageID);
            result.SetString("type","result");
            result.BeginNode("servermessage");
            result.SetString("message",key.Get());
            result.SetString("value",value.Get());
            result.EndNode();
            result.EndNode();
        }
        else
        {
            result.BeginNode("rlbuddysystem");
            result.SetInt("ver",1);
            result.SetInt("id",MessageID);
            result.SetString("type","error");
            result.BeginNode("servermessage");
            result.SetString("message","nosuchattr");
        }
    }
    else
    {
        this->GenerateNotLoggedInMessage(result,MessageID);
    }

    result.GetDocumentAsString(strResult);
}