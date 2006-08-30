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
    portArray(8, 8),
    handleMsgLockCount(0)
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

    // lock array
    this->BeginHandleMessage();

    int i;
    int num = this->portArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->portArray[i] != 0)
        {
            this->portArray[i]->Put(msg);
        }
    }

    // unlock array
    this->EndHandleMessage();
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
    // lock array
    this->BeginHandleMessage();

    int i;
    int num = this->portArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->portArray[i] != 0)
        {
            if (this->portArray[i]->Accepts(msg))
            {
                this->portArray[i]->HandleMessage(msg);
            }
        }
    }

    // unlock array
    this->EndHandleMessage();
}

//------------------------------------------------------------------------------
/**
    Attach a new message port.

    @param  port    pointer to a message port object
*/
void
Dispatcher::AttachPort(Port* port)
{
    // try to cleanup
    this->CleanupEmptyPorts();

    n_assert(port);
    // check if there is a free ptr that could be used
    int i;
    int num = this->portArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->portArray[i] == 0)
        {
            // use free element
            this->portArray[i] = port;
            return;
        }
    }

    // fallthrough: append port
    this->portArray.Append(port);
}

//------------------------------------------------------------------------------
/**
    Remove a message port object.

    Do not delete the array element, just set to 0. This makes it possible
    that while handling a msg, one of the ports could be removed without invalidating
    the loop over the array.

    @param  port    pointer to message port object to be removed
*/
void
Dispatcher::RemovePort(Port* port)
{
    nArray<Ptr<Port> >::iterator iter = this->portArray.Find(port);
    n_assert(iter);
    // set ptr to 0
    iter->operator =(0);

    // try to cleanup
    this->CleanupEmptyPorts();
}

//------------------------------------------------------------------------------
/**
*/
void
Dispatcher::CleanupEmptyPorts()
{
    if (!this->IsInHandleMessage())
    {
        int i;
        for (i = 0; i < this->portArray.Size(); /*empty*/)
        {
            if (this->portArray[i] == 0)
            {
                this->portArray.Erase(i);
            }
            else
            {
                // next
                i++;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Dispatcher::BeginHandleMessage()
{
    this->handleMsgLockCount++;
}

//------------------------------------------------------------------------------
/**
*/
void
Dispatcher::EndHandleMessage()
{
    n_assert(this->handleMsgLockCount > 0);
    this->handleMsgLockCount--;
}

//------------------------------------------------------------------------------
/**
*/
bool
Dispatcher::IsInHandleMessage() const
{
    return (this->handleMsgLockCount > 0);
}

} // namespace Message
