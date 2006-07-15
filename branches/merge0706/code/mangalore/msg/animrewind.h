#ifndef MSG_ANIMREWIND_H
#define MSG_ANIMREWIND_H
//------------------------------------------------------------------------------
/**
    @class Message::AnimRewind

    Rewind path animation.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class AnimRewind : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(AnimRewind);
    DeclareMsgId;
};

RegisterFactory(AnimRewind);

} // namespace Message
//------------------------------------------------------------------------------
#endif
