//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendetransformgeom.h"

nNebulaScriptClass(nOpendeTransformGeom, "nopendegeom");

//----------------------------------------------------------------------------
/**
*/
nOpendeTransformGeom::nOpendeTransformGeom() :
    geomName("none")
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTransformGeom::Create( const char* space )
{
    nOpendeGeom::Create( space );
    this->geomId = nOpende::CreateGeomTransform( this->getSpace() );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeTransformGeom::~nOpendeTransformGeom()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTransformGeom::SetGeom( dGeomID g )
{
    n_assert( this->id() && "nOpendeTransformGeom::id not valid!" );
    nOpende::GeomTransformSetGeom( this->id(), g );
}

//----------------------------------------------------------------------------
/**
*/
dGeomID nOpendeTransformGeom::GetGeom()
{
    n_assert( this->id() && "nOpendeTransformGeom::id not valid!" );
    return nOpende::GeomTransformGetGeom( this->id() );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTransformGeom::SetGeom( const char* gName )
{
    n_assert( this->id() && "nOpendeTransformGeom::id not valid!" );
    nRoot* temp = nKernelServer::Instance()->Lookup( gName );
    n_assert( temp &&
              temp->IsA( nKernelServer::Instance()->FindClass( "nopendegeom" ) ) );
    this->geomName = gName;
    nOpende::GeomTransformSetGeom( this->id(), ((nOpendeGeom*)temp)->id() );
}

//----------------------------------------------------------------------------
/**
*/
const char* nOpendeTransformGeom::GetGeomName()
{
    n_assert( this->id() && "nOpendeTransformGeom::id not valid!" );
    return this->geomName.Get();
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTransformGeom::SetCleanup( bool mode )
{
    n_assert( this->id() && "nOpendeTransformGeom::id not valid!" );
    nOpende::GeomTransformSetCleanup( this->id(), mode ? 1:0 );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeTransformGeom::GetCleanup()
{
    n_assert( this->id() && "nOpendeTransformGeom::id not valid!" );
    return nOpende::GeomTransformGetCleanup( this->id() ) == 1;
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTransformGeom::SetInfo( int mode )
{
    n_assert( this->id() && "nOpendeTransformGeom::id not valid!" );
    nOpende::GeomTransformSetInfo( this->id(), mode );
}

//----------------------------------------------------------------------------
/**
*/
int nOpendeTransformGeom::GetInfo()
{
    n_assert( this->id() && "nOpendeTransformGeom::id not valid!" );
    return nOpende::GeomTransformGetInfo( this->id() );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
