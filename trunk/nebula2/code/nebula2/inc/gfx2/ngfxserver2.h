#ifndef N_GFXSERVER2_H
#define N_GFXSERVER2_H
//------------------------------------------------------------------------------
/**
    @class nGfxServer2

    New generation gfx server, completely vertex and pixel shader based.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_CAMERA2_H
#include "gfx2/ncamera2.h"
#endif

#ifndef N_DISPLAYMODE2_H
#include "gfx2/ndisplaymode2.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_TEXTURE2_H
#include "gfx2/ntexture2.h"
#endif

#undef N_DEFINES
#define N_DEFINES nGfxServer2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nMesh2;
class nShader2;
class nResourceServer;
class N_PUBLIC nGfxServer2 : public nRoot
{
public:
    /// transform types
    enum nTransformType
    {
        MODELVIEW = 0,          // current modelview matrix (read/write)
        PROJECTION,             // current projection matrix (read/write)
        INVMODELVIEW,           // inverse of current modelview matrix (read only)
        MODELVIEWPROJECTION,    // the combined modelview-projection matrix (read only)
        VIEW,                   // current view matrix (read/write)
        INVVIEW,                // current inverse view matrix (read only)
        MODEL,                  // the model (world) matrix (read only)

        NUM_TRANSFORMTYPES,
    };

    /// buffer types
    enum nBufferType
    {
        COLOR = (1<<0),
        DEPTH = (1<<1),
        STENCIL = (1<<2),
        ALL = (COLOR | DEPTH | STENCIL),
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
    /// create a render target object
    virtual nTexture2* NewRenderTarget(const char* rsrcName, int width, int height, nTexture2::Format fmt, bool hasColor, bool hasDepth, bool hasStencil);
    /// claim access rights to dynamic mesh
    virtual nMesh2* LockDynamicMesh();
    /// give up access to dynamic mesh
    virtual void UnlockDynamicMesh(nMesh2* dynMesh);

    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// set window title
    virtual void SetWindowTitle(const char* title);
    /// get window title
    const char* GetWindowTitle() const;
    /// set the current camera description
    virtual void SetCamera(const nCamera2& cam);
    /// get the current camera description
    const nCamera2& GetCamera() const;
    /// open the display
    virtual bool OpenDisplay();
    /// close the display
    virtual void CloseDisplay();
    /// trigger the window system message pump
    virtual bool Trigger();
    
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
    /// set transform
    virtual void SetTransform(nTransformType type, const matrix44& matrix);
    /// get transform
    const matrix44& GetTransform(nTransformType type) const;
    /// push transform
    void PushTransform(nTransformType type, const matrix44& matrix);
    /// pop transform
    const matrix44& PopTransform(nTransformType type);
    /// set vertex range to render from current mesh
    void SetVertexRange(int firstVertex, int numVertices);
    /// set index range to render from current mesh
    void SetIndexRange(int firstIndex, int numIndices);

    /// draw the current mesh, texture and shader
    virtual void Draw();

    /// add text to the text buffer
    virtual void Text(const char* text, float x, float y);
    /// draw the text buffer
    virtual void DrawTextBuffer();

    static nKernelServer* kernelServer;

    enum
    {
        MAX_VERTEXSTREAMS = 16,
        MAX_TEXTURESTAGES = 8,
        MAX_TRANSFORMSTACKDEPTH = 4,
    };

protected:
    nString windowTitle;
    bool displayOpen;
    bool inBeginScene;

    nAutoRef<nResourceServer> refResource;
    nCamera2 camera;

    nRef<nTexture2> refRenderTarget;
    nRef<nMesh2>    refMeshes[MAX_VERTEXSTREAMS];
    nRef<nTexture2> refTextures[MAX_TEXTURESTAGES];
    nRef<nShader2>  refShader;
    int vertexRangeFirst;
    int vertexRangeNum;
    int indexRangeFirst;
    int indexRangeNum;
    
    matrix44 transform[NUM_TRANSFORMTYPES];
    int transformTopOfStack[NUM_TRANSFORMTYPES];
    matrix44 transformStack[NUM_TRANSFORMTYPES][MAX_TRANSFORMSTACKDEPTH];

public:
    // note: this stuff is public because WinProcs may need to access it
    nDisplayMode2 displayMode;
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
*/
inline
const char*
nGfxServer2::GetWindowTitle() const
{
    return this->windowTitle.Get();
}

//------------------------------------------------------------------------------
/**
    Get the current camera object.
*/
inline
const nCamera2&
nGfxServer2::GetCamera() const
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
    n_assert((stage >= 0) && (stage < MAX_VERTEXSTREAMS));
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
    Set transformation matrix.

    @param  type        transform type
    @return             the 4x4 matrix
*/
inline
const matrix44&
nGfxServer2::GetTransform(nTransformType type) const
{
    n_assert(type < NUM_TRANSFORMTYPES);
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
