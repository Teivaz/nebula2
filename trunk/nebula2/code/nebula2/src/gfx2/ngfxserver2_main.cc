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
#include "gfx2/nmesharray.h"

nNebulaScriptClass(nGfxServer2, "nroot");
nGfxServer2* nGfxServer2::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nGfxServer2::nGfxServer2() :
    displayOpen(false),
    inBeginScene(false),
    inBeginLines(false),
    inBeginShapes(false),
    refResource("/sys/servers/resource"),
    vertexRangeFirst(0),
    vertexRangeNum(0),
    indexRangeFirst(0),
    indexRangeNum(0),
    featureSetOverride(InvalidFeatureSet),
    cursorVisibility(System),
    cursorDirty(true),
    inDialogBoxMode(false),
    gamma(1.0f),
    brightness(65280.0/65535.0-0.5),
    contrast(65280.0/65535.0-0.5)
{
    n_assert(0 == Singleton);
    Singleton = this;

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
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Create a new static mesh object.

    @param  rsrcName    a resource name for resource sharing
    @return             a new nMesh2 object
*/
nMesh2*
nGfxServer2::NewMesh(const char* /*rsrcName*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new static mesh array object.

    @return             a new nMeshArray object
*/
nMeshArray*
nGfxServer2::NewMeshArray(const char* /*rsrcName*/)
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
nGfxServer2::NewTexture(const char* /*rsrcName*/)
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
nGfxServer2::NewShader(const char* /*rsrcName*/)
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
nGfxServer2::NewFont(const char* /*rsrcName*/, const nFontDesc& /*fontDesc*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new shared instance stream object.

    @param  rsrcName    a resource name for resource sharing (0 if no sharing)
    @return             pointer to a new nInstanceStream object
*/
nInstanceStream*
nGfxServer2::NewInstanceStream(const char* rsrcName)
{
    return (nInstanceStream*) this->refResource->NewResource("ninstancestream", rsrcName, nResource::Other);
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
nGfxServer2::NewRenderTarget(const char* /*rsrcName*/,
                             int /*width*/,
                             int /*height*/,
                             nTexture2::Format /*format*/,
                             int /*usageFlags*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Set the prefered display mode. This must be called outside
    OpenDisplay()/CloseDisplay().
*/
void
nGfxServer2::SetDisplayMode(const nDisplayMode2& /*mode*/)
{
    n_error("nGfxServer2: Pure virtual function called!");
}

//------------------------------------------------------------------------------
/**
*/
const nDisplayMode2&
nGfxServer2::GetDisplayMode() const
{
    n_error("nGfxServer2: Pure virtual function called!");
    static nDisplayMode2 dummy;
    return dummy;
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

    23-Aug-04    kims    added calling RestoreGamma().
*/
void
nGfxServer2::CloseDisplay()
{
    n_assert(this->displayOpen);
    this->displayOpen = false;

    // restore to original.
    this->RestoreGamma();
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
    Reset the light array. BeginScene() will call this.
*/
void
nGfxServer2::ClearLights()
{
    this->lightArray.Reset();
}

//------------------------------------------------------------------------------
/**
    Add a light to the light array. Return new number of lights.
*/
int
nGfxServer2::AddLight(const nLight& light)
{
    this->lightArray.Append(light);
    return this->lightArray.Size();
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
        // reset the light array
        this->ClearLights();
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
nGfxServer2::Clear(int /*bufferTypes*/, float /*red*/, float /*green*/, float /*blue*/, float /*alpha*/, float /*z*/, int /*stencil*/)
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
    if (t)
    {
        t->AddRef();
    }
    if (this->refRenderTarget.isvalid())
    {
        this->refRenderTarget->Release();
        this->refRenderTarget.invalidate();
    }
    this->refRenderTarget = t;
}

//------------------------------------------------------------------------------
/**
    Set the current mesh object for rendering.
    This will increment its refcount and decrement the refcount 
    of the previous object.

    @param  mesh        pointer to a nMesh2 object
*/
void
nGfxServer2::SetMesh(nMesh2* mesh)
{
    if (mesh)
    {
        mesh->AddRef();
    }
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
        this->refMesh.invalidate();
    }
    this->refMesh = mesh;
}

//------------------------------------------------------------------------------
/**
    Set the current mesh array object for rendering.
    This will increment its refcount and decrement the refcount 
    of the previous object.

    @param  meshArray   pointer to a nMeshArray object
*/
void
nGfxServer2::SetMeshArray(nMeshArray* meshArray)
{
    if (0 != meshArray)
    {
        meshArray->AddRef();
    }
    if (this->refMeshArray.isvalid())
    {
        this->refMeshArray->Release();
        this->refMeshArray.invalidate();
    }
    this->refMeshArray = meshArray;
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
    n_assert((stage >= 0) && (stage < MaxTextureStages));

    if (texture)
    {
        texture->AddRef();
    }
    if (this->refTextures[stage].isvalid())
    {
        this->refTextures[stage]->Release();
        this->refTextures[stage].invalidate();
    }
    this->refTextures[stage] = texture;
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
    if (shd)
    {
        shd->AddRef();
    }
    if (this->refShader.isvalid())
    {
        this->refShader->Release();
        this->refShader.invalidate();
    }
    this->refShader = shd;
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
    if (font)
    {
        font->AddRef();
    }
    if (this->refFont.isvalid())
    {
        this->refFont->Release();
        this->refFont.invalidate();
    }
    this->refFont = font;
}

//------------------------------------------------------------------------------
/**
    Set the current instance stream object for rendering.
    This will increment its refcount and decrement the refcount of
    the previous object.

    @param  stream      pointer to nInstanceStream object
*/
void
nGfxServer2::SetInstanceStream(nInstanceStream* stream)
{
    if (stream)
    {
        stream->AddRef();
    }
    if (this->refInstanceStream.isvalid())
    {
        this->refInstanceStream->Release();
        this->refInstanceStream.invalidate();
    }
    this->refInstanceStream = stream;
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
    bool updModelLight = false;
    bool updViewProjection = false;
    bool updModelLightProj = false;
    bool updModelLightProjection = false;
    switch (type)
    {
        case Model:
            this->transform[Model] = matrix;
            this->transform[InvModel] = matrix;
            this->transform[InvModel].invert_simple();
            updModelView = true;
            updModelLight = true;
            break;

        case View:
            this->transform[View] = matrix;
            this->transform[InvView] = matrix;
            this->transform[InvView].invert_simple();
            updModelView = true;
            updViewProjection = true;
            break;

        case Projection:
            this->transform[Projection] = matrix;
            updViewProjection = true;
            break;

        case Texture0:
        case Texture1:
        case Texture2:
        case Texture3:
            this->transform[type] = matrix;
            break;

        case Light:
            this->transform[type] = matrix;
            updModelLight = true;
            updModelLightProj = true;
            break;


        default:
            n_error("nGfxServer2::SetTransform() Trying to set read-only transform type!");
            break;
    }

    if (updModelView)
    {
        this->transform[ModelView]    = this->transform[Model] * this->transform[View];
        this->transform[InvModelView] = this->transform[InvView] * this->transform[InvModel];
    }
    if (updModelLight)
    {
        this->transform[ModelLight] = this->transform[Model] * this->transform[Light];
        this->transform[InvModelLight] = this->transform[Light] * this->transform[InvModel];
    }
    if (updViewProjection)
    {
        this->transform[ViewProjection] = this->transform[View] * this->transform[Projection];
    }

    // update the modelview/projection matrix
    if (updModelView || updViewProjection)
    {
        this->transform[ModelViewProjection] = this->transform[ModelView] * this->transform[Projection];
    }
    if (updModelLight || updModelLightProjection)
    {
        this->transform[ModelLightProjection] = this->transform[ModelLight] * this->transform[Projection];
    }
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
    n_assert(this->transformTopOfStack[type] < MaxTransformStackDepth);
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
nGfxServer2::DrawIndexed(PrimitiveType /*primType*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Draw current mesh with non-indexed primitives.
*/
void
nGfxServer2::Draw(PrimitiveType /*primType*/)
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
nGfxServer2::DrawIndexedNS(PrimitiveType /*primType*/)
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
nGfxServer2::DrawNS(PrimitiveType /*primType*/)
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
    @param  rect    screen space rectangle in which to draw the text
    @param  flags   combination of nFont2::RenderFlags
*/
void
nGfxServer2::DrawText(const char* /*text*/, const vector4& /*color*/, const rectangle& /*rect*/, uint /*flags*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Return the text extents of the given text using the current font in
    screen space coordinates.
*/
vector2
nGfxServer2::GetTextExtent(const char* /*text*/)
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
nGfxServer2::SaveScreenshot(const char* /*filename*/)
{
    // implement me in subclass
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

//------------------------------------------------------------------------------
/**
    Enter dialog box mode.
*/
void
nGfxServer2::EnterDialogBoxMode()
{
    n_assert(!this->inDialogBoxMode);
    this->inDialogBoxMode = true;
}

//------------------------------------------------------------------------------
/**
    Leave dialog box mode.
*/
void
nGfxServer2::LeaveDialogBoxMode()
{
    n_assert(this->inDialogBoxMode);
    this->inDialogBoxMode = false;
}

//------------------------------------------------------------------------------
/**
    Return true if currently in dialog box mode.
*/
bool
nGfxServer2::InDialogBoxMode() const
{
    return this->inDialogBoxMode;
}

//------------------------------------------------------------------------------
/**
    This method should return the number of currently available stencil bits 
    (override in subclass).
*/
int
nGfxServer2::GetNumStencilBits() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    This method should return the number of currently available depth bits
    (override in subclass).
*/
int
nGfxServer2::GetNumDepthBits() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Begin rendering lines. Override this method in a subclass.
*/
void
nGfxServer2::BeginLines()
{
    n_assert(!this->inBeginLines);
    this->inBeginLines = true;
}

//------------------------------------------------------------------------------
/**
    Draw a 3d line strip using the current transforms. 
*/
void
nGfxServer2::DrawLines3d(const vector3* vertexList, int numVertices, const vector4& color)
{
    // empty    
}

//------------------------------------------------------------------------------
/**
    Draw a 2d lines in screen space.
*/
void
nGfxServer2::DrawLines2d(const vector2* vertexList, int numVertices,const vector4& color)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Finish rendering lines. Override this method in a subclass.
*/
void
nGfxServer2::EndLines()
{
    n_assert(this->inBeginLines);
    this->inBeginLines = false;
}

//------------------------------------------------------------------------------
/**
    Begin rendering shapes.
*/
void
nGfxServer2::BeginShapes()
{
    n_assert(!this->inBeginShapes);
    this->inBeginShapes = true;
}

//------------------------------------------------------------------------------
/**
    Render a shape.
*/
void
nGfxServer2::DrawShape(ShapeType type, const matrix44& model, const vector4& color)
{
    n_assert(this->inBeginShapes);
}

//------------------------------------------------------------------------------
/**
    Finish shape drawing.
*/
void
nGfxServer2::EndShapes()
{
    n_assert(this->inBeginShapes);
    this->inBeginShapes = false;
}

//------------------------------------------------------------------------------
/**
    23-Aug-04    kims    created
*/
void
nGfxServer2::AdjustGamma()
{
    // empty.
}

//------------------------------------------------------------------------------
/**
    23-Aug-04    kims    created
*/
void
nGfxServer2::RestoreGamma()
{
    // empty.
}