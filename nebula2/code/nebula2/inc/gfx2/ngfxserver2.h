#ifndef N_GFXSERVER2_H
#define N_GFXSERVER2_H
//------------------------------------------------------------------------------
/**
    @class nGfxServer2
    @ingroup NebulaGraphicsSystem

    New generation gfx server, completely vertex and pixel shader based.

    See also @ref N2ScriptInterface_ngfxserver2

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "gfx2/ncamera2.h"
#include "gfx2/ndisplaymode2.h"
#include "kernel/nautoref.h"
#include "mathlib/matrix.h"
#include "gfx2/ntexture2.h"
#include "mathlib/rectangle.h"
#include "gfx2/nprimitivetypes.h"
#include "gfx2/nmousecursor.h"

//------------------------------------------------------------------------------
class nMesh2;
class nShader2;
class nResourceServer;
class nFont2;
class nFontDesc;

class nViewport
{
public:
    float x;
    float y;
    float width;
    float height;
    float nearz;
    float farz;
};

class nGfxServer2 : public nRoot
{
public:
    /// transform types
    enum TransformType
    {
        Model = 0,              ///< model -> world matrix (read/write)
        View,                   ///< world -> view matrix (read/write)
        Projection,             ///< view  -> projection matrix (read/write)
        InvModel,               ///< world -> model matrix (read only)
        InvView,                ///< view  -> world matrix (read only)
        ModelView,              ///< model -> view matrix (read only)
        InvModelView,           ///< view -> model matrix (read only)
        ModelViewProjection,    ///< model -> projection matrix (read only)

        NumTransformTypes
    };

    /// buffer types
    enum BufferType
    {
        ColorBuffer   = (1<<0),
        DepthBuffer   = (1<<1),
        StencilBuffer = (1<<2),
        AllBuffers    = (ColorBuffer | DepthBuffer | StencilBuffer),
    };

    /// feature sets (from worst to best)
    enum FeatureSet
    {
        InvalidFeatureSet = 0,      // Open() hasn't been called yet
        DX7,                        // a typical dx7 card with fixed function pipeline
        DX8,                        // a typical dx8 card with at least vs/ps 1.1
        DX8SB,                      // a typical dx8 card with support for shadow buffers
        DX9,                        // a dx9 card with at least vs/ps 2.0 AND float textures
    };

    /// the visible mouse cursor type
    enum CursorVisibility
    {
        None,               // no mouse cursor visible
        System,             // use Window's system mouse cursor
        Custom,             // use the custom mouse cursor
    };

    /// constructor
    nGfxServer2();
    /// destructor
    virtual ~nGfxServer2();

    /// create a shared mesh object
    virtual nMesh2* NewMesh(const char* rsrcName);
    /// create a shared texture object
    virtual nTexture2* NewTexture(const char* rsrcName);
    /// create a shared shader object
    virtual nShader2* NewShader(const char* rsrcName);
    /// create a font object
    virtual nFont2* NewFont(const char* rsrcName, const nFontDesc& fontDesc);
    /// create a render target object
    virtual nTexture2* NewRenderTarget(const char* rsrcName, int width, int height, nTexture2::Format fmt, int usageFlags);

    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// set the current camera description
    virtual void SetCamera(nCamera2& cam);
    /// get the current camera description
    nCamera2& GetCamera();
    /// set the viewport
    virtual void SetViewport(nViewport& vp);
    /// get the viewport
    virtual nViewport& GetViewport();
    /// open the display
    virtual bool OpenDisplay();
    /// close the display
    virtual void CloseDisplay();
    /// trigger the window system message pump
    virtual bool Trigger();
    /// get the best supported feature set
    virtual FeatureSet GetFeatureSet();
    
    /// set a new render target texture
    virtual void SetRenderTarget(nTexture2* t);
    /// get the current render target
    nTexture2* GetRenderTarget() const;
    
    /// start rendering to current render target
    virtual bool BeginScene();
    /// finish rendering to current render target
    virtual void EndScene();
    /// present the contents of the back buffer
    virtual void PresentScene();
    /// clear buffers
    virtual void Clear(int bufferTypes, float red, float green, float blue, float alpha, float z, int stencil);

    /// set current mesh
    virtual void SetMesh(int stream, nMesh2* mesh);
    /// get current mesh
    nMesh2* GetMesh(int stream) const;
    /// set current texture
    virtual void SetTexture(int stage, nTexture2* tex);
    /// get current texture
    nTexture2* GetTexture(int stage) const;
    /// set current shader
    virtual void SetShader(nShader2* shader);
    /// get current shader
    nShader2* GetShader() const;
    /// set current font
    virtual void SetFont(nFont2* font);
    /// get current font
    nFont2* GetFont() const;
    /// set transform
    virtual void SetTransform(TransformType type, const matrix44& matrix);
    /// get transform
    const matrix44& GetTransform(TransformType type) const;
    /// push transform
    void PushTransform(TransformType type, const matrix44& matrix);
    /// pop transform
    const matrix44& PopTransform(TransformType type);
    /// set vertex range to render from current mesh
    void SetVertexRange(int firstVertex, int numVertices);
    /// set index range to render from current mesh
    void SetIndexRange(int firstIndex, int numIndices);
    /// set npatch tesselation level
    void SetNPatchSegments(float segments);
    /// get npatch tesselation level
    float GetNPatchSegments() const;

    /// draw the current mesh with indexed primitives
    virtual void DrawIndexed(nPrimitiveType primType);
    /// draw the current mesh with non-indexed primitives
    virtual void Draw(nPrimitiveType primType);
    /// render indexed primitives without applying shader state (NS == No Shader)
    virtual void DrawIndexedNS(nPrimitiveType primType);
    /// render non-indexed primitives without applying shader state (NS == No Shader)
    virtual void DrawNS(nPrimitiveType primType);
    /// draw text (immediately)
    virtual void DrawText(const char* text, const vector4& color, float xPos, float yPos);
    /// get text extents
    virtual vector2 GetTextExtent(const char* text);

    /// add text to the text buffer (OLD STYLE)
    virtual void Text(const char* text, const vector4& color, float xPos, float yPos);
    /// draw the text buffer
    virtual void DrawTextBuffer();

    /// set mouse cursor image and hotspot
    virtual void SetMouseCursor(const nMouseCursor& cursor);
    /// get mouse cursor image
    virtual const nMouseCursor& GetMouseCursor() const;
    /// show/hide the mouse cursor
    virtual void SetCursorVisibility(CursorVisibility type);
    /// get mouse cursor display status
    virtual CursorVisibility GetCursorVisibility() const;
    
    /// save a screen shot
    virtual bool SaveScreenshot(const char* filename);    

    enum
    {
        MAX_VERTEXSTREAMS = 16,
        MAX_TEXTURESTAGES = 8,
        MAX_TRANSFORMSTACKDEPTH = 4,
    };

protected:
    bool displayOpen;
    bool inBeginScene;

    nAutoRef<nResourceServer> refResource;
    nCamera2 camera;
    nViewport viewport;

    nRef<nTexture2> refRenderTarget;
    nRef<nMesh2>    refMeshes[MAX_VERTEXSTREAMS];
    nRef<nTexture2> refTextures[MAX_TEXTURESTAGES];
    nRef<nFont2>    refFont;
    nRef<nShader2>  refShader;
    nMouseCursor curMouseCursor;
    int vertexRangeFirst;
    int vertexRangeNum;
    int indexRangeFirst;
    int indexRangeNum;
    float nPatchSegments;
    
    matrix44 transform[NumTransformTypes];
    int transformTopOfStack[NumTransformTypes];
    matrix44 transformStack[NumTransformTypes][MAX_TRANSFORMSTACKDEPTH];
    bool cursorDirty;

public:
    // note: this stuff is public because WinProcs may need to access it
    nDisplayMode2 displayMode;
    CursorVisibility cursorVisibility;
};

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
nGfxServer2::GetDisplayMode() const
{
    return this->displayMode;
}


//------------------------------------------------------------------------------
/**
    Get the current camera object.
*/
inline
nCamera2&
nGfxServer2::GetCamera()
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
    Get the current viewport.
