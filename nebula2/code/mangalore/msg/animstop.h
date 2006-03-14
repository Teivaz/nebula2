#ifndef MSG_ANIMSTOP_H
#define MSG_ANIMSTOP_H
//------------------------------------------------------------------------------
/**
    @class Msg::AnimStop
  
    Stop path animation playback.
    
    (C) 2005 Radon Labs GmbH
*/    
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class AnimStop : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(AnimStop);
    DeclareMsgId;
};

RegisterFactory(AnimStop);

}; // namespace Msg
//------------------------------------------------------------------------------
#endif

