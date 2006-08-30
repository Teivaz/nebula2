#ifndef MSG_MOVEGOTO_H
#define MSG_MOVEGOTO_H
//------------------------------------------------------------------------------
/**
    @class Message::MoveGoto

    A highlevel MoveGoto message. The expected behaviour is that an entity
    should move to the target point defined by the move goto message
    and stop there. If possible, the entity should steer around obstacles
    and use pathfinding to reach the target point.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class MoveGoto : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(MoveGoto);
    DeclareMsgId;

public:
    /// constructor
    MoveGoto();
    /// set the target position
    void SetPosition(const vector3& p);
    /// get the target position
    const vector3& GetPosition() const;
    /// set the distance to target in which entity stops
    void SetDistance(float f);
    /// get the distance to target in which entity stops
    float GetDistance() const;

private:
    vector3 pos;
    float distance;
};

RegisterFactory(MoveGoto);

//------------------------------------------------------------------------------
/**
*/
inline
MoveGoto::MoveGoto():
    distance(0.5f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MoveGoto::SetPosition(const vector3& p)
{
    this->pos = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
MoveGoto::GetPosition() const
{
    return this->pos;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
MoveGoto::SetDistance(float f)
{
    this->distance = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
MoveGoto::GetDistance() const
{
    return this->distance;
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
