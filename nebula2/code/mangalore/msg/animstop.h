#ifndef MSG_ANIMSTOP_H
#define MSG_ANIMSTOP_H
//------------------------------------------------------------------------------
/**
    @class Message::AnimStop

    Stop path animation playback.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class AnimStop : public Msg
{
    DeclareRtti;
    DeclareFactory(AnimStop);
    DeclareMsgId;
};

RegisterFactory(AnimStop);

} // namespace Message
//------------------------------------------------------------------------------
#endif
