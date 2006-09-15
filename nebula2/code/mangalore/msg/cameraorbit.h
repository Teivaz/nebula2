#ifndef MSG_CAMERAORBIT_H
#define MSG_CAMERAORBIT_H
//------------------------------------------------------------------------------
/**
    @class Message::CameraOrbit

    A camera orbit rotation message.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class CameraOrbit : public Msg
{
    DeclareRtti;
    DeclareFactory(CameraOrbit);
    DeclareMsgId;

public:
    /// constructor
    CameraOrbit();
    /// set horizontal rotation angle in degree
    void SetHorizontalRotation(float a);
    /// get horizontal rotation angle in degree
    float GetHorizontalRotation() const;
    /// set vertical rotation angle in degree
    void SetVerticalRotation(float a);
    /// get vertical rotation angle in degree
    float GetVerticalRotation() const;

private:
    float horiAngle;
    float vertAngle;
};

RegisterFactory(CameraOrbit);

//------------------------------------------------------------------------------
/**
*/
inline
CameraOrbit::CameraOrbit() :
    horiAngle(0.0f),
    vertAngle(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CameraOrbit::SetHorizontalRotation(float a)
{
    this->horiAngle = a;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CameraOrbit::GetHorizontalRotation() const
{
    return this->horiAngle;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CameraOrbit::SetVerticalRotation(float a)
{
    this->vertAngle = a;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CameraOrbit::GetVerticalRotation() const
{
    return this->vertAngle;
}

};
//------------------------------------------------------------------------------
#endif
