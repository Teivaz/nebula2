#ifndef PROPERTIES_SIMPLECAMERAPROPERTY_H
#define PROPERTIES_SIMPLECAMERAPROPERTY_H
//------------------------------------------------------------------------------
/**
    a simple camera, moves with ADSW and can look around.
    
    (C) 2006 Radon Labs GmbH
*/
#include "properties/cameraproperty.h"

namespace Message
{
    class MoveDirection;
    class CameraOrbit;
}

//------------------------------------------------------------------------------
namespace Properties
{
class SimpleCameraProperty : public CameraProperty
{
    DeclareRtti;
	DeclareFactory(SimpleCameraProperty);

public:
    /// constructor
    SimpleCameraProperty();
    
    /// return true if message is accepted by controller
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);

    /// called before rendering happens
    virtual void OnRender();

protected:
    /// handle move direction
    void OnMoveDirection(Message::MoveDirection* msg);
    /// handle camera orbit
    void OnCameraOrbit(Message::CameraOrbit* msg);

private:
    float relHorizontalRotation;
    float relVerticalRotation;
    vector3 relMove;

    float relHorizontalRotationFactor;
    float relVerticalRotationFactor;
    float relMoveFactor;
};

RegisterFactory(SimpleCameraProperty);

}; // namespace Property
//------------------------------------------------------------------------------
#endif