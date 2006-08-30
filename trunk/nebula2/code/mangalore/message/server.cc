//------------------------------------------------------------------------------
//  message/server.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "message/server.h"
#include "message/port.h"

namespace Message
{
ImplementRtti(Message::Server, Foundation::RefCounted);
ImplementFactory(Message::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    portArray(1024, 1024),
    broadcastLockCount(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the message subsystem for use.
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the message subsystem after use.
*/
void
Server::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Register a message port with the server. Only registered message ports
    will receive broadcast messages.

    @param  port    pointer to a Port object
*/
void
Server::RegisterPort(Port* port)
{
    n_assert(port);
    n_assert(0 == this->portArray.Find(port));

    // try to cleanup
    this->CleanupEmptyPorts();

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

    this->portArray.Append(port);
}

//------------------------------------------------------------------------------
/**
    Unregister a message port from the server. The port will no longer
    receive broadcast messages.

    Do not delete the array element, just set to 0. This makes it possible
    that while broadcasting, one of the ports could be removed without invalidating
    the loop over the array.

    @param  port    pointer to a registered Port object
*/
void
Server::UnregisterPort(Port* port)
{
    n_assert(port);
    nArray<Ptr<Port> >::iterator iter = this->portArray.Find(port);
    n_assert(iter);
    // set ptr to 0
    iter->operator =(0);

    // try to cleanup
    this->CleanupEmptyPorts();
}

//------------------------------------------------------------------------------
/**
    Send an asynchronous message to a specific message port. The port does not
    have to be registered with the server (although it can be). Asynchronous
    sending means, the message will be added to the port's message queue,
    and the message will be processed during the port's HandlePendingMessages()
    method.

    @param  port        pointer to a port object
    @param  msg         pointer to a message object
*/
void
Server::SendAsync(Port* port, Msg* msg)
{
    n_assert(port);
    port->Put(msg);
}

//------------------------------------------------------------------------------
/**
    Broadcast an asynchronous message to all registered message ports.

    @param  msg     pointer to a message object
*/
void
Server::BroadcastAsync(Msg* msg)
{
    // lock array
    this->BeginBroadcast();

    int num = this->portArray.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (this->portArray[i] != 0)
        {
            this->portArray[i]->Put(msg);
        }
    }

    // unlock array
    this->EndBroadcast();
}

//------------------------------------------------------------------------------
/**
    Send a synchronous message to a specific message port. The port does not
    have to be registered with the server (although it can be). Synchronous
    sending means, the message will not be added to the port's message queue,
    instead the port's HandleMessage() method will be called directly.

    @param  port        pointer to a port object
    @param  msg         pointer to a message object
*/
void
Server::SendSync(Port* port, Msg* msg)
{
    n_assert(port);
    if (port->Accepts(msg))
    {
        port->HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    Broadcast a synchronous message to all registered message ports.

    @param  msg     pointer to a message object
*/
void
Server::BroadcastSync(Msg* msg)
{
    // lock array
    this->BeginBroadcast();

    int num = this->portArray.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (this->portArray[i] != 0)
        {
            Port* port = this->portArray[i];
            if (port->Accepts(msg))
            {
                port->HandleMessage(msg);
            }
        }
    }

    // unlock array
    this->EndBroadcast();
}

//------------------------------------------------------------------------------
/**
*/
void
Server::CleanupEmptyPorts()
{
    if (!this->IsInBroadcast())
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
Server::BeginBroadcast()
{
    this->broadcastLockCount++;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::EndBroadcast()
{
    n_assert(this->broadcastLockCount > 0);
    this->broadcastLockCount--;
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::IsInBroadcast() const
{
    return (this->broadcastLockCount > 0);
}

} // namespace Message
