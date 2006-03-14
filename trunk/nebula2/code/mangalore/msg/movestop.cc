//------------------------------------------------------------------------------
//  msg/movestop.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/movestop.h"

namespace Message
{
ImplementRtti(Message::MoveStop, Message::Msg);
ImplementFactory(Message::MoveStop);
ImplementMsgId(MoveStop);
};
