//------------------------------------------------------------------------------
//  message/port.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "message/port.h"

namespace Message
{
ImplementRtti(Message::Port, Foundation::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Port::~Port()
{
    // discard any pending messages..
}

//------------------------------------------------------------------------------
/**
    Return true if the port accepts a specific message. By default, NO
    messages are accepted.
*/
bool
Port::Accepts(Msg* msg)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Put a new message on the port's message queue.
*/
void
Port::Put(Msg* msg)
{
    n_assert(msg);
    if (this->Accepts(msg))
    {
        this->msgQueue.Append(msg);
    }
}

//------------------------------------------------------------------------------
/**
    Handle all pending messages. This will simply call the virtual
    HandleMessage() method for each message in the msgQueue and clear the
    message queue afterwards.
*/
void
Port::HandlePendingMessages()
{
    // increment ref count on me, because messages could cause removing my entity
    Ptr<Port> myself = this;

    int i;
    int num = this->msgQueue.Size();
    for (i = 0; i < num; i++)
    {
        this->HandleMessage(this->msgQueue[i]);
    }
    this->msgQueue.Clear();

    // decrement ref count
    myself = 0;
}

//------------------------------------------------------------------------------
/**
    Handle a specific message. Overwrite this method in a subclass.
    It is guaranteed that this method will only be called for messages
    which are accepted by Accept().
*/
void
Port::HandleMessage(Msg* msg)
{
    // empty
}

} // namespace Message
