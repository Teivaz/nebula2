//------------------------------------------------------------------------------
//  nopendeserver_main.cc
//
//  (c) 2004  Vadim Macagon
//
//  nOpendeServer is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "opende/nopendeserver.h"
nNebulaScriptClass(nOpendeServer, "nroot");

#include "opende/nopendetrimesh.h"

nOpendeServer* nOpendeServer::opendeServer = 0;

//------------------------------------------------------------------------------
/**
*/
nOpendeServer::nOpendeServer() : 
    surfaceArray( 0 ), 
    numSurfaces( 0 ),
    inBeginSurfaces( false )
{
    nOpendeServer::opendeServer = this;
}

//------------------------------------------------------------------------------
/**
*/
nOpendeServer::~nOpendeServer()
{
    // release any meshes that may still be around...
    nOpendeTriMesh* mesh;
    while ( (mesh = (nOpendeTriMesh*)this->meshList.GetHead()) ) 
    {
        mesh->RemRef();
        if ( 0 == mesh->GetRef() )
        {
            mesh->Remove();
            n_delete( mesh );
        }
    }
      
    dCloseODE(); // get ODE to cleanup
  
    nOpendeServer::opendeServer = 0;
}

//-------------------------------------------------------------------
/**
    @brief Creates a new mesh (or reuses an existing one).
    @param id Identifier for the mesh, the identifier is used to 
              share geoms.
    @param file Path to mesh file (n3d2 or nvx2).
    @param group Mesh group index to load.
    @return A pointer to a mesh.
  
    Currently the mesh file must only contain the coord component
    for vertices and no others.
  
    If you want to create multiple mesh geoms that all use a single
    copy of the mesh geometry create all these geoms with the same
    identifier.
  
    NOTE: The mesh geometry will be loaded from file only if it
    hasn't been already. If you try to create two geoms with the
    same identifier but different file paths, only the first file
    will be loaded.
*/
nOpendeTriMesh* nOpendeServer::NewTriMesh( const char* id, 
                                           const char* file,
                                           int group )
{
    n_assert( id );

    // mesh already exists?
    nOpendeTriMesh* mesh = (nOpendeTriMesh*)this->meshList.Find( id );
    if ( !mesh ) 
    {
        mesh = n_new( nOpendeTriMesh( id ) );
        this->meshList.AddTail( mesh );
    }
  
    if ( !mesh->IsLoaded() )
        mesh->Load( nKernelServer::ks->GetFileServer(), file, group );
    
    n_assert( mesh->IsLoaded() && "failed to load mesh" );
    
    return mesh;
}

