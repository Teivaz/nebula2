//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendeplanegeom.h"

nNebulaScriptClass(nOpendePlaneGeom, "nopendegeom");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendePlaneGeom::nOpendePlaneGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendePlaneGeom::Create( const char* space )
{
    nOpendeGeom::Create( space );
    this->geomId = nOpende::CreatePlane( this->getSpace(), 0, 1, 0, 0 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendePlaneGeom::~nOpendePlaneGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendePlaneGeom::SetParams( float a, float b, float c, float d )
{
    n_assert( this->id() && "nOpendePlaneGeom::id not valid!" );
    nOpende::GeomPlaneSetParams( this->id(), a, b, c, d );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendePlaneGeom::GetParams( vector4& result )
{
    n_assert( this->id() && "nOpendePlaneGeom::id not valid!" );
    nOpende::GeomPlaneGetParams( this->id(), result );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendePlaneGeom::PointDepth( const vector3& p )
{
    n_assert( this->id() && "nOpendePlaneGeom::id not valid!" );
    return nOpende::GeomPlanePointDepth( this->id(), p );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
