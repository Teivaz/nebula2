//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendeuniversaljoint.h"

nNebulaScriptClass(nOpendeUniversalJoint, "nopendejoint");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeUniversalJoint::nOpendeUniversalJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeUniversalJoint::Create( const char* world )
{
    nOpendeJoint::Create( world );
    this->id = nOpende::JointCreateUniversal( this->ref_World->id, 0 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeUniversalJoint::~nOpendeUniversalJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeUniversalJoint::SetAnchor( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );  
    nOpende::JointSetUniversalAnchor( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeUniversalJoint::SetAxis1( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetUniversalAxis1( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeUniversalJoint::SetAxis2( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetUniversalAxis2( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeUniversalJoint::GetAnchor( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetUniversalAnchor( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeUniversalJoint::GetAnchor2( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetUniversalAnchor2( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeUniversalJoint::GetAxis1( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetUniversalAxis1( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeUniversalJoint::GetAxis2( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetUniversalAxis2( this->id, a );
}

//------------------------------------------------------------------------------
/**
*/
void nOpendeUniversalJoint::SetParam( int param, float value )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetUniversalParam( this->id, param, value );
}

//------------------------------------------------------------------------------
/**
*/
float nOpendeUniversalJoint::GetParam( int param )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetUniversalParam( this->id, param );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
