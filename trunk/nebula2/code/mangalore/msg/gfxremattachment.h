#ifndef MSG_GFXREMATTACHMENT_H
#define MSG_GFXREMATTACHMENT_H
//------------------------------------------------------------------------------
/**
    @class Message::GfxRemAttachment

    Remove an attachment from a joint.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class GfxRemAttachment : public Msg
{
    DeclareRtti;
    DeclareFactory(GfxRemAttachment);
    DeclareMsgId;

public:
    /// set joint name
    void SetJointName(const nString& n);
    /// get joint name
    const nString& GetJointName() const;

private:
    nString jointName;
};

RegisterFactory(GfxRemAttachment);

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxRemAttachment::SetJointName(const nString& n)
{
    this->jointName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GfxRemAttachment::GetJointName() const
{
    return this->jointName;
}
} // namespace Message
//------------------------------------------------------------------------------
#endif
