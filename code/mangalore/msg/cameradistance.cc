//------------------------------------------------------------------------------
//  msg/cameradistance.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/cameradistance.h"

namespace Message
{
ImplementRtti(Message::CameraDistance, Message::Msg);
ImplementFactory(Message::CameraDistance);
ImplementMsgId(CameraDistance);
} // namespace Message
