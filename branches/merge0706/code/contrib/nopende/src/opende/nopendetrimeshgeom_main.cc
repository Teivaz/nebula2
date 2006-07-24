//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendetrimeshgeom.h"

nNebulaScriptClass(nOpendeTriMeshGeom, "nopendegeom");

#include "opende/nopendetrimesh.h"
#include "opende/nopendeserver.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeTriMeshGeom::nOpendeTriMeshGeom() :
    triMesh(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::Create( const char* space )
{
    nOpendeGeom::Create( space );
    this->geomId = nOpende::CreateTriMesh( this->getSpace(), 0, 0, 0, 0 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeTriMeshGeom::~nOpendeTriMeshGeom()
{
    // release data?
    if ( this->triMesh )
    {
        this->triMesh->RemRef();
        if ( 0 == this->triMesh->GetRef() )
        {
            this->triMesh->Remove();
            n_delete( this->triMesh );
        }
    }
}

//----------------------------------------------------------------------------
/**
    @param triMeshId Tri mesh id that corresponds to an existing tri mesh.

    Sets the mesh data, if it has been previously set the new data will
    replace the old data.
*/
void nOpendeTriMeshGeom::SetMesh( const char* triMeshId )
{
    // release old data?
    if ( this->triMesh )
    {
        this->triMesh->RemRef();
        if ( 0 == this->triMesh->GetRef() )
        {
            this->triMesh->Remove();
            n_delete( this->triMesh );
        }
    }

    // store pointer to new data
    this->triMesh = nOpendeServer::opendeServer->FindTriMesh( triMeshId );
    this->triMesh->AddRef();

    n_assert( this->triMesh && "triMeshId does not match an existing mesh!" );

    // associate data with geom
    this->SetData( this->triMesh->data );
}

//----------------------------------------------------------------------------
/**
    @return Valid mesh id if the mesh data has been previously specified by
            SetMesh(), or "none" if no mesh data has been set yet.
*/
const char* nOpendeTriMeshGeom::GetMesh()
{
    if ( this->triMesh )
        return this->triMesh->GetName();
    else
        return "none";
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::SetCallback( dTriCallback* callback )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    nOpende::GeomTriMeshSetCallback( this->id(), callback );
}

//----------------------------------------------------------------------------
/**
*/
dTriCallback* nOpendeTriMeshGeom::GetCallback()
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    return nOpende::GeomTriMeshGetCallback( this->id() );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::SetArrayCallback( dTriArrayCallback* arrayCallback )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    nOpende::GeomTriMeshSetArrayCallback( this->id(), arrayCallback );
}

//----------------------------------------------------------------------------
/**
*/
dTriArrayCallback* nOpendeTriMeshGeom::GetArrayCallback()
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    return nOpende::GeomTriMeshGetArrayCallback( this->id() );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::SetRayCallback( dTriRayCallback* callback )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    nOpende::GeomTriMeshSetRayCallback( this->id(), callback );
}

//----------------------------------------------------------------------------
/**
*/
dTriRayCallback* nOpendeTriMeshGeom::GetRayCallback()
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    return nOpende::GeomTriMeshGetRayCallback( this->id() );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::SetData( dTriMeshDataID data )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    nOpende::GeomTriMeshSetData( this->id(), data );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::EnableTC( int geomClass, bool enable )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    nOpende::GeomTriMeshEnableTC( this->id(), geomClass, enable );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::EnableTC( const char* geomClass, bool enable )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    nOpende::GeomTriMeshEnableTC( this->id(),
                                  nOpendeMarshal::StringToGeomClass( geomClass ),
                                  enable );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeTriMeshGeom::IsTCEnabled( int geomClass )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    return nOpende::GeomTriMeshIsTCEnabled( this->id(), geomClass );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeTriMeshGeom::IsTCEnabled( const char* geomClass )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    return nOpende::GeomTriMeshIsTCEnabled( this->id(),
                                            nOpendeMarshal::StringToGeomClass(
                                            geomClass ) );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::ClearTCCache()
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    nOpende::GeomTriMeshClearTCCache( this->id() );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::GetTriangle( int index,
                                      vector3& v0, vector3& v1, vector3& v2 )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    nOpende::GeomTriMeshGetTriangle( this->id(), index, v0, v1, v2 );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeTriMeshGeom::GetPoint( int index, float u, float v, vector3& out )
{
    n_assert( this->id() && "nOpendeTriMeshGeom::id not valid!" );
    nOpende::GeomTriMeshGetPoint( this->id(), index, u, v, out );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
