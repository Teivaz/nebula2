#ifndef MSG_DRAGBOXREADY_H
#define MSG_DRAGBOXREADY_H
//------------------------------------------------------------------------------
/**
    @class Msg::DragBoxReady
  
    The dragbox has finished with a valid state.
    
    (C) 2006 Radon Labs GmbH
*/    
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class DragBoxReady : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(DragBoxReady);
    DeclareMsgId;
};

RegisterFactory(DragBoxReady);

}; // namespace Msg
//------------------------------------------------------------------------------
#endif

