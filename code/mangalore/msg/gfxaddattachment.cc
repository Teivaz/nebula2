//------------------------------------------------------------------------------
//  msg/gfxaddattachment.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/gfxaddattachment.h"

namespace Message
{
ImplementRtti(Message::GfxAddAttachment, Message::Msg);
ImplementFactory(Message::GfxAddAttachment);
ImplementMsgId(GfxAddAttachment);
};
