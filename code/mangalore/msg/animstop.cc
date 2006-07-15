//------------------------------------------------------------------------------
//  msg/animstop.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/animstop.h"

namespace Message
{
ImplementRtti(Message::AnimStop, Message::Msg);
ImplementFactory(Message::AnimStop);
ImplementMsgId(AnimStop);
};
