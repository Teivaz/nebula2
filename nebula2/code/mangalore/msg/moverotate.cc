//------------------------------------------------------------------------------
//  msg/moverotate.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "msg/moverotate.h"

namespace Message
{
ImplementRtti(Message::MoveRotate, Message::Msg);
ImplementFactory(Message::MoveRotate);
ImplementMsgId(MoveRotate);


//------------------------------------------------------------------------------
/**
*/
MoveRotate::MoveRotate() :
	angle(0.0f)
{
}


};

