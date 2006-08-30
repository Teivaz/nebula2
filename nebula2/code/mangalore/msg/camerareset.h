#ifndef MSG_CAMERARESET_H
#define MSG_CAMERARESET_H
//------------------------------------------------------------------------------
/**
    @class Message::CameraReset

    A camera reset message.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class CameraReset : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(CameraReset);
    DeclareMsgId;
};

RegisterFactory(CameraReset);

};
//------------------------------------------------------------------------------
#endif
