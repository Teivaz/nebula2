//------------------------------------------------------------------------------
//  msg/cameraorbit.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/cameraorbit.h"

namespace Message
{
ImplementRtti(Message::CameraOrbit, Message::Msg);
ImplementFactory(Message::CameraOrbit);
ImplementMsgId(CameraOrbit);
}; // namespace Msg