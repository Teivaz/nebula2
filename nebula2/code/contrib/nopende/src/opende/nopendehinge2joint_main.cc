//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendehinge2joint.h"

nNebulaScriptClass(nOpendeHinge2Joint, "nopendejoint");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeHinge2Joint::nOpendeHinge2Joint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHinge2Joint::Create( const char* world )
{
    nOpendeJoint::Create( world );
    this->id = nOpende::JointCreateHinge2( this->ref_World->id, 0 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeHinge2Joint::~nOpendeHinge2Joint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHinge2Joint::SetAnchor( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetHinge2Anchor( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHinge2Joint::SetAxis1( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetHinge2Axis1( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHinge2Joint::SetAxis2( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetHinge2Axis2( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHinge2Joint::GetAnchor( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetHinge2Anchor( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHinge2Joint::GetAnchor2( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetHinge2Anchor2( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHinge2Joint::GetAxis1( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetHinge2Axis1( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHinge2Joint::GetAxis2( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetHinge2Axis2( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeHinge2Joint::GetAngle1()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetHinge2Angle1( this->id );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeHinge2Joint::GetAngle1Rate()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetHinge2Angle1Rate( this->id );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeHinge2Joint::GetAngle2Rate()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetHinge2Angle2Rate( this->id );
}

//------------------------------------------------------------------------------
void nOpendeHinge2Joint::SetParam( int param, float value )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetHinge2Param( this->id, param, value );
}

//------------------------------------------------------------------------------
float nOpendeHinge2Joint::GetParam( int param )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetHinge2Param( this->id, param );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
