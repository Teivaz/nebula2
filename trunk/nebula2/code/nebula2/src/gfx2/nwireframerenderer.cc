//------------------------------------------------------------------------------
//  nwireframerenderer.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nwireframerenderer.h"
#include "gfx2/ndynamicmesh.h"
#include "gfx2/nshader2.h"
#include "gfx2/ngfxserver2.h"
#include "variable/nvariableserver.h"

//------------------------------------------------------------------------------
/**
*/
nWireframeRenderer::nWireframeRenderer() :
    lineColorHandle(nVariable::INVALID_HANDLE),
    inBegin(false),
    vertexPointer(0),
    indexPointer(0),
    maxVertices(0),
    maxIndices(0),
    curVertex(0),
    curIndex(0)
{
    this->color[0] = 1.0f;
    this->color[1] = 1.0f;
    this->color[2] = 1.0f;
    this->color[3] = 1.0f;
}

//------------------------------------------------------------------------------
/**
*/
nWireframeRenderer::nWireframeRenderer(nGfxServer2* gfxServer, nVariableServer* varServer, const char* shaderFilename) :
    lineColorHandle(nVariable::INVALID_HANDLE),
    inBegin(false),
    vertexPointer(0),
    indexPointer(0),
    maxVertices(0),
    maxIndices(0),
    curVertex(0),
    curIndex(0)
{
    this->Initialize(gfxServer, varServer, shaderFilename);
}

//------------------------------------------------------------------------------
/**
*/
nWireframeRenderer::~nWireframeRenderer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nWireframeRenderer::IsValid() const
{
    return this->refShader.isvalid() && this->refShader.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
void
nWireframeRenderer::Initialize(nGfxServer2* gfxServer, nVariableServer* varServer, const char* shaderFilename)
{
    n_assert(gfxServer);
    n_assert(varServer);
    n_assert(shaderFilename);

    this->refGfxServer = gfxServer;
    this->refShader = gfxServer->NewShader(shaderFilename);
    if (!this->refShader->IsValid())
    {
        this->refShader->SetFilename(shaderFilename);
        bool wireframeShaderLoaded = this->refShader->Load();
        n_assert(wireframeShaderLoaded);
    }
    this->lineColorHandle = varServer->GetVariableHandleByName("lineColor");
    n_assert(nVariable::INVALID_HANDLE != this->lineColorHandle);
}

//------------------------------------------------------------------------------
/**
*/
void
nWireframeRenderer::Begin(float r, float g, float b, float a)
{
    n_assert(this->refShader.isvalid());
    n_assert(this->refGfxServer.isvalid());
    n_assert(nVariable::INVALID_HANDLE != this->lineColorHandle);
    n_assert(!this->inBegin);

    // initialize rendering through dynamic mesh
    this->dynMesh.Begin(this->refGfxServer.get(),   // in: gfx server pointer
                        LINELIST,                   // in: primitive type
                        nMesh2::Coord,              // in: vertex width
                        this->vertexPointer,        // out: pointer to vertex array
                        this->indexPointer,         // out: pointer to index array
                        this->maxVertices,          // out: max number of vertices
                        this->maxIndices);          // out: max number of indices

    // initialize wireframe shader and set as current shader
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
    this->color[3] = a;
    this->refShader->SetVector(this->lineColorHandle, this->color);
    this->refGfxServer->SetShader(this->refShader.get());

    this->inBegin = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nWireframeRenderer::Line(const vector3& v0, const vector3& v1)
{
    n_assert(this->inBegin);

    // see if we need to call Swap() on the dynamic mesh
    if (((this->curVertex + 2) >= this->maxVertices) ||
        ((this->curIndex + 2) >= this->maxIndices))
    {
        this->refShader->SetVector(this->lineColorHandle, this->color);

        this->dynMesh.Swap(this->curVertex, this->curIndex, this->vertexPointer, this->indexPointer);
        this->curVertex = 0;
        this->curIndex = 0;
    }

    // write new vertices/indices
    float* vp = this->vertexPointer + (3 * this->curVertex);
    *vp++ = v0.x;  *vp++ = v0.y;  *vp++ = v0.z;
    *vp++ = v1.x;  *vp++ = v1.y;  *vp++ = v1.z;
    this->indexPointer[this->curIndex++] = this->curVertex++;
    this->indexPointer[this->curIndex++] = this->curVertex++;
}

//------------------------------------------------------------------------------
/**
*/
void
nWireframeRenderer::End()
{
    n_assert(this->inBegin);
    this->refShader->SetVector(this->lineColorHandle, this->color);
    this->dynMesh.End(this->curVertex, this->curIndex);
    this->inBegin = false;
}


