//------------------------------------------------------------------------------
//  nGetProfileAttr.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/ngetprofileattr.h"
#include "network/nbuddyclient.h"


//------------------------------------------------------------------------------
/**
*/
nGetProfileAttr::nGetProfileAttr()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGetProfileAttr::~nGetProfileAttr()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool nGetProfileAttr::Execute()
{
    nStream xmlStream;
    xmlStream.SetFilename("MyData");
    xmlStream.OpenString("");
    xmlStream.BeginNode("rlbuddysystem");
    xmlStream.SetInt("ver",1);
    xmlStream.SetInt("id",this->uniqueID);
    xmlStream.SetString("type","get");
    xmlStream.BeginNode("getprofileattr");
    xmlStream.SetString("key",this->key);
    xmlStream.EndNode();
    xmlStream.EndNode();

    xmlStream.GetDocumentAsString(this->xmlRequest);

    return nBuddyClient::Instance()->SendCommand(this);
}


void nGetProfileAttr::EvaluateResult(nStream& result)
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
                if (message == this->key)
                {
                   this->value = result.GetString("value");
                   this->curStatus = COMMAMD_RESULT;
                }
             }

         }
         else if (type=="error")
         {
             this->curStatus = COMMAMD_ERROR;

             if (result.SetToFirstChild())
             {
                nString message = result.GetString("message");
                if (message == "nosuchattr")
                {
                   this->curStatus = COMMAMD_ERROR;
                }
             }
         }
     }
}