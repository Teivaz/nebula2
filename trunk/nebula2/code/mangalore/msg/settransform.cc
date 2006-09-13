//------------------------------------------------------------------------------
//  msg/settransform.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/settransform.h"

namespace Message
{
ImplementRtti(Message::SetTransform, Message::Msg);
ImplementFactory(Message::SetTransform);
ImplementMsgId(SetTransform);
} // namespace Message
