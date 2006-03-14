//------------------------------------------------------------------------------
//  message/dispatcher.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "message/dispatcher.h"
#include "message/port.h"

namespace Message
{
ImplementRtti(Message::Dispatcher, Message::Port);
ImplementFactory(Message::Dispatcher);

//------------------------------------------------------------------------------
/**
*/
Dispatcher::Dispatcher() :
    portArray(8, 8)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    The dispatcher's Accepts() method accepts all incoming messages,
    each attached port will decide on its own whether it will handle
    the message or not.
*/
bool
Dispatcher::Accepts(Message::Msg* msg)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Forward the Msg object to all attached Handlers which are interested
    in the message. The message will not be stored inside the Dispatcher.

    @param  msg     pointer to a message object
*/
void
Dispatcher::Put(Msg* msg)
{
    n_assert(msg);
    int i;
    int num = this->portArray.Size();
    for (i = 0; i < num; i++)
    {
        this->portArray[i]->Put(msg);
    }
}

//------------------------------------------------------------------------------
/**
    Handle a specific message. Overwrite this method in a subclass.
    It is guaranteed that this method will only be called for messages
    which are accepted by Accept().
*/
void
Dispatcher::HandleMessage(Msg* msg)
{
    int i;
    int num = this->portArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->portArray[i]->Accepts(msg))
        {
            this->portArray[i]->HandleMessage(msg);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Attach a new message port.

    @param  port    pointer to a message port object
*/
void
Dispatcher::AttachPort(Port* port)
{
    n_assert(port);
    this->portArray.Append(port);
}

//------------------------------------------------------------------------------
/**
    Remove a message port object.

    @param  handler     pointer to message port object to be removed
*/
void
Dispatcher::RemovePort(Port* port)
{
    nArray<Ptr<Port> >::iterator iter = this->portArray.Find(port);
    n_assert(iter);
    this->portArray.Erase(iter);
}

} // namespace Message