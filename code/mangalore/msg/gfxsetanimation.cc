//------------------------------------------------------------------------------
//  msg/gfxsetanimation.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/gfxsetanimation.h"

namespace Message
{
ImplementRtti(Message::GfxSetAnimation, Message::Msg);
ImplementFactory(Message::GfxSetAnimation);
ImplementMsgId(GfxSetAnimation);
};
