#ifndef MSG_MOVETURN_H
#define MSG_MOVETURN_H
//------------------------------------------------------------------------------
/**
    @class Message::MoveTurn

    Commands an entity to turn into the specified direction defined
    by a 3d vector. The direction vector can be absolute or camera relative.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class MoveTurn : public Msg
{
    DeclareRtti;
    DeclareFactory(MoveTurn);
    DeclareMsgId;

public:
    /// constructor
    MoveTurn();
    /// set the direction vector
    void SetDirection(const vector3& v);
    /// get the direction vector
    const vector3& GetDirection() const;
    /// set camera relative flag
    void SetCameraRelative(bool b);
    /// get camera relative flag
    bool GetCameraRelative() const;

private:
    vector3 dir;
    bool camRelative;
};

RegisterFactory(MoveTurn);

//------------------------------------------------------------------------------
/**
*/
inline
void
MoveTurn::SetDirection(const vector3& v)
{
    this->dir = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
MoveTurn::GetDirection() const
{
    return this->dir;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MoveTurn::SetCameraRelative(bool b)
{
    this->camRelative = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
MoveTurn::GetCameraRelative() const
{
    return this->camRelative;
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
