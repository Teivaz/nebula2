//------------------------------------------------------------------------------
//  nGetBuddylist.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/ngetbuddylist.h"
#include "network/nbuddyclient.h"


//------------------------------------------------------------------------------
/**
*/
nGetBuddylist::nGetBuddylist()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGetBuddylist::~nGetBuddylist()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool nGetBuddylist::Execute()
{

    nStream xmlStream;
    xmlStream.SetFilename("MyData");
    xmlStream.OpenString("");
    xmlStream.BeginNode("rlbuddysystem");
    xmlStream.SetInt("ver",1);
    xmlStream.SetInt("id",this->uniqueID);
    xmlStream.SetString("type","get");
    xmlStream.BeginNode("getbuddylist");
    xmlStream.EndNode();
    xmlStream.EndNode();

    xmlStream.GetDocumentAsString(this->xmlRequest);

    return nBuddyClient::Instance()->SendCommand(this);
}


void nGetBuddylist::EvaluateResult(nStream& result)
{
     this->curStatus = COMMAMD_ERROR;

     if (result.HasAttr("type"))
     {
         nString type = result.GetString("type");

         if (type=="result")
         {
             if (result.SetToFirstChild())
             {
                if (result.HasAttr("message"))
                {
                    nString message = result.GetString("message");
                    if (message == "buddylist")
                    {
                         this->BuddyList.Clear();

                         if (result.SetToFirstChild("buddy"))
                         {
                             do
                             {
                                 if (result.HasAttr("name"))
                                 {
                                     this->BuddyList.PushBack(result.GetString("name"));
                                 }

                             } while (result.SetToNextChild("buddy"));

                         }

                         this->curStatus = COMMAMD_RESULT;
                    }
                }
             }

         }
         else if (type=="error")
         {
             this->curStatus = COMMAMD_ERROR;
         }
     }
}