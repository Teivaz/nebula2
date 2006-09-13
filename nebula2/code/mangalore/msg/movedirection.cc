//------------------------------------------------------------------------------
//  msg/movedirection.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/movedirection.h"

namespace Message
{
ImplementRtti(Message::MoveDirection, Message::Msg);
ImplementFactory(Message::MoveDirection);
ImplementMsgId(MoveDirection);
} // namespace Message
