//------------------------------------------------------------------------------
//  msg/updatetransform.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/updatetransform.h"

namespace Message
{
ImplementRtti(Message::UpdateTransform, Message::Msg);
ImplementFactory(Message::UpdateTransform);
ImplementMsgId(UpdateTransform);
} // namespace Message
