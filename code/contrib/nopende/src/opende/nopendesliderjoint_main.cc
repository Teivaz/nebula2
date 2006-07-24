//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendesliderjoint.h"

nNebulaScriptClass(nOpendeSliderJoint, "nopendejoint");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeSliderJoint::nOpendeSliderJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSliderJoint::Create( const char* world )
{
    nOpendeJoint::Create( world );
    this->id = nOpende::JointCreateSlider( this->ref_World->id, 0 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeSliderJoint::~nOpendeSliderJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSliderJoint::SetAxis( const vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetSliderAxis( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSliderJoint::GetAxis( vector3& a )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetSliderAxis( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeSliderJoint::GetPosition()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetSliderPosition( this->id );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeSliderJoint::GetPositionRate()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetSliderPositionRate( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSliderJoint::SetParam( int param, float value )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetSliderParam( this->id, param, value );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeSliderJoint::GetParam( int param )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetSliderParam( this->id, param );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
