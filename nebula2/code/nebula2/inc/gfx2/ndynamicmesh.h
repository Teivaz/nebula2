#ifndef N_DYNAMICMESH_H
#define N_DYNAMICMESH_H
//------------------------------------------------------------------------------
/**
    @class nDynamicMesh
    @ingroup NebulaGraphicsSystem

    Helper class for rendering dynamic geometry, simplifies writing
    to the global dynamic mesh offered by the gfx server.
    
    (C) 2003 RadonLabs GmbH
*/

#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"

//------------------------------------------------------------------------------
class nDynamicMesh
{
public:
    /// constructor
    nDynamicMesh();
    /// destructor
    ~nDynamicMesh();
	/// initialize the dynamic mesh
    bool Initialize(nGfxServer2* gfxServer, nGfxServer2::PrimitiveType primType, int vertexComponents, int usageFlags, bool indexedRendering);
    /// if this returns false, call Initialize()
    bool IsValid() const;
    /// begin indexed rendering
    void BeginIndexed(float*& vertexPointer, ushort*& indexPointer, int& maxNumVertices, int& maxNumIndices);
    /// do an intermediate swap for indexed rendering
    void SwapIndexed(int numValidVertices, int numValidIndices, float*& vertexPointer, ushort*& indexPointer);
    /// end indexed rendering
    void EndIndexed(int numValidVertices, int numValidIndices);
    /// begin non-indexed rendering
    void Begin(float*& vertexPointer, int& maxNumVertices);
    /// do an intermediate swap for non-indexed rendering
    void Swap(int numValidVertices, float*& vertexPointer);
    /// end non-indexed rendering
    void End(int numValidVertices);

private:
    enum
    {	
        VertexBufferSize = 16384,                   // number of vertices
        IndexBufferSize  = 3 * VertexBufferSize,    // number of indices
    };
    nGfxServer2* gfxServer;
    bool indexedRendering;
    nRef<nMesh2> refMesh;
    nGfxServer2::PrimitiveType primitiveType;
};

