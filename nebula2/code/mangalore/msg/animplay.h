#ifndef MSG_ANIMPLAY_H
#define MSG_ANIMPLAY_H
//------------------------------------------------------------------------------
/**
    @class Message::AnimPlay

    Start path animation playback.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class AnimPlay : public Msg
{
    DeclareRtti;
    DeclareFactory(AnimPlay);
    DeclareMsgId;
};

RegisterFactory(AnimPlay);

} // namespace Message
//------------------------------------------------------------------------------
#endif
