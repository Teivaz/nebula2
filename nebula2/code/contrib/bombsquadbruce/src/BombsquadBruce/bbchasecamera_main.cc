//------------------------------------------------------------------------------
//  (C) 2003 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbchasecamera.h"
#include "BombsquadBruce/bbroot.h"
#include "BombsquadBruce/general.h"

nNebulaScriptClass(BBChaseCamera, "bbcamera");

//------------------------------------------------------------------------------
BBChaseCamera::BBChaseCamera() :
	m_Mode(NONE),
    m_CameraOffset(0.0, 0.0, 0.0),
    m_FocalOffset(0.0, 0.0, 0.0),
	m_MinZoomDist(0.01f),
	m_MaxZoomDist(1000.0f),
    m_MoveSpeed( 1.0f ),
    m_RotateSpeed( 0.5f ),
    m_OrbitSpeed( 0.05f ),
	m_ZoomSpeed( 2.0f ),
    m_AdjustPosCmdProto( 0 )
{
}

//------------------------------------------------------------------------------
BBChaseCamera::~BBChaseCamera()
{
}

/**
    @brief Determine how the Spin input channel is interpreted.
    The Mode determines whether the Spin channel (usually corresponding to mouse movement)
    causes the camera to spin, orbit the focal point, etc.
    For example, a script could cause the mode to be set to ORBIT as long as the left mouse
    button is held down.
*/
void BBChaseCamera::SetSpinMode(SpinMode mode)
{
    m_Mode = mode;
}

void BBChaseCamera::SetMaxZoomDist(float maxZoomDist )
{
    if( maxZoomDist >= 0.0f )
    {
        m_MaxZoomDist = maxZoomDist;
        n_assert( m_MinZoomDist <= m_MaxZoomDist );
    }
    else if( maxZoomDist == -1 )
    {
        m_MaxZoomDist = -1;
    }
    else
    {
        n_assert( "Invalid zoom distance (must be -1 or >= 0).\n" );
    }
}

/**
    @brief Update the camera's position and orientation based on style, user input and mode.
*/
void BBChaseCamera::Trigger(double deltaTime)
{
    n_assert( m_TargetEntity.isvalid() );
    BBCamera::Trigger(deltaTime);
    ReactToInput();
    Chase(deltaTime);
}

//-------------------------------------------------------------------
// private methods
//-------------------------------------------------------------------

/**
    @brief Adjusts camera and focal point offsets in response to user input.
*/
void BBChaseCamera::ReactToInput()
{
	Move();
    Zoom();
	switch( m_Mode )
	{
	    case ROTATE:
		    Rotate();
		    break;
	    case ORBIT:
		    Orbit();
		    break;
	}
}

/**
    @brief Sets camera transform based on target entity's transform, camera offset, and focal point offset.
*/

void BBChaseCamera::Chase(double deltaTime)
{
    vector3 newPos;
    m_TargetEntity->GetTransformMatrix().mult( m_CameraOffset, newPos );
    AdjustPositionInScript(newPos, deltaTime);
    m_TransformMatrix.set_translation( newPos );
    m_TargetEntity->GetTransformMatrix().mult( m_FocalOffset, newPos );
    m_TransformMatrix.lookatRh( newPos, BBUtil::UP_VECTOR );    
}

/**
    @brief Fly the camera around, always facing the same point.

    Shifts the camera offset according to MOVE_ input; doesn't change the focal point.
*/

void BBChaseCamera::Move()
{ 
    if( 0.0f != GetLeftRightMove() || 0.0f != GetFwdBackMove() )
    {
        const vector3 delta(	-GetLeftRightMove() * m_MoveSpeed,
		    			    	0.0f,
			    		    	-GetFwdBackMove() * m_MoveSpeed
				    	    );
        m_CameraOffset += delta;
        DoZoom(0.0f); // just to ensure that we aren't too close or far from the focal point
    }
}

