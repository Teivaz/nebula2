//------------------------------------------------------------------------------
//  message/server.cc
//  (C) 2003 RadonLabs GmbH
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
    portArray(1024, 1024)
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
    this->portArray.Append(port);
}

//------------------------------------------------------------------------------
/**
    Unregister a message port from the server. The port will no longer
    receive broadcast messages.

    @param  port    pointer to a registered Port object
*/
void
Server::UnregisterPort(Port* port)
{
    n_assert(port);
    nArray<Ptr<Port> >::iterator iter = this->portArray.Find(port);
    n_assert(iter);
    this->portArray.Erase(iter);
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
    int num = this->portArray.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        this->portArray[i]->Put(msg);
    }
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
    int num = this->portArray.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        Port* port = this->portArray[i];
        if (port->Accepts(msg))
        {
            port->HandleMessage(msg);
        }
    }
}

} // namespace Message