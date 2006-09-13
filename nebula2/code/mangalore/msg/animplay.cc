//------------------------------------------------------------------------------
//  msg/animplay.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/animplay.h"

namespace Message
{
ImplementRtti(Message::AnimPlay, Message::Msg);
ImplementFactory(Message::AnimPlay);
ImplementMsgId(AnimPlay);
} // namespace Message
