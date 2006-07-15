#ifndef MSG_MOVESETVELOCITY_H
#define MSG_MOVESETVELOCITY_H
//------------------------------------------------------------------------------
/**
    @class Message::MoveSetVelocity

    Set the (relative) linear velocity of an entity between 0.0 and 1.0.
    The actual resulting velocity also depends on the MaxVelocity
    attribute attached to the entity.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"
#include "game/entity.h"

//------------------------------------------------------------------------------
namespace Message
{
class MoveSetVelocity : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(MoveSetVelocity);
    DeclareMsgId;

public:
    /// constructor
    MoveSetVelocity();
    /// set relative velocity (usually between 0 and 1)
    void SetRelVelocity(float v);
    /// get relative velocity
    float GetRelVelocity() const;

private:
    float relVelocity;
};

RegisterFactory(MoveSetVelocity);

//------------------------------------------------------------------------------
/**
*/
inline
MoveSetVelocity::MoveSetVelocity() :
    relVelocity(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MoveSetVelocity::SetRelVelocity(float v)
{
    this->relVelocity = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
MoveSetVelocity::GetRelVelocity() const
{
    return this->relVelocity;
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
