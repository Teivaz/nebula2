//------------------------------------------------------------------------------
//  message/msg.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "message/msg.h"
#include "message/server.h"

namespace Message
{
ImplementRtti(Message::Msg, Foundation::RefCounted);
ImplementMsgId(Msg);

//------------------------------------------------------------------------------
/**
*/
Msg::~Msg()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Msg::SendSync(Port* port)
{
    n_assert(port);

    // still route all traffic through a central place
    Server::Instance()->SendSync(port, this);
}

//------------------------------------------------------------------------------
/**
*/
void
Msg::BroadcastSync()
{
    Server::Instance()->BroadcastSync(this);
}

//------------------------------------------------------------------------------
/**
*/
void
Msg::SendAsync(Port* port)
{
    n_assert(port);

    // still route all traffic through a central place
    Server::Instance()->SendAsync(port, this);
}

//------------------------------------------------------------------------------
/**
*/
void
Msg::BroadcastAsync()
{
    Server::Instance()->BroadcastAsync(this);
}

} // namespace Message
