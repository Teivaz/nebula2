//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendefixedjoint.h"

nNebulaScriptClass(nOpendeFixedJoint, "nopendejoint");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeFixedJoint::nOpendeFixedJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeFixedJoint::Create( const char* world )
{
    nOpendeJoint::Create( world );
    this->id = nOpende::JointCreateFixed( this->ref_World->id, 0 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeFixedJoint::~nOpendeFixedJoint()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeFixedJoint::Fix()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetFixed( this->id );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
