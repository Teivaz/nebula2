//------------------------------------------------------------------------------
//  msg/animrewind.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/animrewind.h"

namespace Message
{
ImplementRtti(Message::AnimRewind, Message::Msg);
ImplementFactory(Message::AnimRewind);
ImplementMsgId(AnimRewind);
} // namespace Message
