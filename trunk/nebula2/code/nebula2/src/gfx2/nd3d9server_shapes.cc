//------------------------------------------------------------------------------
//  nd3d9server_shapes.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9shader.h"

//------------------------------------------------------------------------------
/**
    Begin rendering shape primitives. Shape primitives are handy for
    quickly rendering debug visualizations.
*/
void
nD3D9Server::BeginShapes()
{
    nGfxServer2::BeginShapes();

    // render shader with state backup
    nShader2* shd = this->refShapeShader;
    int numPasses = shd->Begin(true);
    n_assert(1 == numPasses);
    shd->Pass(0);
}

//------------------------------------------------------------------------------
/**
    Render a standard shape using the provided model matrix and color.
*/
void
nD3D9Server::DrawShape(ShapeType type, const matrix44& model, const vector4& color)
{
    n_assert(0 != this->shapeMeshes[type]);
    n_assert(this->inBeginShapes);

    // update color in shader
    nShader2* shd = this->refShapeShader;
    if (shd->IsParameterUsed(nShaderState::MatDiffuse))
    {
        shd->SetVector4(nShaderState::MatDiffuse, color);
    }

    // update model matrix
    this->PushTransform(nGfxServer2::Model, model);
    HRESULT hr = this->shapeMeshes[type]->DrawSubset(0);
    n_dxtrace(hr, "DrawSubset() failed in nD3D9Server::DrawShape()");
    this->PopTransform(nGfxServer2::Model);
}

//------------------------------------------------------------------------------
/**
    Finish rendering shapes.
*/
void
nD3D9Server::EndShapes()
{
    nGfxServer2::EndShapes();
    this->refShapeShader->End();
}
