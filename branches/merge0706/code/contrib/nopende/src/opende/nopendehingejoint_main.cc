//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendehingejoint.h"

nNebulaScriptClass(nOpendeHingeJoint, "nopendejoint");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeHingeJoint::nOpendeHingeJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHingeJoint::Create( const char* world )
{
    nOpendeJoint::Create( world );
    this->id = nOpende::JointCreateHinge( this->ref_World->id, 0 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeHingeJoint::~nOpendeHingeJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHingeJoint::SetAnchor( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetHingeAnchor( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHingeJoint::SetAxis( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetHingeAxis( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHingeJoint::GetAnchor( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetHingeAnchor( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHingeJoint::GetAnchor2( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetHingeAnchor2( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeHingeJoint::GetAxis( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetHingeAxis( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeHingeJoint::GetAngle()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetHingeAngle( this->id );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeHingeJoint::GetAngleRate()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetHingeAngleRate( this->id );
}

//----------------------------------------------------------------------------
/**
*/  
void nOpendeHingeJoint::SetParam( int param, float value )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetHingeParam( this->id, param, value );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeHingeJoint::GetParam( int param )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetHingeParam( this->id, param );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
