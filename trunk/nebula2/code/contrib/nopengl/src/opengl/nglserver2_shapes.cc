//------------------------------------------------------------------------------
//  nglserver2_shapes.cc
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"

//------------------------------------------------------------------------------
/**
    Begin rendering shape primitives. Shape primitives are handy for
    quickly rendering debug visualizations.
*/
void
nGLServer2::BeginShapes()
{
    nGfxServer2::BeginShapes();
    // TODO: Implement
}

//------------------------------------------------------------------------------
/**
    Render a standard shape using the provided model matrix and color.
*/
void
nGLServer2::DrawShape(ShapeType type, const matrix44& model, const vector4& color)
{
    n_assert(this->inBeginShapes);
    // TODO: Implement
}

//------------------------------------------------------------------------------
/**
    Render a standard shape using the provided model matrix without
    any shader management.
*/
void
nGLServer2::DrawShapeNS(ShapeType type, const matrix44& model)
{
    // TODO: Implement
}

//------------------------------------------------------------------------------
/**
    Draw non-indexed primitives. This is slow, so it should only be used for 
    debug visualizations. Vertex width is number of float's!
*/
void 
nGLServer2::DrawShapePrimitives(PrimitiveType type, int numPrimitives, const vector3* vertexList, int vertexWidth, const matrix44& model, const vector4& color)
{
    // TODO: Implement
}

//------------------------------------------------------------------------------
/**
    Directly Draw indexed primitives.  This is slow, so it should only be used for 
    debug visualizations. Vertex width is number of float's!
*/
void
nGLServer2::DrawShapeIndexedPrimitives(PrimitiveType type,
                                        int numPrimitives,
                                        const vector3* vertices,
                                        int numVertices,
                                        int vertexWidth,
                                        void* indices,
                                        IndexType indexType,
                                        const matrix44& model,
                                        const vector4& color)
{
    // TODO: Implement
}

//------------------------------------------------------------------------------
/**
    Finish rendering shapes.
*/
void
nGLServer2::EndShapes()
{
    nGfxServer2::EndShapes();
    // TODO: Implement

    // clear current mesh, otherwise Nebula2's redundancy mechanism may be fooled!
    this->SetMesh(0, 0);
}