//------------------------------------------------------------------------------
/**
    @brief Delete a tri mesh previously created by NewTriMesh().
    @param id A mesh identifier.
    
    The mesh matching the specified id must not be in use (i.e. no tri mesh
    geoms should exist that still reference it), otherwise this will assert.
    If there is no mesh that matches the specified id this will do nothing.
*/
void nOpendeServer::DeleteTriMesh( const char* id )
{
    n_assert( id );
    nOpendeTriMesh* mesh = (nOpendeTriMesh*)this->meshList.Find( id );
    if ( mesh )
    {
        n_assert( (0 == mesh->GetRef()) && "TriMesh is still in use!" );
    
        mesh->Remove();
        n_delete( mesh );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get the tri mesh instance for the specified id.
    @return Pointer to a mesh instance, or zero if no meshes match the
            specified id.
*/
nOpendeTriMesh* nOpendeServer::FindTriMesh( const char* id )
{
    n_assert( id );
    
    return (nOpendeTriMesh*)this->meshList.Find( id );
}

//------------------------------------------------------------------------------
/**
*/
void nOpendeServer::BeginSurfaces( int num )
{
    n_assert( !this->inBeginSurfaces );
    n_assert( num > 0 );
    this->inBeginSurfaces = true;
    this->surfaceArray = n_new( nOpendeSurface[num] );
    this->numSurfaces = num;
}

//------------------------------------------------------------------------------
/**
    @brief Define a new surface.
*/
void nOpendeServer::AddSurface( int index, const char* name )
{
    n_assert( this->inBeginSurfaces );
    n_assert( index < this->numSurfaces );
    this->surfaceArray[index].SetName( name );
}

//------------------------------------------------------------------------------
/**
*/
void nOpendeServer::EndSurfaces()
{
    n_assert( this->inBeginSurfaces );
}

//------------------------------------------------------------------------------
/**
    @brief Get the surface that corresponds to the specified index.
*/
nOpendeSurface* nOpendeServer::GetSurface( int index )
{
    n_assert( this->inBeginSurfaces );
    n_assert( index < this->numSurfaces );
  
    return &(this->surfaceArray[index]);
}

//------------------------------------------------------------------------------
/**
    @brief Get the surface index that corresponds to the specified surface.
  
    This does a linear search through all surfaces, so can be slow, ideally
    you should cache the index you get from here and reuse it.
*/
int nOpendeServer::GetSurfaceIndex( const char* name )
{
    n_assert( this->inBeginSurfaces );
  
    for ( int i = 0; i < this->numSurfaces; i++ )
        if ( 0 == strcmp( this->surfaceArray[i].GetName(), name ) )
            return i;
    return -1;
}

//-------------------------------------------------------------------
/**
*/
void nOpendeServer::EnableSurfaceParam( int index, const char* param )
{
    n_assert( this->inBeginSurfaces );
    n_assert( index < this->numSurfaces );
  
    nOpendeSurface* surf = this->GetSurface( index );
  
    if ( 0 == strcmp( param, "fdir1" ) )
        surf->EnableFlag( dContactFDir1 );
    else if ( 0 == strcmp( param, "approxfdir1" ) )
        surf->EnableFlag( dContactApprox1_1 );
    else if ( 0 == strcmp( param, "approxfdir2" ) )
        surf->EnableFlag( dContactApprox1_2 );
    else if ( 0 == strcmp( param, "approxfboth" ) )
        surf->EnableFlag( dContactApprox1 );
    else
        n_error( "nOpendeServer::EnableSurfaceParam() - unrecognized param %s", param );
}

//-------------------------------------------------------------------
/**
*/
void nOpendeServer::SetSurfaceParam( int index, const char* param, 
                                     float value )
{
    n_assert( this->inBeginSurfaces );
    n_assert( index < this->numSurfaces );
  
    nOpendeSurface* surf = this->GetSurface( index );
  
    if ( 0 == strcmp( param, "mu" ) )
    {
        surf->SetMU( value );
    }
    else if ( 0 == strcmp( param, "mu2" ) )
    {   
        surf->SetMU2( value );
        surf->EnableFlag( dContactMu2 );
    }
    else if ( 0 == strcmp( param, "cfm" ) )
    {
        surf->SetSoftCFM( value );
        surf->EnableFlag( dContactSoftCFM );
    }
    else if ( 0 == strcmp( param, "erp" ) )
    {
        surf->SetSoftERP( value );
        surf->EnableFlag( dContactSoftERP );
    }
    else if ( 0 == strcmp( param, "bounce" ) )
    {
        surf->SetBounce( value );
        surf->EnableFlag( dContactBounce );
    }
    else if ( 0 == strcmp( param, "bvel" ) )
    {
        surf->SetBounceVel( value );
        surf->EnableFlag( dContactBounce );
    }
    else if ( 0 == strcmp( param, "motion1" ) )
    {
        surf->SetMotion1( value );
        surf->EnableFlag( dContactMotion1 );
    }
    else if ( 0 == strcmp( param, "motion2" ) )
    {
        surf->SetMotion2( value );
        surf->EnableFlag( dContactMotion2 );
    }
    else if ( 0 == strcmp( param, "slip1" ) )
    {
        surf->SetSlip1( value );
        surf->EnableFlag( dContactSlip1 );
    }
    else if ( 0 == strcmp( param, "slip2" ) )
    {
        surf->SetSlip2( value );
        surf->EnableFlag( dContactSlip2 );
    }
    else
        n_error( "nOpendeServer::SetSurfaceParam() - unrecognized param %s", param );
}
