#ifndef CC_CHASECAMERA_H
#define CC_CHASECAMERA_H
//------------------------------------------------------------------------------
/**
    @class CCChaseCamera
    @ingroup BombsquadBruceContribModule
    @brief Follows a specified target CCRoot around.

    CCChaseCamera maintains its position at a (script-settable) offset
    relative to a (script-settable) CCRoot target, and looks at a focal
    point which is also specified as a (script-settable) offset from the
    target.  Thus, if we set the position to directly behind the target, 
    and the focal point to beside the target's upper left corner, then 
    we move forward when the target does, and, if it rotates, we swing 
    around so as to continue to look beside the UL corner, from behind.

    The Movement channels alters the camera's offset, flying the camera around.
    When moved by user input, the camera rotates automatically to continue to 
    look at its original focal point.  When the user stops moving, the camera 
    maintains its new offset relative to the target.  

    The Spin channels have various effects depending on mode:
    In Orbit mode, the camera's offset it altered such that the camera orbits
    its focal point, always facing and maintaining a constant distance from it.
    In Spin mode, the camera rotates in place.  The focal point is shifted to the
    point the camera is now looking at, preserving the original camera-focal point 
    distance.

    The ZMove channel causes the camera's offset moves towards or away from the focal point
    along the line that connects the two.

    (C)	2004 Rafael Van Daele-Hunt
*/

#include "BombsquadBruce/cccamera.h"
#include "kernel/ndynautoref.h"

//------------------------------------------------------------------------------
class CCRoot;

class CCChaseCamera : public CCCamera
{
public:
	enum SpinMode 
	{ // determines how the camera reacts to user "spin channel" input (usually from mouse movement)
		NONE,    // ignores input
		ORBIT,   // orbit the focal point
		ROTATE  // rotate in the corresponding direction
	};
public:
    CCChaseCamera();
    virtual ~CCChaseCamera();
    virtual bool SaveCmds(nPersistServer* ps);

    // script commands:
    /// Tells the camera which entity to track.
    void SetTargetEntityPath(const char* path) { m_TargetEntity = path; }
    /// Set camera's offset with respect to the the target entity.
    void SetCameraOffset(float x, float y, float z) { m_CameraOffset.set(x, y, z); }
    /// Set the offset of the focal point with respect to the the target entity.
    void SetFocalOffset(float x, float y, float z) { m_FocalOffset.set(x, y, z); }
    /// Sets camera response to the "spin channels".
    void SetSpinMode(SpinMode);

    /// the minimum distance permitted between the camera's and focal point's offsets
    void SetMinZoomDist(float minZoomDist ) { n_assert( minZoomDist >= 0.0f ); m_MinZoomDist = minZoomDist; n_assert( m_MinZoomDist <= m_MaxZoomDist ); }
    /// the maximum distance permitted between the camera's and focal point's offsets.  -1 means unlimited.
    void SetMaxZoomDist(float maxZoomDist );
    void SetMoveSpeed( float moveSpeed ) { if( moveSpeed >= 0.0f ) m_MoveSpeed = moveSpeed;} 
    void SetRotateSpeed( float rotateSpeed ) { if( rotateSpeed >= 0.0f ) m_RotateSpeed = rotateSpeed;}
    void SetOrbitSpeed( float orbitSpeed ) { if( orbitSpeed >= 0.0f ) m_OrbitSpeed = orbitSpeed;}
    void SetZoomSpeed(float zoomSpeed ) { if( zoomSpeed >= 0.0f ) m_ZoomSpeed = zoomSpeed;}

    // nonscript commands:
    /// Update the camera's position and orientation based on user input, mode, and target CCRoot's transformation.
    void Trigger(double deltaTime);

private:
    void ReactToInput();
    void Chase(double deltaTime);

    void Move();
    void Orbit();
    void Rotate();
    void Zoom();

    void DoZoom(float deltaZoom);
    void SpinAround( vector3& orbitThisPt, const vector3& aroundThatPt, float speed );

    void AdjustPositionInScript(vector3& posToAdjust, double deltaTime);
    int CheckForObstructingObjects(const vector3& pos);

    nDynAutoRef<CCRoot> m_TargetEntity;
    SpinMode m_Mode;
    vector3 m_CameraOffset; // relative to the canonical target position
    vector3 m_FocalOffset;  // relative to the canonical target position

    // scriptable "constants":
    float m_MinZoomDist;
    float m_MaxZoomDist;
    float m_MoveSpeed;
    float m_RotateSpeed;
    float m_OrbitSpeed;
    float m_ZoomSpeed;

    // script-side commands
    nCmdProto * m_AdjustPosCmdProto;
};


#endif
