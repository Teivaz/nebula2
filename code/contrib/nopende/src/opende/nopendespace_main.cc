//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendespace.h"

nNebulaScriptClass(nOpendeSpace, "nopendegeom");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeSpace::nOpendeSpace()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nOpendeSpace::~nOpendeSpace()
{
    if ( this->id() )
    {
        nOpende::SpaceDestroy( this->id() );
        this->geomId = 0;
    }
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSpace::SetCleanup( bool mode )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nOpende::SpaceSetCleanup( this->id(), mode ? 1 : 0 );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeSpace::GetCleanup()
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    return nOpende::SpaceGetCleanup( this->id() ) == 1;
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSpace::AddGeom( dGeomID g )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nOpende::SpaceAdd( this->id(), g );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSpace::RemoveGeom( dGeomID g )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nOpende::SpaceRemove( this->id(), g );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeSpace::ContainsGeom( dGeomID g )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    return nOpende::SpaceQuery( this->id(), g ) == 1;
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSpace::AddGeom( const char* path )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nRoot* temp = this->kernelServer->Lookup( path );
    n_assert( temp && temp->IsA( this->kernelServer->FindClass( "nopendegeom" ) ) );
    nOpende::SpaceAdd( this->id(), static_cast<nOpendeGeom*>(temp)->id() );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSpace::RemoveGeom( const char* path )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nRoot* temp = this->kernelServer->Lookup( path );
    n_assert( temp && temp->IsA( this->kernelServer->FindClass( "nopendegeom" ) ) );
    nOpende::SpaceRemove( this->id(), static_cast<nOpendeGeom*>(temp)->id() );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeSpace::ContainsGeom( const char* path )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nRoot* temp = this->kernelServer->Lookup( path );
    n_assert( temp && temp->IsA( this->kernelServer->FindClass( "nopendegeom" ) ) );
    return nOpende::SpaceQuery( this->id(),
                                static_cast<nOpendeGeom*>(temp)->id() ) == 1;
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSpace::AddSpace( dSpaceID s )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nOpende::SpaceAdd( this->id(), (dGeomID)s );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeSpace::RemoveSpace( dSpaceID s )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nOpende::SpaceRemove( this->id(), (dGeomID)s );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeSpace::ContainsSpace( dSpaceID s )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    return nOpende::SpaceQuery( this->id(), (dGeomID)s );
}

//----------------------------------------------------------------------------
/**
*/
int nOpendeSpace::GetNumGeoms()
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    return nOpende::SpaceGetNumGeoms( this->id() );
}

//----------------------------------------------------------------------------
/**
*/
dGeomID nOpendeSpace::GetGeom( int i )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    return nOpende::SpaceGetGeom( this->id(), i );
}

//----------------------------------------------------------------------------
/**
    @brief Determines which pairs of geoms in a space may potentially
           intersect, and calls the callback function with each candidate
           pair.

    This is really nothing more than a "shortcut" for calling
    dSpaceCollide( this->id, data, callback );
*/
void nOpendeSpace::Collide( void* data, dNearCallback* callback )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nOpende::SpaceCollide( this->id(), data, callback );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
