//------------------------------------------------------------------------------
//  nAddBuddy.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nAddBuddy.h"
#include "network/nbuddyclient.h"


//------------------------------------------------------------------------------
/**
*/
nAddBuddy::nAddBuddy()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAddBuddy::~nAddBuddy()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool nAddBuddy::Execute()
{
    nStream xmlStream;
    xmlStream.SetFilename("MyData");
    xmlStream.OpenString("");
    xmlStream.BeginNode("rlbuddysystem");
    xmlStream.SetInt("ver",1);
    xmlStream.SetInt("id",this->uniqueID);
    xmlStream.SetString("type","set");
    xmlStream.BeginNode("addbuddy");
    xmlStream.SetString("user",this->buddy);
    xmlStream.EndNode();
    xmlStream.EndNode();

    xmlStream.GetDocumentAsString(this->xmlRequest);

    return nBuddyClient::Instance()->SendCommand(this);
}


void nAddBuddy::EvaluateResult(nStream& result)
{
     this->curStatus = COMMAMD_ERROR;

     if (result.HasAttr("type"))
     {
         nString type = result.GetString("type");

         if (type=="result")
         {
             if (result.SetToFirstChild())
             {
                nString message = result.GetString("message");
                if (message == "buddyadded")
                {
                   this->curStatus = COMMAMD_RESULT;
                }
             }

         }
         else if(type=="error")
         {
             this->curStatus = COMMAMD_ERROR;

             if (result.SetToFirstChild())
             {
                nString message = result.GetString("message");
                if (message == "userdoesnotexist")
                {
                   this->curStatus = COMMAMD_ERROR;
                }
             }
         }
     }
}