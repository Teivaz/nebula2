//------------------------------------------------------------------------------
//  msg/camerareset.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/camerareset.h"

namespace Message
{
ImplementRtti(Message::CameraReset, Message::Msg);
ImplementFactory(Message::CameraReset);
ImplementMsgId(CameraReset);
}; // namespace Msg
