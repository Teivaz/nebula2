//------------------------------------------------------------------------------
//  ngfxserver2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/ngfxserver2.h"
#include "resource/nresourceserver.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nfont2.h"

nNebulaScriptClass(nGfxServer2, "nroot");

//------------------------------------------------------------------------------
/**
*/
nGfxServer2::nGfxServer2() :
    displayOpen(false),
    inBeginScene(false),
    refResource("/sys/servers/resource"),
    vertexRangeFirst(0),
    vertexRangeNum(0),
    indexRangeFirst(0),
    indexRangeNum(0),

    cursorVisibility(System),
    cursorDirty(true)
{
    int i;
    for (i = 0; i < NumTransformTypes; i++)
    {
        this->transformTopOfStack[i] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
nGfxServer2::~nGfxServer2()
{
    //empty
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
    Create a new shared font object.

    @param  rsrcName    a resource name for resource sharing
    @param  fontDesc    a valid font description object
    @return             a nFont2 object
*/
nFont2*
nGfxServer2::NewFont(const char* rsrcName, const nFontDesc& fontDesc)
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
    @param  usageFlags  a combination of nTexture2::Usage flags (RenderTargetXXX only)
*/
nTexture2*
nGfxServer2::NewRenderTarget(const char* rsrcName,
                             int width,
                             int height,
                             nTexture2::Format format,
                             int usageFlags)
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
    Set the current camera. Subclasses should adjust their projection matrix
    accordingly when this method is called.

    @param  camera      a camera object with valid parameters
*/
void
nGfxServer2::SetCamera(nCamera2& camera)
{
    this->camera = camera;
}

//------------------------------------------------------------------------------
/**
    Set the current viewport. Subclasses should adjust the device accordingly.
*/
void
nGfxServer2::SetViewport(nViewport& vp)
{
    this->viewport = vp;
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

    @param  shd     pointer to a nShader2 object
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
    Set the current font object for rendering.
    This will increment its refcount and decrement the refcount 
    of the previous object.

    @param  font        pointer to a nFont2 object
*/
void
nGfxServer2::SetFont(nFont2* font)
{
    if (this->refFont.isvalid())
    {
        this->refFont->Release();
        this->refFont.invalidate();
    }
    if (font)
    {
        this->refFont = font;
        font->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
    Set the current mouse cursor.

    @param  cursor      a valid mouse cursor object
*/
void
nGfxServer2::SetMouseCursor(const nMouseCursor& cursor)
{
    this->curMouseCursor = cursor;
    this->cursorDirty = true;
}

//------------------------------------------------------------------------------
/**
    Get the current mouse cursor.
*/
const nMouseCursor&
nGfxServer2::GetMouseCursor() const
{
    return this->curMouseCursor;
}

//------------------------------------------------------------------------------
/**
    Set transformation matrix.

    @param  type        transform type
    @param  matrix      the 4x4 matrix
*/
void
nGfxServer2::SetTransform(TransformType type, const matrix44& matrix)
{
    n_assert(type < NumTransformTypes);
    bool updModelView = false;
    switch (type)
    {
        case Model:
            this->transform[Model] = matrix;
            this->transform[InvModel] = matrix;
            this->transform[InvModel].invert_simple();
            updModelView = true;
            break;

        case View:
            this->transform[View] = matrix;
            this->transform[InvView] = matrix;
            this->transform[InvView].invert_simple();
            updModelView = true;
            break;

        case Projection:
            this->transform[Projection] = matrix;
            break;

        default:
            n_error("nGfxServer2::SetTransform() Trying to set read-only transform type!");
            break;
    }

    if (updModelView)
    {
        this->transform[ModelView]    = this->transform[Model] * this->transform[View];
        this->transform[InvModelView] = this->transform[ModelView];
        this->transform[InvModelView].invert_simple();
    }

    // update the modelview/projection matrix
    this->transform[ModelViewProjection] = this->transform[ModelView] * this->transform[Projection];
}

//------------------------------------------------------------------------------
/**
    Push current transformation on stack and set new matrix.

    @param  type        transform type
    @param  matrix      the 4x4 matrix
*/
void
nGfxServer2::PushTransform(TransformType type, const matrix44& matrix)
{
    n_assert(type < NumTransformTypes);
    n_assert(this->transformTopOfStack[type] < MAX_TRANSFORMSTACKDEPTH);
    this->transformStack[type][this->transformTopOfStack[type]++] = this->transform[type];
    this->SetTransform(type, matrix);
}

//------------------------------------------------------------------------------
/**
    Pop transformation from stack and make it the current transform.
*/
const matrix44&
nGfxServer2::PopTransform(TransformType type)
{
    n_assert(type < NumTransformTypes);
    this->SetTransform(type, this->transformStack[type][--this->transformTopOfStack[type]]);
    return this->transform[type];
}

//------------------------------------------------------------------------------
/**
    Draw current mesh with indexed primitives.
*/
void
nGfxServer2::DrawIndexed(nPrimitiveType primType)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Draw current mesh with non-indexed primitives.
*/
void
nGfxServer2::Draw(nPrimitiveType primType)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render indexed primitives without applying shader state. You have
    to call nShader2::Begin(), nShader2::Pass() and nShader2::End() 
    yourself as needed.
*/
void
nGfxServer2::DrawIndexedNS(nPrimitiveType primType)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render non-indexed primitives without applying shader state. You have
    to call nShader2::Begin(), nShader2::Pass() and nShader2::End() 
    yourself as needed.
*/
void
nGfxServer2::DrawNS(nPrimitiveType primType)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Add text to the text buffer (OBSOLETE)
*/
void
nGfxServer2::Text(const char* /*text*/, const vector4& /*color*/, float /*x*/, float /*y*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render the text in the text buffer (OBSOLETE)
*/
void
nGfxServer2::DrawTextBuffer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Draw text.

    @param  text    the text to draw
    @param  color   the text color
    @param  xPos    screen space x position
    @param  yPos    screen space y position
*/
void
nGfxServer2::DrawText(const char* text, const vector4& color, float xPos, float yPos)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Return the text extents of the given text using the current font in
    screen space coordinates.
*/
vector2
nGfxServer2::GetTextExtent(const char* text)
{
    return vector2(0.0f, 0.0f);
}

//------------------------------------------------------------------------------
/**
    Return the supported feature set.
*/
nGfxServer2::FeatureSet
nGfxServer2::GetFeatureSet()
{
    return InvalidFeatureSet;
}

//------------------------------------------------------------------------------
/**
    Save a screenshot.
*/
bool
nGfxServer2::SaveScreenshot(const char* filename)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Set cursor visibility.
*/
void
nGfxServer2::SetCursorVisibility(CursorVisibility v)
{
    this->cursorVisibility = v;
}

//------------------------------------------------------------------------------
/**
    Get the mouse cursor visibility status.
*/
nGfxServer2::CursorVisibility
nGfxServer2::GetCursorVisibility() const
{
    return this->cursorVisibility;
}
