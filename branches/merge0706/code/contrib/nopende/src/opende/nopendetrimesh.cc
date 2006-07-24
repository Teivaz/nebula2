//--------------------------------------------------------------------
//  nopendetrimesh.cc
//
//  (c) 2004  Vadim Macagon
//
//  nOpendeTriMesh is licensed under the terms of the Nebula License.
//--------------------------------------------------------------------
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "opende/nopendetrimesh.h"
#include "mathlib/vector.h"
#include "mathlib/triangle.h"
#include "mathlib/line.h"
#include "opende/nopendelayer.h"
#include "gfx2/nmeshgroup.h"
#include "gfx2/nn3d2loader.h"
#include "gfx2/nnvx2loader.h"
#include "gfx2/nmesh2.h"

//------------------------------------------------------------------------------
/**
*/
nOpendeTriMesh::nOpendeTriMesh( const char* id ) :
    nHashNode(id), refCount(0),
    numVertices(0), numFaces(0), vertexData(0), faceData(0),
    isLoaded(false), data(0)
{
    //
}

//------------------------------------------------------------------------------
/**
*/
nOpendeTriMesh::~nOpendeTriMesh()
{
    if ( this->data )
    {
        nOpende::GeomTriMeshDataDestroy( this->data );
        this->data = 0;
    }
    if ( this->vertexData )
    {
        n_free( this->vertexData );
        this->vertexData = 0;
    }
    if ( this->faceData )
    {
        n_free( this->faceData );
        this->faceData = 0;
    }
    n_assert( 0 == this->refCount );
}

//------------------------------------------------------------------------------
/**
*/
int nOpendeTriMesh::AddRef()
{
    return ++refCount;
}

//------------------------------------------------------------------------------
/**
*/
int nOpendeTriMesh::RemRef()
{
    n_assert( refCount > 0 );
    return --refCount;
}

//------------------------------------------------------------------------------
/**
*/
int nOpendeTriMesh::GetRef()
{
    return refCount;
}

//------------------------------------------------------------------------------
/**
*/
bool nOpendeTriMesh::IsLoaded()
{
    return this->isLoaded;
}

//------------------------------------------------------------------------------
/**
*/
void nOpendeTriMesh::Begin( int numVerts, int numTris )
{
    n_assert( !this->isLoaded );
    n_assert( !this->vertexData );
    n_assert( !this->faceData );

    this->numVertices = numVerts;
    this->numFaces    = numTris;
    this->vertexData  = (float*) n_malloc(numVerts * 3 * sizeof(float));
    this->faceData    = (int*)   n_malloc(numTris  * 3 * sizeof(int));
}

//------------------------------------------------------------------------------
/**
*/
void nOpendeTriMesh::SetVertex( int index, vector3& v )
{
    n_assert( this->vertexData );
    n_assert( (index >= 0) && (index < this->numVertices) );

    float* ptr = this->vertexData + 3 * index;
    ptr[0] = v.x;
    ptr[1] = v.y;
    ptr[2] = v.z;
}

//------------------------------------------------------------------------------
/**
*/
void nOpendeTriMesh::SetTriangle( int index, int p0Index, int p1Index, int p2Index )
{
    n_assert( this->faceData );
    n_assert( (index >= 0) && (index < this->numFaces) );
    n_assert( (p0Index >= 0) && (p0Index < this->numVertices) );
    n_assert( (p1Index >= 0) && (p1Index < this->numVertices) );
    n_assert( (p2Index >= 0) && (p2Index < this->numVertices) );

    int* ptr = this->faceData + 3 * index;
    ptr[0] = p0Index;
    ptr[1] = p1Index;
    ptr[2] = p2Index;
}

//------------------------------------------------------------------------------
/**
*/
void nOpendeTriMesh::End()
{
    n_assert( (this->numVertices > 0) && (this->numFaces > 0) );
    n_assert( this->faceData && this->vertexData );
    n_assert( !this->data );
    n_assert( !this->isLoaded );

    this->data = nOpende::GeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle( this->data,
                                 this->vertexData, sizeof(float) * 3,
                                 this->numVertices, this->faceData,
                                 this->numFaces * 3, sizeof(int) * 3 );
    this->isLoaded = true;
}

