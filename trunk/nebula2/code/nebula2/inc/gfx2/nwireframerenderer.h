#ifndef N_WIREFRAMERENDERER_H
#define N_WIREFRAMERENDERER_H
//------------------------------------------------------------------------------
/**
    Renders a source mesh as wireframe through the global dynamic mesh.
    Requires the system shader file "wireframe.fx" to work.
    
    (C) 2003 RadonLabs GmbH
*/
#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_VARIABLE_H
#include "variable/nvariable.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_DYNAMICMESH_H
#include "gfx2/ndynamicmesh.h"
#endif

class nGfxServer2;
class nVariableServer;
class nShader2;

//------------------------------------------------------------------------------
class nWireframeRenderer
{
public:
    enum Component
    {
        Coord,
        Normal,
        Tangent,
        Binormal,

        NumComponents,
    };

    /// constructor
    nWireframeRenderer();
    /// constructor with initialization
    nWireframeRenderer(nGfxServer2* gfxServer, nVariableServer* varServer, const char* shaderFilename);
    /// destructor
    ~nWireframeRenderer();
    /// check if renderer is in a valid state (if returns false, call Initialize())
    bool IsValid() const;
    /// initialize the renderer if in invalid state
    void Initialize(nGfxServer2* gfxServer, nVariableServer* varServer, const char* shaderFilename);
    /// begin rendering
    void Begin(float r, float g, float b, float a);
    /// render a line
    void Line(const vector3& v0, const vector3& v1);
    /// finish rendering
    void End();

private:
    nRef<nShader2> refShader;
    nRef<nGfxServer2> refGfxServer;
    nVariable::Handle lineColorHandle;
    nDynamicMesh dynMesh;
    float4 color;
    bool inBegin;
    float* vertexPointer;
    ushort* indexPointer;
    int maxVertices;
    int maxIndices;
    int curVertex;
    int curIndex;
};

//------------------------------------------------------------------------------
#endif