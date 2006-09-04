#ifndef N_SENDMESSAGE_H
#define N_SENDMESSAGE_H



#define BUDDYMESSAGE_MAX_LENGTH 400

//------------------------------------------------------------------------------
/**
    @class nSendMessage
    @ingroup Network

    send message to buddy command for buddyclient

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "network/ncommand.h"


//------------------------------------------------------------------------------
class nSendMessage : public nCommand
{
public:
    /// constructor
    nSendMessage();
    /// destructor
   ~nSendMessage();

   bool Execute();
   void EvaluateResult(nStream& result);

   void SetBuddy(nString& b);
   void SetMessageText(nString& t);

   static void MaskXMLChars(nString& message);

private:

   nString buddy;
   nString messageText;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSendMessage::SetBuddy(nString& b)
{
    this->buddy = b;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nSendMessage::SetMessageText(nString& t)
{
    this->messageText = t;
}



#endif