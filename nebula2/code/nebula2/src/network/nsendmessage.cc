//------------------------------------------------------------------------------
//  nSendMessage.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nSendMessage.h"
#include "network/nbuddyclient.h"


//------------------------------------------------------------------------------
/**
*/
nSendMessage::nSendMessage()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSendMessage::~nSendMessage()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool nSendMessage::Execute()
{
    if (messageText.IsEmpty()) return false;

    this->MaskXMLChars(this->messageText);

    nStream xmlStream;
    xmlStream.SetFilename("MyData");
    xmlStream.OpenString("");
    xmlStream.BeginNode("rlbuddysystem");
    xmlStream.SetInt("ver",1);
    xmlStream.SetInt("id",this->uniqueID);
    xmlStream.SetString("type","message");
    xmlStream.BeginNode("login");
    xmlStream.SetString("user",this->buddy);
    xmlStream.SetString("text",nString(this->messageText));
    xmlStream.EndNode();
    xmlStream.EndNode();

    xmlStream.GetDocumentAsString(this->xmlRequest);

    return nBuddyClient::Instance()->SendCommand(this);
}


void nSendMessage::EvaluateResult(nStream& result)
{
     if (result.HasAttr("type"))
     {
         nString type = result.GetString("type");

         if (type=="result")
         {
             if (result.SetToFirstChild())
             {
                nString message = result.GetString("message");
                if (message == "messagesend")
                {
                     this->curStatus = COMMAMD_RESULT;
                }
             }

         }
         else if (type=="error")
         {
             this->curStatus = COMMAMD_ERROR;
         }
     }
}



void nSendMessage::MaskXMLChars(nString& message)
{
    /* xml masks:
    < &lt;
    > &gt;
    & &amp;
    " &quot;
    ' &apos; */

    nString new_message;
    for (int i = 0; i < message.Length(); i++)
    {
        char current_str[2];
        current_str[0] = message[i];
        current_str[1] = 0;

        if (message[i]=='<') new_message.Append("&lt;");
        else if (message[i]=='>') new_message.Append("&gt;");
        else if (message[i]=='&') new_message.Append("&amp;");
        else if (message[i]=='"') new_message.Append("&quot;");
        else if (message[i]=='\'') new_message.Append("&apos;");
        else new_message.Append(current_str);
    }

    message = new_message;
}





