//------------------------------------------------------------------------------
//  msg/gfxsetvisible.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/gfxsetvisible.h"

namespace Message
{
ImplementRtti(Message::GfxSetVisible, Message::Msg);
ImplementFactory(Message::GfxSetVisible);
ImplementMsgId(GfxSetVisible);
} // namespace Message
