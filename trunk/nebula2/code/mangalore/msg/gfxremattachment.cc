//------------------------------------------------------------------------------
//  msg/gfxremattachment.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/gfxremattachment.h"

namespace Message
{
ImplementRtti(Message::GfxRemAttachment, Message::Msg);
ImplementFactory(Message::GfxRemAttachment);
ImplementMsgId(GfxRemAttachment);
};
