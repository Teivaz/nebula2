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
#include "kernel/nautoref.h"
#include "mathlib/matrix.h"
#include "gfx2/ntexture2.h"
#include "mathlib/rectangle.h"
#include "gfx2/nmousecursor.h"
#include "gfx2/nshaderlist.h"
#include "gfx2/ndisplaymode2.h"
#include "gfx2/nfont2.h"

//------------------------------------------------------------------------------
class nMesh2;
class nShader2;
class nResourceServer;
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
        ViewProjection,         ///< current view * projection matrix
        Texture0,               ///< texture transform for layer 0 (read/write)
        Texture1,
        Texture2,
        Texture3,

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

    /// primitive types
    enum PrimitiveType
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan,
    };

    /// feature sets (from worst to best)
    enum FeatureSet
    {
        InvalidFeatureSet = 0,      // Open() hasn't been called yet
        DX7,                        // a typical dx7 card with fixed function pipeline
        DX8,                        // a typical dx8 card with at least vs/ps 1.1
        DX8SB,                      // a typical dx8 card with support for shadow buffers
        DX9,                        // a dx9 card with at least vs/ps 2.0
        DX9FLT,                     // a dx9 card with floating point textures/render targets
    };

    /// the visible mouse cursor type
    enum CursorVisibility
    {
        None,               // no mouse cursor visible
        System,             // use Window's system mouse cursor
        Custom,             // use the custom mouse cursor
    };

    enum MeshSource
    {
        NoSource,
        SingleMesh,         // one single mesh is to be drawn
        //MeshArray,          // a mesh array is to be drawn
    };

    enum
    {
        MaxVertexStreams = 16,
        MaxTextureStages = 4,
        MaxTransformStackDepth = 4,
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
    virtual void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    virtual const nDisplayMode2& GetDisplayMode() const;
    /// set the current camera description
    virtual void SetCamera(nCamera2& cam);
    /// override the feature set
    void SetFeatureSetOverride(FeatureSet f);
    /// get the best supported feature set
    virtual FeatureSet GetFeatureSet();
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
    /// get prioritized shader index by shader name
    int GetShaderIndex(const char* shaderName);
    
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
    virtual void SetMesh(nMesh2* mesh);
    /// get current mesh
    nMesh2* GetMesh() const;
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

    /// draw the current mesh with indexed primitives
    virtual void DrawIndexed(PrimitiveType primType);
    /// draw the current mesh with non-indexed primitives
    virtual void Draw(PrimitiveType primType);
    /// render indexed primitives without applying shader state (NS == No Shader)
    virtual void DrawIndexedNS(PrimitiveType primType);
    /// render non-indexed primitives without applying shader state (NS == No Shader)
    virtual void DrawNS(PrimitiveType primType);
    /// draw text (immediately)
    virtual void DrawText(const char* text, const vector4& color, const rectangle& rect, uint flags);
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

    /// enter dialog box mode (display mode must have DialogBoxMode enabled!)
    virtual void EnterDialogBoxMode();
    /// leave dialog box mode
    virtual void LeaveDialogBoxMode();
    /// return true if currently in dialog box mode
    bool InDialogBoxMode() const;

    /// save a screen shot
    virtual bool SaveScreenshot(const char* filename);    

    /// convert feature set string to enum
    static FeatureSet StringToFeatureSet(const char* str);
    /// convert feature set enum to string
    static const char* FeatureSetToString(FeatureSet f);
    /// convert cursor visibility string to enum
    static CursorVisibility StringToCursorVisibility(const char* str);

protected:
    bool displayOpen;
    bool inBeginScene;

    nAutoRef<nResourceServer> refResource;
    nCamera2 camera;
    nViewport viewport;

    nRef<nTexture2> refRenderTarget;
    nRef<nMesh2>    refMesh;

    nRef<nTexture2> refTextures[MaxTextureStages];
    nRef<nFont2>    refFont;
    nRef<nShader2>  refShader;
    nMouseCursor curMouseCursor;
    nShaderList shaderList;
    int vertexRangeFirst;
    int vertexRangeNum;
    int indexRangeFirst;
    int indexRangeNum;
    FeatureSet featureSetOverride;

    matrix44 transform[NumTransformTypes];
    int transformTopOfStack[NumTransformTypes];
    matrix44 transformStack[NumTransformTypes][MaxTransformStackDepth];
    bool cursorDirty;
    bool inDialogBoxMode;
    MeshSource meshSource;  // draw single mesh or mesh array?
public:
    // note: this stuff is public because WinProcs may need to access it
    CursorVisibility cursorVisibility;
};

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::FeatureSet
nGfxServer2::StringToFeatureSet(const char* str)
{
    n_assert(str);
    if (0 == strcmp(str, "dx7"))
    {
        return DX7;
    }
    else if (0 == strcmp(str, "dx8"))
    {
        return DX8;
    }
    else if (0 == strcmp(str, "dx8sb"))
    {
        return DX8SB;
    }
    else if (0 == strcmp(str, "dx9"))
    {
        return DX9;
    }
    else if (0 == strcmp(str, "dx9flt"))
    {
        return DX9FLT;
    }
    else 
    {
        return InvalidFeatureSet;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGfxServer2::FeatureSetToString(FeatureSet f)
{
    switch (f)
    {
        case nGfxServer2::DX7:      return "dx7"; break;
        case nGfxServer2::DX8:      return "dx8"; break;
        case nGfxServer2::DX8SB:    return "dx8sb"; break;
        case nGfxServer2::DX9:      return "dx9"; break;
        case nGfxServer2::DX9FLT:   return "dx9flt"; break;
        default:                    return "invalid"; break;
    }
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
*/
inline
void
nGfxServer2::SetFeatureSetOverride(FeatureSet f)
{
    this->featureSetOverride = f;
    n_printf("nGfxServer2: set feature set override to '%s'\n", FeatureSetToString(f));
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

    @return             pointer to current nMesh2 object
*/
inline
nMesh2*
nGfxServer2::GetMesh() const
{
    return this->refMesh.isvalid() ? this->refMesh.get() : 0;
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
    n_assert((stage >= 0) && (stage < MaxTextureStages));
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
/**
    Get shader priority index, which is generated by parsing the
    shaders:shaderlist.txt file. Can be used for priority sorting
    of scene nodes in the scene server.

    @param  shaderName  shader filename, e.g. "shaders:default.fx"
    @return             priority index of shader
*/
inline
int
nGfxServer2::GetShaderIndex(const char* shaderName)
{
    if (!this->shaderList.IsLoaded())
    {
        if (!this->shaderList.Load("shaders:shaderlist.txt"))
        {
            n_error("nGfxServer2::GetShaderIndex(): Failed to parse 'shaders:shaderlist.txt'!");
            return -1;
        }
    }
    int index = this->shaderList.GetShaderIndex(shaderName);
    if (index == -1)
    {
        n_error("Shader '%s' not defined in 'shaders:shaderlist.txt'!", shaderName);
    }
    return index;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::CursorVisibility
nGfxServer2::StringToCursorVisibility(const char* str)
{
    n_assert(str);
    if (0 == strcmp(str, "none"))
    {
        return nGfxServer2::None;
    }
    else if (0 == strcmp(str, "system"))
    {
        return nGfxServer2::System;
    }
    else if (0 == strcmp(str, "custom"))
    {
        return nGfxServer2::Custom;
    }
    else
    { 
        n_error( "Invalid string '%s' passed to StringToCursorVisibility!", str );
    }
}
//------------------------------------------------------------------------------
#endif
