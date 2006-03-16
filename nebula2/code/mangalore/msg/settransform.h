#ifndef MSG_SETTRANSFORM_H
#define MSG_SETTRANSFORM_H
//------------------------------------------------------------------------------
/**
    @class Message::SetTransform
    
    Set the complete transform of a entity, including the  physics tranform.
    Use not per Frame, the physics won't like it!
    
    (C) 2005 Radon Labs GmbH
*/
#include "msg/updatetransform.h"

//------------------------------------------------------------------------------
namespace Message
{
class SetTransform : public UpdateTransform
{
    DeclareRtti;
	DeclareFactory(SetTransform);
    DeclareMsgId;
};

RegisterFactory(SetTransform);

}; // namespace Msg
//------------------------------------------------------------------------------
#endif
