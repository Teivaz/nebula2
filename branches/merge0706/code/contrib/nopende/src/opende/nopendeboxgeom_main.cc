//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendeboxgeom.h"

nNebulaScriptClass(nOpendeBoxGeom, "nopendegeom");

//----------------------------------------------------------------------------
/**
*/
nOpendeBoxGeom::nOpendeBoxGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBoxGeom::Create( const char* space )
{
    nOpendeGeom::Create( space );
    this->geomId = nOpende::CreateBox( this->getSpace(), 1.0f, 1.0f, 1.0f );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeBoxGeom::~nOpendeBoxGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBoxGeom::SetLengths( float lx, float ly, float lz )
{
    n_assert( this->id() && "nOpendeBoxGeom::id not valid!" );
    nOpende::GeomBoxSetLengths( this->id(), lx, ly, lz );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBoxGeom::GetLengths( vector3& result )
{
    n_assert( this->id() && "nOpendeBoxGeom::id not valid!" );
    nOpende::GeomBoxGetLengths( this->id(), result );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeBoxGeom::PointDepth( const vector3& p )
{
    n_assert( this->id() && "nOpendeBoxGeom::id not valid!" );
    return nOpende::GeomBoxPointDepth( this->id(), p );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
