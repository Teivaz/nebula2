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
    // XXX: Implement
}

//------------------------------------------------------------------------------
/**
    Render a standard shape using the provided model matrix and color.
*/
void
nGLServer2::DrawShape(ShapeType type, const matrix44& model, const vector4& color)
{
    n_assert(this->inBeginShapes);
    // XXX: Implement
}

//------------------------------------------------------------------------------
/**
    Finish rendering shapes.
*/
void
nGLServer2::EndShapes()
{
    nGfxServer2::EndShapes();
    // XXX: Implement
}
