#ifndef MSG_ANIMPLAY_H
#define MSG_ANIMPLAY_H
//------------------------------------------------------------------------------
/**
    @class Msg::AnimPlay
  
    Start path animation playback.
    
    (C) 2005 Radon Labs GmbH
*/    
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class AnimPlay : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(AnimPlay);
    DeclareMsgId;
};

RegisterFactory(AnimPlay);

}; // namespace Msg
//------------------------------------------------------------------------------
#endif

