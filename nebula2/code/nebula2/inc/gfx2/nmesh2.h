#ifndef N_MESH2_H
#define N_MESH2_H
//------------------------------------------------------------------------------
/**
    Internally holds opaque vertex and index data to feed a vertex shader.
    Vertices in a mesh are simply an array of floats. 
    Meshes are generally static and loaded from mesh resource files.
    
    nMesh2 is normally a superclass for Gfx API specific derived classes, like
    Direct3D or OpenGL.

    Mesh Fileformats:

    --------------------------------------------------------
    ASCII: n3d2

    type n3d2
    numgroups [numGroups]
    numvertices [numVertices] 
    vertexwidth [vertexWidth]
    numtris [numTriangles]
    vertexcomps [coord normal tangent binormal color uv0 uv1 uv2 uv3 weights jindices]
    g [firstVertex] [numVertices] [firstTriangle] [numTriangles]
    ...
    v 0.0 0.0 0.0 ....
    v 0.0 0.0 0.0 ....
    ...
    t 0 1 2
    t 1 2 3
    ....
    
    --------------------------------------------------------
    BINARY: nvx2
    
    uint magic = 'NVX2'
    int numGroups;
    int numVertices;
    int vertexWidth;
    int numTriangles;
    int vertexComponents:   one bit set for each vertex component
        Coord    = (1<<0)
        Normal   = (1<<1)
        Tangent  = (1<<2)
        Binormal = (1<<3)
        Color    = (1<<4)
        Uv0      = (1<<5)
        Uv1      = (1<<6)
        Uv2      = (1<<7)
        Uv3      = (1<<8)
        ...

    for each group...
        int vertexRangeFirst
        int vertexRangeNum
        int firstTriangle
        int numTriangles
    end

    float[] vertices;
    ushort[] indices;

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_RESOURCE_H
#include "resource/nresource.h"
#endif

#ifndef N_PRIMITIVETYPES_H
#include "gfx2/nprimitivetypes.h"
#endif

#ifndef N_MESHGROUP_H
#include "gfx2/nmeshgroup.h"
#endif

#undef N_DEFINES
#define N_DEFINES nMesh2
#include "kernel/ndefdllclass.h"

class nGfxServer2;
class nVariableServer;

//------------------------------------------------------------------------------
class N_PUBLIC nMesh2 : public nResource
{
public:
    enum Usage
    {
        WriteOnce,      // (default) CPU only fills the vertex buffer once, and never touches it again
        ReadOnly,       // CPU reads from the vertex buffer, which can never be rendered
        WriteOnly,      // CPU writes frequently to vertex buffer, but never read data back
    };

    enum VertexComponent
    {
        Coord    = (1<<0),
        Normal   = (1<<1),
        Tangent  = (1<<2),
        Binormal = (1<<3),
        Color    = (1<<4),
        Uv0      = (1<<5),
        Uv1      = (1<<6),
        Uv2      = (1<<7),
        Uv3      = (1<<8),
        Weights  = (1<<9),
        JIndices = (1<<10),
    };

    /// constructor
    nMesh2();
    /// destructor
    virtual ~nMesh2();
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
    /// render the mesh in wireframe mode (as debug visualization)
    virtual void RenderWireframe(nGfxServer2* gfxServer, nVariableServer* varServer, const char* shaderName, int groupIndex);

    /// set the mesh use type
    void SetUsage(Usage t);
    /// get the mesh use type
    Usage GetUsage() const;
    /// set number of vertices
    void SetNumVertices(int num);
    /// get number of vertices in mesh
    int GetNumVertices() const;
    /// set number of indices
    void SetNumIndices(int num);
    /// get num indices in mesh
    int GetNumIndices() const;
    /// set vertex components 
    void SetVertexComponents(int compMask);
    /// get vertex components
    int GetVertexComponents() const;
    /// get vertex width (number of floats in one vertex)
    int GetVertexWidth() const;
    /// set the primitive type
    void SetPrimitiveType(nPrimitiveType t);
    /// get the primitive type
    nPrimitiveType GetPrimitiveType() const;
    /// set number of groups
    void SetNumGroups(int num);
    /// get number of groups
    int GetNumGroups() const;
    /// get group by index
    nMeshGroup& GetGroup(int index) const;
    /// returns the byte size of the embedded vertex buffer
    int GetVertexBufferByteSize() const;
    /// returns the byte size of the embedded index buffer
    int GetIndexBufferByteSize() const;

    static nKernelServer* kernelServer;

protected:
    /// set the byte size of the vertex buffer
    void SetVertexBufferByteSize(int s);
    /// set the byte size of the index buffer
    void SetIndexBufferByteSize(int s);

    Usage usage;
    int vertexComponentMask;
    int vertexWidth;                // depends on vertexComponentMask
    int numVertices;
    int numIndices;
    int numGroups;
    nPrimitiveType primType;
    nMeshGroup* groups;
    int vertexBufferByteSize;
    int indexBufferByteSize;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMesh2::SetUsage(Usage t)
{
    this->usage = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMesh2::Usage
nMesh2::GetUsage() const
{
    return this->usage;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMesh2::SetNumVertices(int num)
{
    n_assert(num > 0);
    this->numVertices = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMesh2::GetNumVertices() const
{
    return this->numVertices;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMesh2::SetNumGroups(int num)
{
    n_assert(num > 0);
    n_assert(0 == this->groups);
    this->numGroups = num;
    this->groups = new nMeshGroup[num];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMesh2::GetNumGroups() const
{
    return this->numGroups;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMeshGroup&
nMesh2::GetGroup(int i) const
{
    n_assert((i >= 0) && (i < this->numGroups));
    return this->groups[i];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMesh2::SetVertexComponents(int compMask)
{
    this->vertexComponentMask = compMask;
    this->vertexWidth = 0;
    if (compMask & Coord)    this->vertexWidth += 3;
    if (compMask & Normal)   this->vertexWidth += 3;
    if (compMask & Tangent)  this->vertexWidth += 3;
    if (compMask & Binormal) this->vertexWidth += 3;
    if (compMask & Color)    this->vertexWidth += 4;
    if (compMask & Uv0)      this->vertexWidth += 2;
    if (compMask & Uv1)      this->vertexWidth += 2;
    if (compMask & Uv2)      this->vertexWidth += 2;
    if (compMask & Uv3)      this->vertexWidth += 2;
    if (compMask & Weights)  this->vertexWidth += 4;
    if (compMask & JIndices) this->vertexWidth += 4;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMesh2::GetVertexComponents() const
{
    return this->vertexComponentMask;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMesh2::GetVertexWidth() const
{
    return this->vertexWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMesh2::SetPrimitiveType(nPrimitiveType t)
{
    this->primType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nPrimitiveType
nMesh2::GetPrimitiveType() const
{
    return this->primType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMesh2::SetNumIndices(int num)
{
    n_assert(num > 0);
    this->numIndices = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMesh2::GetNumIndices() const
{
    return this->numIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMesh2::SetVertexBufferByteSize(int s)
{
    n_assert(s > 0);
    this->vertexBufferByteSize = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMesh2::SetIndexBufferByteSize(int s)
{
    n_assert(s > 0);
    this->indexBufferByteSize = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMesh2::GetVertexBufferByteSize() const
{
    return this->vertexBufferByteSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMesh2::GetIndexBufferByteSize() const
{
    return this->indexBufferByteSize;
}

//------------------------------------------------------------------------------
#endif

