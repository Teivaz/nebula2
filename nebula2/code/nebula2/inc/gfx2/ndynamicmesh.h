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
    bool Initialize(nGfxServer2::PrimitiveType primType, int vertexComponents, int usageFlags, bool indexedRendering, bool shared = true);
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

protected:
    enum
    {	
        VertexBufferSize = 16384,                   // number of vertices
        IndexBufferSize  = 3 * VertexBufferSize,    // number of indices
    };
    bool indexedRendering;
    nRef<nMesh2> refMesh;
    nGfxServer2::PrimitiveType primitiveType;
};

//------------------------------------------------------------------------------
#endif
