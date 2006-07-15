#ifndef MSG_CAMERADISTANCE_H
#define MSG_CAMERADISTANCE_H
//------------------------------------------------------------------------------
/**
    @class Message::CameraDistance
    
    Change the distance of a 3rd camera to its lookat point.
    
    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class CameraDistance : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(CameraDistance);
    DeclareMsgId;

public:
    /// constructor
    CameraDistance();
    /// set relative camera distance change (-1.0f..+1.0f)
    void SetRelativeDistanceChange(float d);
    /// get relative camera distance change
    float GetRelativeDistanceChange() const;

private:
    float relDistChange;
};

RegisterFactory(CameraDistance);

//------------------------------------------------------------------------------
/**
*/
inline
CameraDistance::CameraDistance() :
    relDistChange(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CameraDistance::SetRelativeDistanceChange(float d)
{
    this->relDistChange = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CameraDistance::GetRelativeDistanceChange() const
{
    return this->relDistChange;
}

};
//------------------------------------------------------------------------------
#endif