//------------------------------------------------------------------------------
/**
*/
bool nOpendeTriMesh::Load( nFileServer2* fs, const char* fname, int group )
{
    n_assert( !this->isLoaded );

    if ( strstr( fname, ".n3d2" ) )
        return this->LoadN3d2( fs, fname, group );
    else if ( strstr( fname, ".nvx2" ) )
        return this->LoadNvx2( fs, fname, group );
    else
    {
        n_printf( "nOpendeTriMesh: Unknown file format '%s'\n", fname );
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool nOpendeTriMesh::LoadN3d2( nFileServer2* fs, const char* fname, int group )
{
    nN3d2Loader loader;
    loader.SetFilename( fname );
    loader.SetIndexType( nMeshLoader::Index32 );

    if ( loader.Open() )
    {
        n_assert( group < loader.GetNumGroups() );
        // coord component must be present
        n_assert( (loader.GetVertexComponents() & nMesh2::Coord)
                  && "Coord vertex component must be present!" );
        // coord component is the ONLY one that should be present
        n_assert( (loader.GetVertexWidth() == 3)
                  && "Only the Coord vertex component must be present!" );

        const nMeshGroup& meshGroup = loader.GetGroupAt( group );
        this->Begin( meshGroup.GetNumVertices(), meshGroup.GetNumIndices() / 3 );
        int vertexBufSize = loader.GetNumVertices() * 3 * sizeof(float);
        int indexBufSize = loader.GetNumIndices() * sizeof(int);
        if ( loader.GetNumGroups() > 1 )
        {
            // load ALL vertices & indices from the file
            // FIXME: could the loader be modified to load only data for a group?
            // We wouldn't have to allocate all this extra memory if it could.
            float* vertices = (float*)n_malloc( vertexBufSize );
            n_assert( vertices );
            loader.ReadVertices( vertices, vertexBufSize );
            int* indices = (int*)n_malloc( indexBufSize );
            n_assert( indices );
            loader.ReadIndices( indices, indexBufSize );
            // copy the mesh group data we want
            memcpy( this->vertexData,
                    vertices + meshGroup.GetFirstVertex() * 3 * sizeof(float),
                    meshGroup.GetNumVertices() * 3 * sizeof(float) );
            memcpy( this->faceData,
                    indices + meshGroup.GetFirstIndex() * sizeof(int),
                    meshGroup.GetNumIndices() * sizeof(int) );
            // release all the other data we don't need
            if ( vertices )
                n_free( vertices );
            if ( indices )
                n_free( indices );
        }
        else
        {
            // just one group so no need to allocate any extra memory
            loader.ReadVertices( this->vertexData, vertexBufSize );
            loader.ReadIndices( this->faceData, indexBufSize );
        }
        this->End();

        loader.Close();
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool nOpendeTriMesh::LoadNvx2( nFileServer2* fs, const char* fname, int group )
{
    nNvx2Loader loader;
    loader.SetFilename( fname );
    loader.SetIndexType( nMeshLoader::Index32 );

    if ( loader.Open() )
    {
        n_assert( group < loader.GetNumGroups() );
        // coord component must be present
        n_assert( (loader.GetVertexComponents() & nMesh2::Coord)
                  && "Coord vertex component must be present!" );
        // coord component is the ONLY one that should be present
        n_assert( (loader.GetVertexWidth() == 3)
                  && "Only the Coord vertex component must be present!" );

        const nMeshGroup& meshGroup = loader.GetGroupAt( group );
        this->Begin( meshGroup.GetNumVertices(), meshGroup.GetNumIndices() / 3 );
        int vertexBufSize = loader.GetNumVertices() * 3 * sizeof(float);
        int indexBufSize = loader.GetNumIndices() * sizeof(int);
        if ( loader.GetNumGroups() > 1 )
        {
            // load ALL vertices & indices from the file
            // FIXME: could the loader be modified to load only data for a group?
            // We wouldn't have to allocate all this extra memory if it could.
            float* vertices = (float*)n_malloc( vertexBufSize );
            n_assert( vertices );
            loader.ReadVertices( vertices, vertexBufSize );
            int* indices = (int*)n_malloc( indexBufSize );
            n_assert( indices );
            loader.ReadIndices( indices, indexBufSize );
            // copy the mesh group data we want
            memcpy( this->vertexData,
                    vertices + meshGroup.GetFirstVertex() * 3 * sizeof(float),
                    meshGroup.GetNumVertices() * 3 * sizeof(float) );
            memcpy( this->faceData,
                    indices + meshGroup.GetFirstIndex() * sizeof(int),
                    meshGroup.GetNumIndices() * sizeof(int) );
            // release all the other data we don't need
            if ( vertices )
                n_free( vertices );
            if ( indices )
                n_free( indices );
        }
        else
        {
            // just one group so no need to allocate any extra memory
            loader.ReadVertices( this->vertexData, vertexBufSize );
            loader.ReadIndices( this->faceData, indexBufSize );
        }
        this->End();

        loader.Close();
        return true;
    }

    return false;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
