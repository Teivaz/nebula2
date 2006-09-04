//------------------------------------------------------------------------------
//  nSetProfileAttr.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nsetprofileattr.h"
#include "network/nbuddyclient.h"
#include "network/nSendMessage.h"


//------------------------------------------------------------------------------
/**
*/
nSetProfileAttr::nSetProfileAttr()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSetProfileAttr::~nSetProfileAttr()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool nSetProfileAttr::Execute()
{
    nSendMessage::MaskXMLChars(this->value);

    nStream xmlStream;
    xmlStream.SetFilename("MyData");
    xmlStream.OpenString("");
    xmlStream.BeginNode("rlbuddysystem");
    xmlStream.SetInt("ver",1);
    xmlStream.SetInt("id",this->uniqueID);
    xmlStream.SetString("type","set");
    xmlStream.BeginNode("setprofileattr");
    xmlStream.SetString("key",this->key);
    xmlStream.SetString("value",this->value);
    xmlStream.EndNode();
    xmlStream.EndNode();

    xmlStream.GetDocumentAsString(this->xmlRequest);

   return nBuddyClient::Instance()->SendCommand(this);
}


void nSetProfileAttr::EvaluateResult(nStream& result)
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
                if (message == "profileattrset")
                {
                   this->curStatus = COMMAMD_RESULT;
                }
             }

         }
         else if(type=="error")
         {
             this->curStatus = COMMAMD_ERROR;
         }
     }
}