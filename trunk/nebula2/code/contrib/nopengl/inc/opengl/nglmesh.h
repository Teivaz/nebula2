#ifndef N_GLMESH_H
#define N_GLMESH_H
//------------------------------------------------------------------------------
/**
    @class nGLMesh
    @ingroup OpenGL

    nMesh2 subclass for OpenGL.

    Haron
*/
#include "gfx2/nmesh2.h"
#include "opengl/nglserver2.h"

//------------------------------------------------------------------------------
class nGLMesh : public nMesh2
{
public:
    /// constructor
    nGLMesh();
    /// destructor
    virtual ~nGLMesh();
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

protected:
    /// load mesh resource
    virtual bool LoadResource();
    /// unload mesh resource
    virtual void UnloadResource();

private:
    friend class nGLServer2;

    /// create the gl vertex buffer
    void CreateVertexBuffer();
    /// create the gl index buffer
    void CreateIndexBuffer();
    /// load n3d2 file
    bool LoadN3d2File();
    /// load nvx2 file
    bool LoadNvx2File();
    /// update the group bounding boxes (slow!)
    void UpdateGroupBoundingBoxes();

    bool BeginRender(int vertexStart, bool useIndex = false, int indexStart = 0);
    void EndRender(bool useIndex = false);

    //void* indexBufferOffset(int offset);
    //void* vertexBufferOffset(int offset);

    //bool Begin(int vertexStart, bool useIndex, int indexStart);
    //void End();

    void* vertexOffset(int vertexStart, int component);
    void* indexOffset(int indexStart);
    void CreateVertexDeclaration();

    nAutoRef<nGLServer2> refGfxServer;
    bool VBMapFlag;
    bool IBMapFlag;
    uint vertexBuffer;
    uint indexBuffer;
    float*  privVertexBuffer;
    ushort* privIndexBuffer;

public:
    enum VertexComponentIndex
    {
        CoordIndex    = 0,
        NormalIndex   = 1,
        Uv0Index      = 2,
        Uv1Index      = 3,
        Uv2Index      = 4,
        Uv3Index      = 5,
        ColorIndex    = 6,
        TangentIndex  = 7,
        BinormalIndex = 8,
        WeightsIndex  = 9,
        JIndicesIndex = 10,

        ComponentsNum
    };
private:

    ushort texCoordNum;
    int texCoordFirst;
    ushort componentOffset[nGLMesh::ComponentsNum];
    static const ushort componentSize[nGLMesh::ComponentsNum];
};

inline
GLsizei
indexStride()
{
    return sizeof(GLshort); //??? depend of nMeshLoader::IndexType
}

//------------------------------------------------------------------------------
#endif
