//------------------------------------------------------------------------------
//  msg/movesetvelocity.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/movesetvelocity.h"

namespace Message
{
ImplementRtti(Message::MoveSetVelocity, Message::Msg);
ImplementFactory(Message::MoveSetVelocity);
ImplementMsgId(MoveSetVelocity);
};
