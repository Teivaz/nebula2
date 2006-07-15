//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendecapsulegeom.h"

nNebulaScriptClass(nOpendeCapsuleGeom, "nopendegeom");

//----------------------------------------------------------------------------
/**
*/
nOpendeCapsuleGeom::nOpendeCapsuleGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeCapsuleGeom::Create( const char* space )
{
    nOpendeGeom::Create( space );
    this->geomId = nOpende::CreateCCylinder( this->getSpace(), 1, 1 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeCapsuleGeom::~nOpendeCapsuleGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeCapsuleGeom::SetParams( float radius, float length )
{
    n_assert( this->id() && "nOpendeCapsuleGeom::id not valid!" );
    nOpende::GeomCCylinderSetParams( this->id(), radius, length );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeCapsuleGeom::GetParams( float* radius, float* length )
{
    n_assert( this->id() && "nOpendeCapsuleGeom::id not valid!" );
    nOpende::GeomCCylinderGetParams( this->id(), radius, length );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeCapsuleGeom::PointDepth( const vector3& p )
{
    n_assert( this->id() && "nOpendeCapsuleGeom::id not valid!" );
    return nOpende::GeomCCylinderPointDepth( this->id(), p );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
