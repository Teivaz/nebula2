#ifndef N_D3D9MESH_H
#define N_D3D9MESH_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Mesh
    @ingroup NebulaD3D9GraphicsSystem

    nMesh2 subclass for Direct3D9.

    (C) 2003 RadonLabs GmbH
*/
#include "gfx2/nmesh2.h"
#include "gfx2/nd3d9server.h"

//------------------------------------------------------------------------------
class nD3D9Mesh : public nMesh2
{
public:
    /// constructor
    nD3D9Mesh();
    /// destructor
    virtual ~nD3D9Mesh();
    /// supports async resource loading
    virtual bool CanLoadAsync() const;
    /// lock vertex buffer
    virtual float* LockVertices();
    /// unlock vertex buffer
    virtual void UnlockVertices();
    /// lock index buffer
    virtual ushort* LockIndices();
    /// unlock index buffer
    virtual void UnlockIndices();
    /// get an estimated byte size of the resource data (for memory statistics)
    virtual int GetByteSize();

protected:
    /// load mesh resource
    virtual bool LoadResource();
    /// unload mesh resource
    virtual void UnloadResource();

private:
    friend class nD3D9Server;
    
    /// create the d3d vertex buffer
    void CreateVertexBuffer();
    /// create the d3d index buffer
    void CreateIndexBuffer();
    /// create the vertex declaration
    void CreateVertexDeclaration();
    /// get d3d vertex buffer
    IDirect3DVertexBuffer9* GetVertexBuffer();
    /// get d3d index buffer
    IDirect3DIndexBuffer9* GetIndexBuffer();
    /// get the d3d vertex declaration
    IDirect3DVertexDeclaration9* GetVertexDeclaration();
    /// load n3d2 file
    bool LoadN3d2File();
    /// load nvx2 file
    bool LoadNvx2File();
    /// update the group bounding boxes (slow!)
    void UpdateGroupBoundingBoxes();

    nAutoRef<nD3D9Server> refGfxServer;
    DWORD d3dVBLockFlags;
    DWORD d3dIBLockFlags;
    IDirect3DVertexBuffer9* vertexBuffer;
    IDirect3DIndexBuffer9* indexBuffer;
    IDirect3DVertexDeclaration9* vertexDeclaration;
    void* privVertexBuffer;                     // valid if Usage==ReadOnly
    void* privIndexBuffer;                      // valif if Usage==ReadOnly
};

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DVertexBuffer9* 
nD3D9Mesh::GetVertexBuffer()
{
    n_assert(this->IsValid());
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DIndexBuffer9* 
nD3D9Mesh::GetIndexBuffer()
{
    n_assert(this->IsValid());
    return this->indexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DVertexDeclaration9*
nD3D9Mesh::GetVertexDeclaration()
{
    n_assert(this->IsValid());
    return this->vertexDeclaration;
}

//------------------------------------------------------------------------------
#endif

