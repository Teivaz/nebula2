#ifndef PROPERTIES_CHASECAMERAPROPERTY_H
#define PROPERTIES_CHASECAMERAPROPERTY_H
//------------------------------------------------------------------------------
/**
    A chase camera for 3rd person camera control.
    
    (C) 2005 Radon Labs GmbH
*/
#include "properties/cameraproperty.h"
#include "mathlib/polar.h"
#include "util/npfeedbackloop.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
namespace Attr
{
    // chase camera specific attributes
    DeclareFloat(CameraDistance);
    DeclareFloat(CameraMinDistance);
    DeclareFloat(CameraMaxDistance);
    DeclareFloat(CameraDistanceStep);
    DeclareVector3(CameraOffset);
    DeclareFloat(CameraLowStop);
    DeclareFloat(CameraHighStop);
    DeclareFloat(CameraAngularVelocity);
    DeclareFloat(CameraLinearGain);
    DeclareFloat(CameraAngularGain);
    DeclareFloat(CameraDefaultTheta);
};

//------------------------------------------------------------------------------
namespace Properties
{
class ChaseCameraProperty : public CameraProperty
{
    DeclareRtti;
	DeclareFactory(ChaseCameraProperty);

public:
    /// constructor
    ChaseCameraProperty();
    /// destructor
    virtual ~ChaseCameraProperty();
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called when camera focus is obtained
    virtual void OnObtainFocus();
    /// called before camera is "rendered"
    virtual void OnRender();
    /// return true if message is accepted by controller
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);

protected:
    /// do a collision check
    vector3 DoCollideCheck(const vector3& from, const vector3& to);
    /// handle a camera reset message
    virtual void HandleCameraReset();
    /// handle a camera orbit message
    virtual void HandleCameraOrbit(float dRho, float dTheta);
    /// handle a camera distance change
    virtual void HandleCameraDistanceChange(float z);
    /// update the camera matrix
    virtual void UpdateCamera();

    polar2 cameraAngles;
    float cameraDistance;

    nPFeedbackLoop<vector3> cameraPos;
    nPFeedbackLoop<vector3> cameraLookat;
};

RegisterFactory(ChaseCameraProperty);

}; // namespace Property
//------------------------------------------------------------------------------
#endif