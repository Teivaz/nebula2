//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccmobileactor.h"
#include "input/ninputserver.h"
#include "BombsquadBruce/general.h"

nNebulaScriptClass(CCMobileActor, "ccactor");


//------------------------------------------------------------------------------
/**
*/
CCMobileActor::CCMobileActor() :
    m_ApparentRotation( 0 ),
    m_DesiredApparentRotation( 0 ),
    m_LastDesiredAppRot(-1.0f),
    m_ApparentTurnSpd( 0 ),
    m_CurAppTurnSpd( 0 ),
    m_CurFwdSpd( 0 ),
    m_CurTurnSpd( 0 ),
    m_CurTiltSpd( 0 ),
    m_CurStrafeSpd( 0 ),
    m_FwdSpd(60.0f),
    m_TurnSpd(2.0f)
{
}

CCMobileActor::~CCMobileActor()
{
}
////////////////////////////////////////////////////////////7
// Script commands
////////////////////////////////////////////////////////////7

void CCMobileActor::SetShapeNode(const char* shapenode_path)
{
    CCActor::SetShapeNode( shapenode_path );
    m_SkinData.Init( m_RenderContext );
}

void CCMobileActor::SetAnimState( int state, float speedFactor )
{
    m_SkinData.SetState( state );
    SetAnimSpeed( speedFactor );
}

void CCMobileActor::SetAnimSpeed( float speedFactor )
{
    m_SkinData.SetSpeedFactor( speedFactor );
}

void CCMobileActor::SetControl( const std::string & control, const std::string & channel )
{
    if( control == "SPIN_LEFT" )
        m_SpinLeftChannel = channel;
    else if( control == "SPIN_RIGHT" )
        m_SpinRightChannel = channel;   
    else if( control == "SPIN_UP" )
        m_SpinUpChannel = channel;
    else if( control == "SPIN_DOWN" )
        m_SpinDownChannel = channel; 
    else if( control == "MOVE_FWD" )
        m_MoveFwdChannel = channel;
    else if( control == "MOVE_BACK" )
        m_MoveBackChannel = channel;
    else if( control == "MOVE_LEFT" )
        m_MoveLeftChannel = channel;
    else if( control == "MOVE_RIGHT" )
        m_MoveRightChannel = channel;
}

/**
    @brief Moves the character
    @param v          velocity in cm/s
    @param rotation   amount to rotate (relative to current heading) in radians per second, positive is counterclockwise
    @param dt         the elapsed time since the previous move, in seconds

    Note that the rotation assumes that the y axis corresponds to the up vector.
    Constraints to movement (e.g. collisions, ground height, etc.) are 
    handled either by the world or in script.
*/
void CCMobileActor::Move( const vector3& v, float rotation, float dt)
{
    m_PreviousPosition = GetPosition(); // store this for reset after collision
    matrix44 newTransMatrix = GetTransformMatrix();
    newTransMatrix.set_translation( vector3() );
    newTransMatrix.rotate_y( rotation * dt );
    newTransMatrix.set_translation( m_PreviousPosition + v * dt );
    SetTransformMatrix( newTransMatrix );
}

////////////////////////////////////////////////////////////
float CCMobileActor::GetCurSpd() const
{
    return m_CurFwdSpd;
}

////////////////////////////////////////////////////////////
float CCMobileActor::GetCurTurnSpd() const
{
    return m_CurTurnSpd;
}

////////////////////////////////////////////////////////////
float CCMobileActor::GetCurTiltSpd() const
{
    return m_CurTiltSpd;
}

////////////////////////////////////////////////////////////
float CCMobileActor::GetCurStrafeSpd() const
{
    return m_CurStrafeSpd;
}

////////////////////////////////////////////////////////////
// Nonscript public methods
////////////////////////////////////////////////////////////

void CCMobileActor::Trigger( float dt )
{
    m_SkinData.StepTime( dt );
    UpdateCurrentSpeed();
    UpdateApparentRotation(dt);
}

void CCMobileActor::Attach(nSceneServer& sceneServer, uint& frameId)
{
    matrix44 oldTransformMatrix = GetTransformMatrix();
    Move( vector3(), m_ApparentRotation, 1.0f );
    m_ApparentHeadingVec = GetCurHeadingVector(); // a bit hackish to do this here rather than in Trigger(), but I don't think it matters
    CCActor::Attach( sceneServer, frameId );
    SetTransformMatrix( oldTransformMatrix );
}

vector3 CCMobileActor::GetCurHeadingVector() const // could consider storing a curHeadingVector and doing lazy initialization of it
{	
	return GetTransformMatrix().z_component();
}

vector3 CCMobileActor::GetApparentHeadingVector() const
{	
	return m_ApparentHeadingVec;
}

////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////

void CCMobileActor::UpdateApparentRotation(float dt)
{
    if( m_CurFwdSpd == 0 )
    {
        if( m_CurStrafeSpd > 0 )
            m_DesiredApparentRotation = PI/2.0;
        else if( m_CurStrafeSpd < 0 )
            m_DesiredApparentRotation = -PI/2.0;
        // else leave it unchanged
    }
    else
    {
        m_DesiredApparentRotation = n_atan( m_CurStrafeSpd / m_CurFwdSpd );
        if( m_CurFwdSpd < 0 ) 
            m_DesiredApparentRotation += PI;
    }
    float delta = CCUtil::Normalize( m_DesiredApparentRotation - m_ApparentRotation ); // 0 to 2*PI    
    if( delta > PI )
        delta = delta - 2 * PI; // -PI to PI 
    if( m_DesiredApparentRotation != m_LastDesiredAppRot )
    { // started a new rotation; determine speed
        m_CurAppTurnSpd = m_ApparentTurnSpd * delta;
        m_LastDesiredAppRot = m_DesiredApparentRotation;
    }
    const float canTurnThisTick = m_CurAppTurnSpd * dt;
    if( n_abs( delta ) <= n_abs(canTurnThisTick) )
        m_ApparentRotation = m_DesiredApparentRotation;
    else
        m_ApparentRotation += canTurnThisTick;
}

void CCMobileActor::UpdateCurrentSpeed()
{
    m_CurFwdSpd = m_FwdSpd * (nInputServer::Instance()->GetSlider(m_MoveFwdChannel.c_str()) - nInputServer::Instance()->GetSlider(m_MoveBackChannel.c_str()));
    m_CurTurnSpd = m_TurnSpd * (nInputServer::Instance()->GetSlider(m_SpinLeftChannel.c_str()) - nInputServer::Instance()->GetSlider(m_SpinRightChannel.c_str())); 
    m_CurTiltSpd = m_TiltSpd * (nInputServer::Instance()->GetSlider(m_SpinUpChannel.c_str()) - nInputServer::Instance()->GetSlider(m_SpinDownChannel.c_str())); 
    m_CurStrafeSpd = m_FwdSpd * (nInputServer::Instance()->GetSlider(m_MoveLeftChannel.c_str()) - nInputServer::Instance()->GetSlider(m_MoveRightChannel.c_str()));
}