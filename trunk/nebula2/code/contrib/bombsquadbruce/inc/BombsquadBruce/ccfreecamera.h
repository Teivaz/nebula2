#ifndef CC_FREECAMERA_H
#define CC_FREECAMERA_H
//------------------------------------------------------------------------------
/**
    @class CCFreeCamera

    @brief a simple camera that can be flown around the world

    A CCFreeCamera keeps track only of its own position and orientation.
    User "move" input flies the camera around the world without changing
    its orientation; user "spin" input interpretion depends on SpinMode.

    (C) 2004 Rafael Van Daele-Hunt
*/
#include "BombsquadBruce/cccamera.h"

//------------------------------------------------------------------------------
class nInputServer;

class CCFreeCamera : public CCCamera
{
public:
	enum SpinMode 
	{ // determines how the camera reacts to user "spin channel" input (usually from mouse movement)
		NONE,  // ignores input
		DRAG,  // input causes the camera to move in the opposite direction (so if a left-click activates DRAG mode, the user can "click the world" and drag it around)
		ROTATE // input causes the camera to rotate in the corresponding direction
	};

    CCFreeCamera();
    virtual ~CCFreeCamera();
    virtual bool SaveCmds(nPersistServer* ps);

    // script commands:
    /// Set camera position (in worldspace).
    void SetPosition(float x, float y, float z);
    /// Set camera orientation via a normalized quaternion.
    void SetOrientation(const quaternion&);
    /// Sets camera response to "spin" channels.
    void SetSpinMode(SpinMode);
    /// Sets the speed of "flight" through the world in response to "move" channels
    void SetMoveSpeed( float moveSpeed ) { if( moveSpeed >= 0.0f ) m_MoveSpeed = moveSpeed;} 
    /// Sets the rotation speed in response to "spin" channels in SPIN mode.
    void SetRotateSpeed(float rotateSpeed ) { if( rotateSpeed >= 0.0f ) m_RotateSpeed = rotateSpeed;}

    // nonscript commands:
    /// Update the camera's position and orientation based on user input and mode.
    virtual void Trigger(double deltaTime);

private:
    void Move();
    void Drag();
    void Rotate();
    void MoveBy( const vector3& );
    void DoRotate( float, float, float );

    SpinMode m_Mode;

    // scriptable "constants":
    float m_MoveSpeed;
    float m_RotateSpeed;
};


#endif
