//------------------------------------------------------------------------------
//  msg/playshakeeffect.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/playshakeeffect.h"

namespace Message
{
ImplementRtti(Message::PlayShakeEffect, Message::Msg);
ImplementFactory(Message::PlayShakeEffect);
ImplementMsgId(PlayShakeEffect);
} // namespace Message
