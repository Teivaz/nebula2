#ifndef N_DYNAMICMESH_H
#define N_DYNAMICMESH_H
//------------------------------------------------------------------------------
/**
    @class nDynamicMesh
    @ingroup NebulaGraphicsSystem

    Helper class for rendering dynamic geometry, simplifies writing
    to the global dynamic mesh offered by the gfx server.
    
    At the moment it is advisable to flush the gfx server's mesh stream 0
    before use, by doing:
    
    @verbatim
    gfxServer->SetMesh(0,0);
    @endverbatim
    
    This is because the gfx server assumes that if the mesh to be drawn is
    the same as the last mesh that was drawn, then the details are the same.
    If you don't do this then there can be errors when changing primitive types.
    This will hopefully be fixed soon.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_GFXSERVER2_H
#include "gfx2/ngfxserver2.h"
#endif

#ifndef N_MESH2_H
#include "gfx2/nmesh2.h"
#endif

//------------------------------------------------------------------------------
class nDynamicMesh
{
public:
    /// constructor
    nDynamicMesh();
    /// destructor
    ~nDynamicMesh();
    /// begin rendering
    void Begin(nGfxServer2* gfxServ, nPrimitiveType primType, int vertexComponentMask, float*& vertexPointer, ushort*& indexPointer, int& maxNumVertices, int& maxNumIndices);
    /// do an intermediate swap
    void Swap(int numValidVertices, int numValidIndices, float*& vertexPointer, ushort*& indexPointer);
    /// end rendering
    void End(int numValidVertices, int numValidIndices);
    /// get vertex width
    int GetVertexWidth();

private:
    nGfxServer2* gfxServer;
    nMesh2* globalDynMesh;
};

//------------------------------------------------------------------------------
/**
*/
inline
nDynamicMesh::nDynamicMesh() :
    gfxServer(0),
    globalDynMesh(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nDynamicMesh::~nDynamicMesh()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Begin rendering to the dynamic mesh. The method returns pointers
    to the beginning of the vertex buffer and index buffer, and the
    maximum number of vertices and indices which can be written
    before Swap() or End() must be called.

    @param  gfxServ             [in]  pointer to gfx server
    @param  primType            [in]  primitive type
    @param  vertexComponentMask [in]  vertex components
    @param  vertexPointer       [out] will be filled with a pointer to the vertex buffer
    @param  indexPointer        [out] will be filled with a pointer to the index buffer
    @param  maxNumVertices      [out] max number of vertices before calling Swap() or End()
    @param  maxNumIndices       [out] max number of indices before calling Swap() or End()
*/
inline
void
nDynamicMesh::Begin(nGfxServer2* gfxServ, 
                    nPrimitiveType primType,
                    int vertexComponentMask,
                    float*& vertexPointer, 
                    ushort*& indexPointer, 
                    int& maxNumVertices, 
                    int& maxNumIndices)
{
    n_assert(0 == this->globalDynMesh);
    n_assert(gfxServ);
    n_assert(vertexComponentMask != 0);
    
    this->gfxServer = gfxServ;
    this->globalDynMesh = this->gfxServer->LockDynamicMesh();
    n_assert(this->globalDynMesh);

    // fix the dynamic mesh's vertex width and number of vertices
    int vertexBufferByteSize = this->globalDynMesh->GetVertexBufferByteSize();

    this->globalDynMesh->SetPrimitiveType(primType);    
    this->globalDynMesh->SetVertexComponents(vertexComponentMask);
    int newNumVertices = vertexBufferByteSize / (this->globalDynMesh->GetVertexWidth() * sizeof(float));
    this->globalDynMesh->SetNumVertices(newNumVertices);
    vertexPointer = this->globalDynMesh->LockVertices();
    indexPointer  = this->globalDynMesh->LockIndices();
    maxNumVertices = this->globalDynMesh->GetNumVertices();
    maxNumIndices  = this->globalDynMesh->GetNumIndices();

    this->gfxServer->SetMesh(0, this->globalDynMesh);
}

//------------------------------------------------------------------------------
/**
    Do an intermediate swap. Call this method when the max number of
    vertices or the max number of indices returned by Begin() have
    been written into the vertex and index buffers. The internal 
    dynamic mesh will be rendered, and render attributes will be returned.
    Note that the contents of the vertex and index buffer will be discarded,
    so everything must be overwritten!

    This method will unlock the global dynamic mesh, immediately render it
    through the gfx server, and lock it again.

    @param  numVertices     [in] number of vertices written to the vertex buffer
    @param  numIndices      [in] number of indices written to the index buffer
    @param  vertexPointer   [out] new vertex buffer pointer for writing new vertices
    @param  indexPointer    [out] new index buffer pointer for writing new indices
*/
inline
void
nDynamicMesh::Swap(int numVertices, int numIndices, float*& vertexPointer, ushort*& indexPointer)
{
    n_assert(this->globalDynMesh);
    n_assert(this->gfxServer);

    // unlock ...
    this->globalDynMesh->UnlockVertices();
    this->globalDynMesh->UnlockIndices();

    // render ...
    this->gfxServer->SetVertexRange(0, numVertices);
    this->gfxServer->SetIndexRange(0, numIndices);
    this->gfxServer->Draw();

    // lock and return
    vertexPointer = this->globalDynMesh->LockVertices();
    indexPointer  = this->globalDynMesh->LockIndices();
}

//------------------------------------------------------------------------------
/**
    Finish rendering. Call this method when no more dynamic geometry 
    needs to be rendered. This method will do a final Draw() call to
    the gfx server with the remaining valid vertices and indices.

    @param  numVertices     number of valid vertices in the vertex buffer
    @param  numIndices      number of valid indices in the vertex buffer
*/
inline
void
nDynamicMesh::End(int numVertices, int numIndices)
{
    n_assert(this->globalDynMesh);
    n_assert(this->gfxServer);

    // unlock ...
    this->globalDynMesh->UnlockVertices();
    this->globalDynMesh->UnlockIndices();

    // render ...
    this->gfxServer->SetVertexRange(0, numVertices);
    this->gfxServer->SetIndexRange(0, numIndices);
    this->gfxServer->Draw();

    // give up ownership of the global dynamic mesh
    this->gfxServer->UnlockDynamicMesh(this->globalDynMesh);
    this->globalDynMesh = 0;
}

//------------------------------------------------------------------------------
/**
    Returns the vertex width for the current component mask.
    
    @return the vertex width in floats.
*/
inline
int
nDynamicMesh::GetVertexWidth()
{
    n_assert(this->globalDynMesh);
    return this->globalDynMesh->GetVertexWidth();
}

//------------------------------------------------------------------------------
#endif
