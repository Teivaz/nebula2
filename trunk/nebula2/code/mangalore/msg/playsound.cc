//------------------------------------------------------------------------------
//  msg/playsound.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/playsound.h"

namespace Message
{
ImplementRtti(Message::PlaySound, Message::Msg);
ImplementFactory(Message::PlaySound);
ImplementMsgId(PlaySound);
}; // namespace Msg
