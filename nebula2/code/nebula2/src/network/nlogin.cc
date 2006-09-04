//------------------------------------------------------------------------------
//  nLogin.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nlogin.h"
#include "network/nbuddyclient.h"


//------------------------------------------------------------------------------
/**
*/
nLogin::nLogin()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLogin::~nLogin()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool nLogin::Execute()
{
    if(this->username.IsEmpty() || this->password.IsEmpty()) return false;

    nStream xmlStream;
    xmlStream.SetFilename("MyData");
    xmlStream.OpenString("");
    xmlStream.BeginNode("rlbuddysystem");
    xmlStream.SetInt("ver",1);
    xmlStream.SetInt("id",this->uniqueID);
    xmlStream.SetString("type","set");
    xmlStream.BeginNode("login");
    xmlStream.SetString("user",this->username);
    xmlStream.SetString("password",this->password);
    xmlStream.SetString("game",this->gameGuid.Get());
    xmlStream.EndNode();
    xmlStream.EndNode();

    xmlStream.GetDocumentAsString(this->xmlRequest);

    return nBuddyClient::Instance()->SendCommand(this);
}


void nLogin::EvaluateResult(nStream& result)
{
     if (result.HasAttr("type"))
     {
         nString type = result.GetString("type");

         if (type=="result")
         {
             if (result.SetToFirstChild())
             {
                nString message = result.GetString("message");
                if (message == "userloggedin")
                {
                   nBuddyClient::Instance()->SetOnlineStatus(nBuddyClient::Online);
                }
             }

             this->curStatus = COMMAMD_RESULT;
         }
         else if(type=="error")
         {
             this->curStatus = COMMAMD_ERROR;
         }
     }
}