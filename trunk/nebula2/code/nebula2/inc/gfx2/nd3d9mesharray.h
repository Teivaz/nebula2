#ifndef N_D3D9MESHARRAY_H
#define N_D3D9MESHARRAY_H
//------------------------------------------------------------------------------
/**
    @class nD3D9MeshArray
    @ingroup NebulaGraphicsSystem

    Holds an array of up to nGfxServer2::MaxVertexStreams meshes.
    Can be posted to the nD3D9GfxServer to assign all streams to the graphics
    device, with the correct vertex declaration based on the vertex components
    of the meshes.
    
    Because of limits in the D3D API the different streams must be different meshes. 
    
    (C) 2004 RadonLabs GmbH
*/
#include "resource/nresource.h"
#include "gfx2/nmesharray.h"
#include "gfx2/nd3d9mesh.h"

class nGfxServer2;
class nVariableServer;
class nD3D9Server;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;
struct IDirect3DVertexDeclaration9;

//------------------------------------------------------------------------------
class nD3D9MeshArray : public nMeshArray
{
public:
    /// constructor
    nD3D9MeshArray();
    /// destructor
    virtual ~nD3D9MeshArray();

    /// set absolute path to resource file for index
    virtual void SetFilenameAt(int index, const nString& path);
    /// set the mesh object at index
    virtual void SetMeshAt(int index, nMesh2* mesh);

    /// get d3d vertex buffer for a mesh
    IDirect3DVertexBuffer9* GetVertexBuffer(int meshIndex) const;
    /// get d3d index buffer of first mesh
    IDirect3DIndexBuffer9* GetIndexBuffer() const;
    /// get the combined vertex declaration for the current meshes
    IDirect3DVertexDeclaration9* GetVertexDeclaration();

protected:
    /// override in subclasse to perform actual resource loading
    virtual bool LoadResource();
    /// override in subclass to perform actual resource unloading
    virtual void UnloadResource();
    /// create the vertex declaration
    void CreateVertexDeclaration();

    bool vertexDeclarationDirty;
    IDirect3DVertexDeclaration9* vertexDeclaration;
};

//------------------------------------------------------------------------------
/**
    Get Vertex buffer of a mesh.
*/
inline
IDirect3DVertexBuffer9*
nD3D9MeshArray::GetVertexBuffer(int index) const
{
    nMesh2* mesh = this->GetMeshAt(index);
    if (0 != mesh)
    {
        return ((nD3D9Mesh*) mesh)->GetVertexBuffer();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Get Index buffer of the first mesh.
*/
inline
IDirect3DIndexBuffer9*
nD3D9MeshArray::GetIndexBuffer() const
{
    nMesh2* mesh = this->GetMeshAt(0);
    if (0 != mesh)
    {
        return ((nD3D9Mesh*) mesh)->GetIndexBuffer();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DVertexDeclaration9*
nD3D9MeshArray::GetVertexDeclaration()
{
    if (this->vertexDeclarationDirty)
    {
        this->CreateVertexDeclaration();
    }
    return this->vertexDeclaration;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nD3D9MeshArray::LoadResource()
{
    bool success = nMeshArray::LoadResource();
    
    if (success && this->vertexDeclarationDirty)
    {
        this->CreateVertexDeclaration();
    }
    
    return success;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nD3D9MeshArray::UnloadResource()
{
    this->vertexDeclarationDirty = true;
    nMeshArray::UnloadResource();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nD3D9MeshArray::SetFilenameAt(int index, const nString &path)
{
    this->vertexDeclarationDirty = true;
    nMeshArray::SetFilenameAt(index, path);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nD3D9MeshArray::SetMeshAt(int index, nMesh2* mesh)
{
    this->vertexDeclarationDirty = true;
    nMeshArray::SetMeshAt(index, mesh);
}
//------------------------------------------------------------------------------
#endif
