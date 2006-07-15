//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopenderaygeom.h"

nNebulaScriptClass(nOpendeRayGeom, "nopendegeom");

//----------------------------------------------------------------------------
/**
*/
nOpendeRayGeom::nOpendeRayGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeRayGeom::Create( const char* space )
{
    nOpendeGeom::Create( space );
    this->geomId = nOpende::CreateRay( this->getSpace(), 1 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeRayGeom::~nOpendeRayGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeRayGeom::SetLength( float length )
{
    n_assert( this->id() && "nOpendeRayGeom::id not valid!" );
    nOpende::GeomRaySetLength( this->id(), length );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeRayGeom::GetLength()
{
    n_assert( this->id() && "nOpendeRayGeom::id not valid!" );
    return nOpende::GeomRayGetLength( this->id() );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeRayGeom::Set( const vector3& pos, const vector3& dir )
{
    n_assert( this->id() && "nOpendeRayGeom::id not valid!" );
    nOpende::GeomRaySet( this->id(), pos, dir );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeRayGeom::Get( vector3& start, vector3& dir )
{
    n_assert( this->id() && "nOpendeRayGeom::id not valid!" );
    nOpende::GeomRayGet( this->id(), start, dir );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeRayGeom::SetParams( bool firstContact, bool backfaceCull )
{
    n_assert( this->id() && "nOpendeRayGeom::id not valid!" );
    nOpende::GeomRaySetParams( this->id(), 
                               firstContact ? 1:0, 
                               backfaceCull ? 1:0 );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeRayGeom::GetParams( bool* firstContact, bool* backfaceCull )
{
    n_assert( this->id() && "nOpendeRayGeom::id not valid!" );
    int a, b;
    nOpende::GeomRayGetParams( this->id(), &a, &b );
    *firstContact = (a == 1);
    *backfaceCull = (b == 1);
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeRayGeom::SetClosestHit( bool closestHit )
{
    n_assert( this->id() && "nOpendeRayGeom::id not valid!" );
    nOpende::GeomRaySetClosestHit( this->id(), closestHit );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeRayGeom::GetClosestHit()
{
    n_assert( this->id() && "nOpendeRayGeom::id not valid!" );
    return nOpende::GeomRayGetClosestHit( this->id() );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
