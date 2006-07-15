//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendespheregeom.h"

nNebulaScriptClass(nOpendeSphereGeom, "nopendegeom");

//----------------------------------------------------------------------------
/**
*/
nOpendeSphereGeom::nOpendeSphereGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSphereGeom::Create( const char* space )
{
    nOpendeGeom::Create( space );
    this->geomId = nOpende::CreateSphere( this->getSpace(), 1.0f );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeSphereGeom::~nOpendeSphereGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSphereGeom::SetRadius( float r )
{
    n_assert( this->id() && "nOpendeSphereGeom::id not valid!" );
    nOpende::GeomSphereSetRadius( this->id(), r );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeSphereGeom::GetRadius()
{
    n_assert( this->id() && "nOpendeSphereGeom::id not valid!" );
    return nOpende::GeomSphereGetRadius( this->id() );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeSphereGeom::PointDepth( const vector3& p )
{
    n_assert( this->id() && "nOpendeSphereGeom::id not valid!" );
    return nOpende::GeomSpherePointDepth( this->id(), p );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
