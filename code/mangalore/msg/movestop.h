#ifndef MSG_MOVESTOP_H
#define MSG_MOVESTOP_H
//------------------------------------------------------------------------------
/**
    @class Message::MoveStop

    A MoveStop message. The expected behaviour is that an entity which
    receives this messages stops immediately.
    
    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class MoveStop : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(MoveStop);
    DeclareMsgId;
};

RegisterFactory(MoveStop);

};
//------------------------------------------------------------------------------
#endif
