#ifndef MSG_DRAGBOXSTARTED_H
#define MSG_DRAGBOXSTARTED_H
//------------------------------------------------------------------------------
/**
    @class Msg::DragBoxStarted

    The dragbox is started.

    (C) 2006 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class DragBoxStarted : public Msg
{
    DeclareRtti;
    DeclareFactory(DragBoxStarted);
    DeclareMsgId;
};

RegisterFactory(DragBoxStarted);

} // namespace Msg
//------------------------------------------------------------------------------
#endif

