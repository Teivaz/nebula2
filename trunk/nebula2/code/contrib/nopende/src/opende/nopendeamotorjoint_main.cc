//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendeamotorjoint.h"

nNebulaScriptClass(nOpendeAMotorJoint, "nopendejoint");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeAMotorJoint::nOpendeAMotorJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeAMotorJoint::Create( const char* world )
{
    nOpendeJoint::Create( world );
    this->id = nOpende::JointCreateAMotor( this->ref_World->id, 0 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeAMotorJoint::~nOpendeAMotorJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeAMotorJoint::SetMode( int mode )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetAMotorMode( this->id, mode );
}

//----------------------------------------------------------------------------
/**
    @brief Set the angular motor mode.
    @param mode Must be either "euler" or "user".
*/
void nOpendeAMotorJoint::SetMode( const char* mode )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    if ( strcmp( mode, "user" ) == 0 )
        nOpende::JointSetAMotorMode( this->id, dAMotorUser );
    else if ( strcmp( mode, "euler" ) == 0 )
        nOpende::JointSetAMotorMode( this->id, dAMotorEuler );
    else
        n_error( "nOpendeAMotorJoint::SetMode(): Uknown mode %s!", mode );
}

//----------------------------------------------------------------------------
/**
*/
int nOpendeAMotorJoint::GetMode()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetAMotorMode( this->id );
}

//----------------------------------------------------------------------------
/**
    @brief Get the current angular motor mode.
    @return Either "euler" or "user".
*/
const char* nOpendeAMotorJoint::GetModeName()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    int mode = nOpende::JointGetAMotorMode( this->id );
    if ( dAMotorUser == mode )
        return "user";
    else if ( dAMotorEuler == mode )
        return "euler";
    else
        n_error( "nOpendeAMotorJoint::GetModeName(): Unknown mode!" );
    return "uknown";
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeAMotorJoint::SetNumAxes( int numAxes )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetAMotorNumAxes( this->id, numAxes );
}

//----------------------------------------------------------------------------
/**
*/
int nOpendeAMotorJoint::GetNumAxes()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetAMotorNumAxes( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeAMotorJoint::SetAxis( int anum, int rel, const vector3& axis )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetAMotorAxis( this->id, anum, rel, axis );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeAMotorJoint::GetAxis( int anum, vector3& result )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetAMotorAxis( this->id, anum, result );
}

//----------------------------------------------------------------------------
/**
*/
int nOpendeAMotorJoint::GetAxisRel( int anum )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetAMotorAxisRel( this->id, anum );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeAMotorJoint::SetAngle( int anum, float angle )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetAMotorAngle( this->id, anum, angle );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeAMotorJoint::GetAngle( int anum )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetAMotorAngle( this->id, anum );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeAMotorJoint::GetAngleRate( int anum )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetAMotorAngleRate( this->id, anum );
}
    
//----------------------------------------------------------------------------
/**
*/
void nOpendeAMotorJoint::SetParam( int param, float value )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetAMotorParam( this->id, param, value );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeAMotorJoint::GetParam( int param )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetAMotorParam( this->id, param );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
