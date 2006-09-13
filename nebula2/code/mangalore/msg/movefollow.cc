//------------------------------------------------------------------------------
//  msg/movefollow.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/movefollow.h"

namespace Message
{
ImplementRtti(Message::MoveFollow, Message::Msg);
ImplementFactory(Message::MoveFollow);
ImplementMsgId(MoveFollow);
} // namespace Message
