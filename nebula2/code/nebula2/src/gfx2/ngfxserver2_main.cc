#define N_IMPLEMENTS nGfxServer2
//------------------------------------------------------------------------------
//  ngfxserver2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/ngfxserver2.h"
#include "resource/nresourceserver.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"

nNebulaScriptClass(nGfxServer2, "nroot");

//------------------------------------------------------------------------------
/**
*/
nGfxServer2::nGfxServer2() :
    displayOpen(false),
    inBeginScene(false),
    refResource(kernelServer),
    vertexRangeFirst(0),
    vertexRangeNum(0),
    indexRangeFirst(0),
    indexRangeNum(0),
    windowTitle("nGfxServer2 window")
{
    this->refResource = "/sys/servers/resource";
    int i;
    for (i = 0; i < NUM_TRANSFORMTYPES; i++)
    {
        this->transformTopOfStack[i] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
nGfxServer2::~nGfxServer2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create a new static mesh object.

    @param  rsrcName    a resource name for resource sharing
    @return             a new nMesh2 object
*/
nMesh2*
nGfxServer2::NewMesh(const char* rsrcName)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new shared texture object.

    @param  rsrcName    a resource name for resource sharing
    @return             a new nTexture2 object
*/
nTexture2*
nGfxServer2::NewTexture(const char* rsrcName)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new shared shader object

    @param  rsrcName    a resource name for resource sharing
    @return             a new nShader2 object
*/
nShader2*
nGfxServer2::NewShader(const char* rsrcName)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new render target object.

    @param  rsrcName    a resource name for resource sharing
    @param  width       width of render target
    @param  height      height of render target
    @param  format      pixel format of render target
    @param  hasColor    true if render target has a color buffer
    @param  hasDepth    true if render target has a depth buffer
    @param  hasStencil  true if render target has a stencil buffer
*/
nTexture2*
nGfxServer2::NewRenderTarget(const char* rsrcName,
                             int width,
                             int height,
                             nTexture2::Format format,
                             bool hasColor,
                             bool hasDepth,
                             bool hasStencil)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Set the prefered display mode. This must be called outside
    OpenDisplay()/CloseDisplay().
*/
void
nGfxServer2::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
    Set the current window title.
*/
void
nGfxServer2::SetWindowTitle(const char* title)
{
    n_assert(title);
    this->windowTitle = title;
}

//------------------------------------------------------------------------------
/**
    Set the current camera. Subclasses should adjust their projection matrix
    accordingly when this method is called.

    @param  camera      a camera object with valid parameters
*/
void
nGfxServer2::SetCamera(const nCamera2& camera)
{
    this->camera = camera;
}

//------------------------------------------------------------------------------
/**
    Open the display.

    @return     true if display successfully opened
*/
bool
nGfxServer2::OpenDisplay()
{
    n_assert(!this->displayOpen);
    this->displayOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the display.
*/
void
nGfxServer2::CloseDisplay()
{
    n_assert(this->displayOpen);
    this->displayOpen = false;
}

//------------------------------------------------------------------------------
/**
    Trigger the windows system message pump. Return false if application
    should be closed on request of the windows system.

    @return     false if window system requests application to quit
*/
bool
nGfxServer2::Trigger()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Begin rendering to the current render target.

    @return     false on error, do not call EndScene() or Present() in this case
*/
bool
nGfxServer2::BeginScene()
{
    n_assert(!this->inBeginScene);
    if (this->displayOpen)
    {
        this->inBeginScene = true;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Finish rendering to the current render target.

    @return     false on error, do not call Present() in this case.
*/
void
nGfxServer2::EndScene()
{
    n_assert(this->inBeginScene);
    this->inBeginScene = false;
}

//------------------------------------------------------------------------------
/**
    Present the contents of the back buffer. This must be called
    outside BeginScene()/EndScene().
*/
void
nGfxServer2::PresentScene()
{
    n_assert(!this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
    Clear buffers.
*/
void
nGfxServer2::Clear(int bufferTypes, float red, float green, float blue, float alpha, float z, int stencil)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Set the current render target. This method must be called outside
    BeginScene()/EndScene(). The method will increment the refcount
    of the render target object and decrement the refcount of the
    previous render target.

    @param  t   the new render target, or 0 to render to the frame buffer
*/
void
nGfxServer2::SetRenderTarget(nTexture2* t)
{
    n_assert(!this->inBeginScene);
    if (this->refRenderTarget.isvalid())
    {
        this->refRenderTarget->Release();
        this->refRenderTarget.invalidate();
    }
    if (t)
    {
        this->refRenderTarget = t;
        t->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
    Set the current mesh object for rendering.
    This will increment its refcount and decrement the refcount 
    of the previous object.

    @param  stream      vertex stream index 
    @param  mesh        pointer to a nMesh2 object
    @param  groupIndex  group index of vertex/index group to render in mesh
*/
void
nGfxServer2::SetMesh(int stream, nMesh2* mesh)
{
    n_assert((stream >= 0) && (stream < MAX_VERTEXSTREAMS));

    if (this->refMeshes[stream].isvalid())
    {
        this->refMeshes[stream]->Release();
        this->refMeshes[stream].invalidate();
    }
    if (mesh)
    {
        this->refMeshes[stream] = mesh;
        mesh->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
    Set the current texture object for rendering.
    This will increment its refcount and decrement the refcount 
    of the previous object.

    @param  stage       texture stage index
    @param  texture     pointer to a nTexture2 object
*/
void
nGfxServer2::SetTexture(int stage, nTexture2* texture)
{
    n_assert((stage >= 0) && (stage < MAX_TEXTURESTAGES));

    if (this->refTextures[stage].isvalid())
    {
        this->refTextures[stage]->Release();
        this->refTextures[stage].invalidate();
    }
    if (texture)
    {
        this->refTextures[stage] = texture;
        texture->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
    Set the current shader object for rendering.
    This will increment its refcount and decrement the refcount 
    of the previous object.

    @param  shader     pointer to a nShader2 object
*/
void
nGfxServer2::SetShader(nShader2* shd)
{
    if (this->refShader.isvalid())
    {
        this->refShader->Release();
        this->refShader.invalidate();
    }
    if (shd)
    {
        this->refShader = shd;
        shd->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
    Set transformation matrix.

    @param  type        transform type
    @param  matrix      the 4x4 matrix
*/
void
nGfxServer2::SetTransform(nTransformType type, const matrix44& matrix)
{
    n_assert(type < NUM_TRANSFORMTYPES);
    this->transform[type] = matrix;
    switch (type)
    {
        case MODELVIEW:
            this->transform[INVMODELVIEW] = matrix;
            this->transform[INVMODELVIEW].invert_simple();
            this->transform[MODEL] = this->transform[MODELVIEW] * this->transform[INVVIEW];
            break;

        case VIEW:
            this->transform[INVVIEW] = matrix;
            this->transform[INVVIEW].invert_simple();
            this->transform[MODEL] = this->transform[MODELVIEW] * this->transform[INVVIEW];
            break;
    }

    // update the modelview/projection matrix
    this->transform[MODELVIEWPROJECTION] = this->transform[MODELVIEW] * this->transform[PROJECTION];
}

//------------------------------------------------------------------------------
/**
    Push current transformation on stack and set new matrix.

    @param  type        transform type
    @param  matrix      the 4x4 matrix
*/
void
nGfxServer2::PushTransform(nTransformType type, const matrix44& matrix)
{
    n_assert(type < NUM_TRANSFORMTYPES);
    n_assert(this->transformTopOfStack[type] < MAX_TRANSFORMSTACKDEPTH);
    this->transformStack[type][this->transformTopOfStack[type]++] = this->transform[type];
    this->SetTransform(type, matrix);
}

//------------------------------------------------------------------------------
/**
    Pop transformation from stack and make it the current transform.
*/
const matrix44&
nGfxServer2::PopTransform(nTransformType type)
{
    n_assert(type < NUM_TRANSFORMTYPES);
    this->SetTransform(type, this->transformStack[type][--this->transformTopOfStack[type]]);
    return this->transform[type];
}

//------------------------------------------------------------------------------
/**
    Draw current mesh, texture and shader.
*/
void
nGfxServer2::Draw()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Claim ownership of dynamic mesh. Note that there is only one dynamic
    mesh for the whole system. Clients which need to render dynamic geometry
    should use the nDynamicMesh utility class which encapsulates and
    simplifies rendering through the global dynamic mesh. Ownership of the
    global dynamic mesh is mutually exclusive.

    This method should be overwritten by derived classes.

    @return     a pointer to the global dynamic mesh
*/
nMesh2*
nGfxServer2::LockDynamicMesh()
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Give up ownership of the dynamic mesh.
    This method should be overwritten by derived classes.

    @param  mesh    pointer to the global dynamic mesh as aquired by LockDynamicMesh()
*/
void
nGfxServer2::UnlockDynamicMesh(nMesh2* mesh)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Add text to the text buffer.
*/
void
nGfxServer2::Text(const char* /*text*/, float /*x*/, float /*y*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render the text in the text buffer.
*/
void
nGfxServer2::DrawTextBuffer()
{
    // empty
}

//------------------------------------------------------------------------------

