#ifndef N_MESSAGE_H
#define N_MESSAGE_H
//------------------------------------------------------------------------------
/**
    @class nMessage
    @ingroup Network

    messsage update object

    (C) 2006 RadonLabs GmbH
*/
#include "network/nupdate.h"


//------------------------------------------------------------------------------
class nMessage : public nUpdate
{

public:


    /// constructor
    nMessage();
    /// destructor
    virtual ~nMessage();

    nString& GetMessageText();
    nString& GetBuddyName();

protected:

    friend class nServerMessageInterpreter;

    nString messageText;
    nString buddyName;

};


//------------------------------------------------------------------------------
/**
*/
inline
nString&
nMessage::GetBuddyName()
{
    return this->buddyName;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString&
nMessage::GetMessageText()
{
    return this->messageText;
}

#endif