/**
    @brief Spin the camera in place.

    Shifts the focal point according to SPIN_ input, doesn't change the camera offset.
*/

void BBChaseCamera::Rotate()
{
    SpinAround( m_FocalOffset, m_CameraOffset, m_RotateSpeed );
}

/**
    @brief Move the camera in an orbit around its focal point.

    Moves the camera offset according to SPIN_ input, maintaining a constant distance from
    the unmoving focal point.
*/

void BBChaseCamera::Orbit()
{	    
    SpinAround( m_CameraOffset, m_FocalOffset, m_OrbitSpeed );
}

/**
    @brief Move the camera towards or away from its focal point according to user ZMOVE_ input.

    If the camera offset is too close to or far from the focal point, adjusts its position.
*/

void BBChaseCamera::Zoom()
{	    
    if( 0.0f != GetZMove() )
    {
        DoZoom( GetZMove() * m_ZoomSpeed );
    }
}

/**
    @brief Move the camera towards or away from its focal point according to parameter.
    
    If the camera offset is too close to or far from the focal point, adjusts its position.
*/

void BBChaseCamera::DoZoom( float zoomDelta )
{
    vector3 sightLine = m_CameraOffset;
    sightLine -= m_FocalOffset;
    float zoomDist = sightLine.len() + zoomDelta;
    bool adjustCameraOffset = ( 0.0 != zoomDelta );

    if( zoomDist < m_MinZoomDist )
    {
        adjustCameraOffset = true;
        zoomDist = m_MinZoomDist;
    }
    else if( m_MaxZoomDist != -1 && zoomDist > m_MaxZoomDist )
    {
        adjustCameraOffset = true;
        zoomDist = m_MaxZoomDist;
    }

    if( adjustCameraOffset )
    {
        sightLine.norm();
        sightLine *= zoomDist;
        m_CameraOffset = m_FocalOffset + sightLine;
    }
}

/**
    @brief Moves thisPt in orbit around thatPt according to user SPIN_ input.
*/

void BBChaseCamera::SpinAround( vector3& orbitThisPt, const vector3& aroundThatPt, float speed )
{
    if ( 0.0f != GetHorizontalSpin() || 0.0f != GetVerticalSpin() )
    {
        orbitThisPt -= aroundThatPt; // get the vector from thatPt to thisPt
        matrix44 m;
        m.rotate_y( GetHorizontalSpin() * speed );
        m.rotate_x( GetVerticalSpin() * speed );
        vector3 temp;
        m.mult( orbitThisPt, temp );
        orbitThisPt = temp + aroundThatPt;
    }
}

/**
    @brief Call a script-side function that adjusts the camera position
    The nature of the adjustment is game-specific 
    (e.g. avoiding occlusion by objects, staying above ground...).
*/
void BBChaseCamera::AdjustPositionInScript(vector3& posToAdjust, double deltaTime)
{   
    if( !m_AdjustPosCmdProto )
    {
        m_AdjustPosCmdProto = GetClass()->FindScriptCmdByName( "AdjustPosition" );
        n_assert( m_AdjustPosCmdProto );
    }
    nCmd* pCmd = m_AdjustPosCmdProto->NewCmd();
    n_assert( pCmd );
    n_assert( 4 == pCmd->GetNumInArgs() );
    n_assert( 3 == pCmd->GetNumOutArgs() );
    pCmd->In()->SetF( posToAdjust.x );
    pCmd->In()->SetF( posToAdjust.y );
    pCmd->In()->SetF( posToAdjust.z );
    pCmd->In()->SetF( static_cast<float>(deltaTime) );
    n_verify( Dispatch( pCmd ) );
    posToAdjust.x = pCmd->Out()->GetF();
    posToAdjust.y = pCmd->Out()->GetF();
    posToAdjust.z = pCmd->Out()->GetF();
    m_AdjustPosCmdProto->RelCmd( pCmd );
}