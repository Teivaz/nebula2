#ifndef MSG_DRAGBOXCANCELLED_H
#define MSG_DRAGBOXCANCELLED_H
//------------------------------------------------------------------------------
/**
    @class Msg::DragBoxCancelled

    The dragbox is cancelled.

    (C) 2006 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class DragBoxCancelled : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(DragBoxCancelled);
    DeclareMsgId;
};

RegisterFactory(DragBoxCancelled);

}; // namespace Msg
//------------------------------------------------------------------------------
#endif

