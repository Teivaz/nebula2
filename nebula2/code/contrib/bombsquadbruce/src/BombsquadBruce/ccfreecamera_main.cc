//------------------------------------------------------------------------------
//  (C) 2003 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccfreecamera.h"

nNebulaScriptClass(CCFreeCamera, "cccamera");

//------------------------------------------------------------------------------
CCFreeCamera::CCFreeCamera() :
    m_Mode(NONE),
    m_MoveSpeed(1.0f),
    m_RotateSpeed( 0.5f )
{
}

//------------------------------------------------------------------------------
CCFreeCamera::~CCFreeCamera()
{
}

/**
    @brief Set camera position (in worldspace).
*/
void CCFreeCamera::SetPosition(float x, float y, float z)
{
    m_TransformMatrix.M41 = x;
    m_TransformMatrix.M42 = y;
    m_TransformMatrix.M43 = z;
}

/**
    @brief Sets camera orientation via a normalized quaternion.
*/

void CCFreeCamera::SetOrientation(const quaternion& quat)
{
    const vector3& pos = m_TransformMatrix.pos_component();
    m_TransformMatrix.ident();
    m_TransformMatrix.set( quat );
    m_TransformMatrix.set_translation( pos );
}

/**
    @brief Determine how the Spin input channel is interpreted.
    The Mode determines whether the Spin channel (usually corresponding to mouse movement)
    causes the camera to rotate, orbit the focal point, etc.
    For example, a script could cause the mode to be set to ORBIT as long as the left mouse
    button is held down.
*/
void CCFreeCamera::SetSpinMode(SpinMode mode)
{
    m_Mode = mode;
}

/**
    @brief Update the camera's position and orientation based on style, user input and mode.
*/
void CCFreeCamera::Trigger(double deltaTime)
{
    CCCamera::Trigger(deltaTime);

    Move();
    switch( m_Mode )
    {
        case ROTATE:
            Rotate();
            break;
        case DRAG:
            Drag();
        break;
    }
}
//-------------------------------------------------------------------
// private methods
//-------------------------------------------------------------------

void CCFreeCamera::Move()
{ 
    if( 0.0f != GetLeftRightMove() || 0.0f != GetFwdBackMove() )
    {
        vector3 absDelta(	-GetLeftRightMove() * m_MoveSpeed,
                            0.0f,
                            -GetFwdBackMove() * m_MoveSpeed
                        );
        MoveBy( absDelta );
    }
}

void CCFreeCamera::Drag()
{
    if ( 0.0f != GetHorizontalSpin() || 0.0f != GetVerticalSpin() )
    {
        vector3 absDelta(	GetHorizontalSpin() * m_MoveSpeed,
                            -GetVerticalSpin() * m_MoveSpeed,
                            0.0f 
                        );
        MoveBy( absDelta );
    }
}

void CCFreeCamera::Rotate()
{
    if( 0.0f != GetHorizontalSpin() ) 
    {
        matrix44 m;
        m.rotate_y(GetHorizontalSpin() * m_RotateSpeed );
        vector3 temp(m_TransformMatrix.pos_component());
        m_TransformMatrix.set_translation( vector3( 0.0f, 0.0f, 0.0f ) );
        m_TransformMatrix.mult_simple( m );
        m_TransformMatrix.set_translation( temp );
    }
    if( 0.0f != GetVerticalSpin() )
    {
        matrix44 m;
        m.rotate_x(GetVerticalSpin() * m_RotateSpeed );
        m.mult_simple(m_TransformMatrix);
        m_TransformMatrix = m;
    }
}

void CCFreeCamera::MoveBy( const vector3& absDelta )
{
    vector3 relDelta; // in camera space, so that e.g. forward moves us forward rather than along the z-axis.
    m_TransformMatrix.mult( absDelta, relDelta );
    m_TransformMatrix.set_translation( relDelta );
}