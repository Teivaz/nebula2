//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendeballjoint.h"

nNebulaScriptClass(nOpendeBallJoint, "nopendejoint");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeBallJoint::nOpendeBallJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBallJoint::Create( const char* world )
{
    nOpendeJoint::Create( world );
    this->id = nOpende::JointCreateBall( this->ref_World->id, 0 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeBallJoint::~nOpendeBallJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBallJoint::SetAnchor( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetBallAnchor( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBallJoint::GetAnchor( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetBallAnchor( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBallJoint::GetAnchor2( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetBallAnchor2( this->id, a );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
