#ifndef MSG_DRAGBOXUPDATED_H
#define MSG_DRAGBOXUPDATED_H
//------------------------------------------------------------------------------
/**
    @class Msg::DragBoxUpdated

    The dragbox is updated.

    (C) 2006 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class DragBoxUpdated : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(DragBoxUpdated);
    DeclareMsgId;
};

RegisterFactory(DragBoxUpdated);

}; // namespace Msg
//------------------------------------------------------------------------------
#endif