*/
inline
nViewport&
nGfxServer2::GetViewport()
{
    return this->viewport;
}

//------------------------------------------------------------------------------
/**
    Get the current render target.

    @return     the current render target, or 0 if frame buffer if render target
*/
inline
nTexture2*
nGfxServer2::GetRenderTarget() const
{
    return this->refRenderTarget.isvalid() ? this->refRenderTarget.get() : 0;
}

//------------------------------------------------------------------------------
/**
    Get the current mesh.

    @param  stream      vertex stream index
    @return             pointer to current nMesh2 object on that stream
*/
inline
nMesh2*
nGfxServer2::GetMesh(int stream) const
{
    n_assert((stream >= 0) && (stream < MAX_VERTEXSTREAMS));
    return this->refMeshes[stream].isvalid() ? this->refMeshes[stream].get() : 0;
}

//------------------------------------------------------------------------------
/**
    Get the current texture.

    @param  stage       texture stage index
    @return             pointer to nTexture2 object
*/
inline
nTexture2*
nGfxServer2::GetTexture(int stage) const
{
    n_assert((stage >= 0) && (stage < MAX_TEXTURESTAGES));
    return this->refTextures[stage].isvalid() ? this->refTextures[stage].get() : 0;
}

//------------------------------------------------------------------------------
/**
    Get the current shader.

    @return             pointer to nShader2 object
*/
inline
nShader2*
nGfxServer2::GetShader() const
{
    return this->refShader.isvalid() ? this->refShader.get() : 0;
}

//------------------------------------------------------------------------------
/**
    Get the current font object.

    @return             pointer to nFont2 object
*/
inline
nFont2*
nGfxServer2::GetFont() const
{
    return this->refFont.isvalid() ? this->refFont.get() : 0;
}

//------------------------------------------------------------------------------
/**
    Set transformation matrix.

    @param  type        transform type
    @return             the 4x4 matrix
*/
inline
const matrix44&
nGfxServer2::GetTransform(TransformType type) const
{
    n_assert(type < NumTransformTypes);
    return this->transform[type];
}

//------------------------------------------------------------------------------
/**
    Set vertex range to render in current mesh.

    @param  firstVertex     index of first vertex
    @param  numVertices     number of vertices 
*/
inline
void
nGfxServer2::SetVertexRange(int firstVertex, int numVertices)
{
    this->vertexRangeFirst = firstVertex;
    this->vertexRangeNum   = numVertices;
}

//------------------------------------------------------------------------------
/**
    Set index range to render in current mesh.

    @param  firstIndex      index of first primitive index
    @param  numIndices      number of indices
*/
inline
void
nGfxServer2::SetIndexRange(int firstIndex, int numIndices)
{
    this->indexRangeFirst = firstIndex;
    this->indexRangeNum   = numIndices;
}

//------------------------------------------------------------------------------
#endif
