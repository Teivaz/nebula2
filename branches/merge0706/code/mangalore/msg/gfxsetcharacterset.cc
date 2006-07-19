//------------------------------------------------------------------------------
//  msg/gfxsetcharacterset.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/gfxsetcharacterset.h"

namespace Message
{
ImplementRtti(Message::GfxSetCharacterSet, Message::Msg);
ImplementFactory(Message::GfxSetCharacterSet);
ImplementMsgId(GfxSetCharacterSet);
};
