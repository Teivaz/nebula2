#ifndef MSG_UPDATETRANSFORM_H
#define MSG_UPDATETRANSFORM_H
//------------------------------------------------------------------------------
/**
    @class Message::UpdateTransform

    Updates the transform of a entity, does not set the physics tranform.
    All property that need to update when entity transformation changes need
    listen to this messags.

    To set the transformation of a entity (including the physics) use the SetTransform message.

    (C) 2006 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class UpdateTransform : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(UpdateTransform);
    DeclareMsgId;

public:

    /// constructor
    UpdateTransform();
    /// set the target transform matrix
    void SetMatrix(const matrix44& m);
    /// get the target transform matrix
    const matrix44& GetMatrix() const;
    /// set smoothing flag
    void SetSmoothing(bool b);
    /// get smoothing flag
    bool GetSmoothing() const;

private:
    matrix44 transform;
    bool     smoothing;
};

RegisterFactory(UpdateTransform);


//------------------------------------------------------------------------------
/**
*/
inline
UpdateTransform::UpdateTransform():
    smoothing(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
UpdateTransform::SetMatrix(const matrix44& m)
{
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
UpdateTransform::GetMatrix() const
{
    return this->transform;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
UpdateTransform::SetSmoothing(bool b)
{
    this->smoothing = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
UpdateTransform::GetSmoothing() const
{
    return this->smoothing;
}

} // namespace Msg
//------------------------------------------------------------------------------
#endif
