//------------------------------------------------------------------------------
//  msg/moveturn.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/moveturn.h"

namespace Message
{
ImplementRtti(Message::MoveTurn, Message::Msg);
ImplementFactory(Message::MoveTurn);
ImplementMsgId(MoveTurn);

//------------------------------------------------------------------------------
/**
*/
MoveTurn::MoveTurn() :
    dir(1.0f, 0.0f, 0.0f),
    camRelative(false)
{
    // empty
}

} // namespace Message
