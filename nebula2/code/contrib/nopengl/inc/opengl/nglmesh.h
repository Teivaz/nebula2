#ifndef N_GLMESH_H
#define N_GLMESH_H
//------------------------------------------------------------------------------
/**
    @class nGLMesh
    @ingroup OpenGL

    nMesh2 subclass for OpenGL.

    2004 Haron
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
    /// get an estimated byte size of the resource data (for memory statistics)
    virtual int GetByteSize();

protected:
    /// load mesh resource
    virtual bool LoadResource();
    /// unload mesh resource
    virtual void UnloadResource();

    /// create the gl vertex buffer
    virtual void CreateVertexBuffer();
    /// create the gl index buffer
    virtual void CreateIndexBuffer();

private:
    friend class nGLServer2;
    friend class nGLMeshArray;

    /// create the vertex declaration
    void CreateVertexDeclaration();
    ///
    bool BeginRender(int vertexStart, bool useIndex = false, int indexStart = 0);
    ///
    void EndRender(bool useIndex = false);

    //void* indexBufferOffset(int offset);
    //void* vertexBufferOffset(int offset);

    void* VertexOffset(int vertexStart, int component);
    void* IndexOffset(int indexStart);

    bool VBMapFlag;
    bool IBMapFlag;
    uint vertexBuffer;
    uint indexBuffer;
    void*  privVertexBuffer;
    void* privIndexBuffer;

    ushort texCoordNum;
    int texCoordFirst;
    ushort *componentOffset;
    static const ushort componentSize[];
};

inline
GLsizei
IndexStride()
{
    return sizeof(GLshort); //??? depend of nMeshLoader::IndexType
}

//------------------------------------------------------------------------------
#endif
