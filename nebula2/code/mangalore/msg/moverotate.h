#ifndef MSG_MOVEROTATE_H
#define MSG_MOVEROTATE_H
//------------------------------------------------------------------------------
/**
    @class Message::MoveRotate

    Commands an entity rotate around the y-axis for a new heading.
    
    (C) 2006 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class MoveRotate : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(MoveRotate);
    DeclareMsgId;

public:
    /// constructor
    MoveRotate();
    /// set the rotation angle (in rad)
    void SetAngle(float angle);
    /// get the rotation angle (in rad)
    float GetAngle() const;

private:
    float angle;
};

RegisterFactory(MoveRotate);

//------------------------------------------------------------------------------
/**
*/
inline
void
MoveRotate::SetAngle(float a)
{
    this->angle = a;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
MoveRotate::GetAngle() const
{
    return this->angle;
}

}; // namespace Msg
//------------------------------------------------------------------------------
#endif

