#ifndef N_D3D9MESH_H
#define N_D3D9MESH_H
//------------------------------------------------------------------------------
/**
    nMesh2 subclass for Direct3D9.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_MESH2_H
#include "gfx2/nmesh2.h"
#endif

#ifndef N_D3D9SERVER_H
#include "gfx2/nd3d9server.h"
#endif

#undef N_DEFINES
#define N_DEFINES nD3D9Mesh
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nD3D9Mesh : public nMesh2
{
public:
    /// constructor
    nD3D9Mesh();
    /// destructor
    virtual ~nD3D9Mesh();
    /// load mesh resource
    virtual bool Load();
    /// unload mesh resource
    virtual void Unload();
    /// lock vertex buffer
    virtual float* LockVertices();
    /// unlock vertex buffer
    virtual void UnlockVertices();
    /// lock index buffer
    virtual ushort* LockIndices();
    /// unlock index buffer
    virtual void UnlockIndices();

    static nKernelServer* kernelServer;

private:
    friend class nD3D9Server;
    
    /// create the d3d vertex buffer
    void CreateVertexBuffer();
    /// create the d3d index buffer
    void CreateIndexBuffer();
    /// create the vertex declaration
    void CreateVertexDeclaration();
    /// get d3d vertex buffer
    IDirect3DVertexBuffer9* GetVertexBuffer() const;
    /// get d3d index buffer
    IDirect3DIndexBuffer9* GetIndexBuffer() const;
    /// get the d3d vertex declaration
    IDirect3DVertexDeclaration9* GetVertexDeclaration() const;
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
nD3D9Mesh::GetVertexBuffer() const
{
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DIndexBuffer9* 
nD3D9Mesh::GetIndexBuffer() const
{
    return this->indexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DVertexDeclaration9*
nD3D9Mesh::GetVertexDeclaration() const
{
    return this->vertexDeclaration;
}

//------------------------------------------------------------------------------
#endif