//------------------------------------------------------------------------------
/**
*/
inline
nDynamicMesh::nDynamicMesh() :
    gfxServer(0),
    indexedRendering(true),
    primitiveType(nGfxServer2::TriangleList)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nDynamicMesh::~nDynamicMesh()
{
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Return true if object is valid, otherwise Initialize() must be called
    to prepare the object for rendering. A dynamic mesh can become
    invalid anytime, thus make sure you check before each call to
    Begin()/End().
*/
inline
bool
nDynamicMesh::IsValid() const
{
    return this->refMesh.isvalid();
}

//------------------------------------------------------------------------------
/**
    Initialize the dynamic mesh. This will create or lookup a mesh
    which is shared with all other dynamic mesh objects with the
    same vertex components and usage flags.
    This method must be called whenever a call to IsValid() returns false.

    @param  gfxServ             pointer to gfx server
    @param  primType            primitive type
    @param  vertexComponents    vertex component mask (see nMesh2)
    @param  usageFlags          usage flags (see nMesh2)
    @param  indexed             true if indexed primitive rendering is intended
    @return                     true if initialized successful
*/
inline
bool
nDynamicMesh::Initialize(nGfxServer2* gfxServ, 
                         nGfxServer2::PrimitiveType primType, 
                         int vertexComponents, 
                         int usageFlags, 
                         bool indexed)
{
    n_assert(!this->IsValid());
    n_assert(gfxServ);

    this->primitiveType = primType;
    this->gfxServer = gfxServ;
    this->indexedRendering = indexed;

    // build resource sharing name
    char resName[128];
    strcpy(resName, "dyn_");
    int charIndex = strlen(resName);
    if (vertexComponents & nMesh2::Coord)       resName[charIndex++] = 'a';
    if (vertexComponents & nMesh2::Normal)      resName[charIndex++] = 'b';
    if (vertexComponents & nMesh2::Tangent)     resName[charIndex++] = 'c';
    if (vertexComponents & nMesh2::Binormal)    resName[charIndex++] = 'd';
    if (vertexComponents & nMesh2::Color)       resName[charIndex++] = 'e';
    if (vertexComponents & nMesh2::Uv0)         resName[charIndex++] = 'f';
    if (vertexComponents & nMesh2::Uv1)         resName[charIndex++] = 'g';
    if (vertexComponents & nMesh2::Uv2)         resName[charIndex++] = 'h';
    if (vertexComponents & nMesh2::Uv3)         resName[charIndex++] = 'i';
    if (vertexComponents & nMesh2::Weights)     resName[charIndex++] = 'f';
    if (vertexComponents & nMesh2::JIndices)    resName[charIndex++] = 'g';
    if (usageFlags & nMesh2::NPatch)            resName[charIndex++] = 'h';
    if (usageFlags & nMesh2::RTPatch)           resName[charIndex++] = 'i';
    if (usageFlags & nMesh2::PointSprite)       resName[charIndex++] = 'j';
    resName[charIndex] = 0;

    // create shared mesh object
    nMesh2* mesh = this->gfxServer->NewMesh(resName);
    n_assert(mesh);
    this->refMesh = mesh;
    
    // initialize the mesh
    if (!mesh->IsValid())
    {
        mesh->SetUsage(usageFlags | nMesh2::WriteOnly);
        mesh->SetVertexComponents(vertexComponents);
        mesh->SetNumVertices(VertexBufferSize);
        mesh->SetNumIndices(IndexBufferSize);
        mesh->Load();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Begin indexed rendering to the dynamic mesh. The method returns pointers
    to the beginning of the vertex buffer and index buffer, and the
    maximum number of vertices and indices which can be written
    before Swap() or End() must be called.

    @param  vertexPointer   [out] will be filled with a pointer to the vertex buffer
    @param  indexPointer    [out] will be filled with a pointer to the index buffer
    @param  maxNumVertices  [out] max number of vertices before calling Swap() or End()
    @param  maxNumIndices   [out] max number of indices before calling Swap() or End()
*/
inline
void
nDynamicMesh::BeginIndexed(float*& vertexPointer, 
                           ushort*& indexPointer, 
                           int& maxNumVertices, 
                           int& maxNumIndices)
{
    n_assert(this->IsValid());
    n_assert(this->indexedRendering);
    n_assert(this->gfxServer);
    nMesh2* mesh = this->refMesh.get();

    this->gfxServer->SetMesh(mesh);

    vertexPointer  = mesh->LockVertices();
    indexPointer   = mesh->LockIndices();
    maxNumVertices = mesh->GetNumVertices();
    maxNumIndices  = mesh->GetNumIndices();
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
nDynamicMesh::SwapIndexed(int numVertices, int numIndices, float*& vertexPointer, ushort*& indexPointer)
{
    n_assert(this->IsValid());
    n_assert(this->indexedRendering);
    n_assert(this->gfxServer);
    nMesh2* mesh = this->refMesh.get();
    mesh->UnlockVertices();
    mesh->UnlockIndices();
    this->gfxServer->SetVertexRange(0, numVertices);
    this->gfxServer->SetIndexRange(0, numIndices);
    this->gfxServer->DrawIndexedNS(this->primitiveType);
    vertexPointer = mesh->LockVertices();
    indexPointer  = mesh->LockIndices();
}

//------------------------------------------------------------------------------
/**
    Finish indexed rendering. Call this method when no more dynamic geometry 
    needs to be rendered. This method will do a final DrawIndexed() call to
    the gfx server with the remaining valid vertices and indices.

    @param  numVertices     number of valid vertices in the vertex buffer
    @param  numIndices      number of valid indices in the vertex buffer
*/
inline
void
nDynamicMesh::EndIndexed(int numVertices, int numIndices)
{
    n_assert(this->IsValid());
    n_assert(this->indexedRendering);
    n_assert(this->gfxServer);
    nMesh2* mesh = this->refMesh.get();
    mesh->UnlockVertices();
    mesh->UnlockIndices();
    this->gfxServer->SetVertexRange(0, numVertices);
    this->gfxServer->SetIndexRange(0, numIndices);
    this->gfxServer->DrawIndexedNS(this->primitiveType );
    this->gfxServer->SetMesh(0);
}

//------------------------------------------------------------------------------
/**
    Begin non-indexed rendering to the dynamic mesh. 

    @param  vertexPointer       [out] will be filled with a pointer to the vertex buffer
    @param  maxNumVertices      [out] max number of vertices before calling Swap() or End()
*/
inline
void
nDynamicMesh::Begin(float*& vertexPointer, int& maxNumVertices)
{
    n_assert(this->IsValid());
    n_assert(!this->indexedRendering);
    n_assert(this->gfxServer);
    nMesh2* mesh = this->refMesh.get();

    this->gfxServer->SetMesh(mesh);

    vertexPointer  = mesh->LockVertices();
    maxNumVertices = mesh->GetNumVertices();
}

//------------------------------------------------------------------------------
/**
    Do an intermediate swap for non-indexed rendering.

    @param  numVertices     [in] number of vertices written to the vertex buffer
    @param  vertexPointer   [out] new vertex buffer pointer for writing new vertices
*/
inline
void
nDynamicMesh::Swap(int numVertices, float*& vertexPointer)
{
    n_assert(this->IsValid());
    n_assert(!this->indexedRendering);
    n_assert(this->gfxServer);
    nMesh2* mesh = this->refMesh.get();

    mesh->UnlockVertices();
    this->gfxServer->SetVertexRange(0, numVertices);
    this->gfxServer->DrawNS(this->primitiveType);
    vertexPointer = mesh->LockVertices();
}

//------------------------------------------------------------------------------
/**
    Finish non-indexed rendering. 

    @param  numVertices     number of valid vertices in the vertex buffer
*/
inline
void
nDynamicMesh::End(int numVertices)
{
    n_assert(this->IsValid());
    n_assert(!this->indexedRendering);
    n_assert(this->gfxServer);
    nMesh2* mesh = this->refMesh.get();
    mesh->UnlockVertices();
    if (0 != numVertices)
    {
        this->gfxServer->SetVertexRange(0, numVertices);
        this->gfxServer->DrawNS(this->primitiveType);
    }
    this->gfxServer->SetMesh(0);
}

//------------------------------------------------------------------------------
#endif
