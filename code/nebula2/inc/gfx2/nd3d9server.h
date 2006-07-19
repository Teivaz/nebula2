#ifndef N_D3D9SERVER_H
#define N_D3D9SERVER_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Server
    @ingroup Gfx2

    D3D9 based gfx server.

    (C) 2002 RadonLabs GmbH
*/
#include "gfx2/ngfxserver2.h"
#include "input/ninputserver.h"
#include "misc/nwatched.h"
#include "gfx2/nd3d9windowhandler.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

#if D3D_SDK_VERSION < 32
#error You must be using the DirectX 9 October 2004 Update SDK!  You may download it from http://www.microsoft.com/downloads/details.aspx?FamilyID=b7bc31fa-2df1-44fd-95a4-c2555446aed4&DisplayLang=en
#endif

//------------------------------------------------------------------------------
//  Debugging definitions (for shader debugging etc...)
//------------------------------------------------------------------------------
#define N_D3D9_USENVPERFHUD (0)

#define N_D3D9_DEBUG (0)

#if N_D3D9_USENVPERFHUD
#define N_D3D9_DEVICETYPE D3DDEVTYPE_REF
#define N_D3D9_ADAPTER (nD3D9Server::Instance()->d3d9->GetAdapterCount() - 1)
#else
#define N_D3D9_DEVICETYPE D3DDEVTYPE_HAL
#define N_D3D9_ADAPTER D3DADAPTER_DEFAULT
#endif

#define N_D3D9_FORCEMIXEDVERTEXPROCESSING (0)
#define N_D3D9_FORCESOFTWAREVERTEXPROCESSING (0)

//------------------------------------------------------------------------------
class nD3D9Server : public nGfxServer2
{
public:
    /// constructor
    nD3D9Server();
    /// destructor
    virtual ~nD3D9Server();
    /// get instance pointer
    static nD3D9Server* Instance();

    /// create a shared mesh object
    virtual nMesh2* NewMesh(const char* rsrcName);
    /// create a mesh array object
    virtual nMeshArray* NewMeshArray(const char* rsrcName);
    /// create a shared texture object
    virtual nTexture2* NewTexture(const char* rsrcName);
    /// create a shared shader object
    virtual nShader2* NewShader(const char* rsrcName);
    /// create a font object
    virtual nFont2* NewFont(const char* rsrcName, const nFontDesc& fontDesc);
    /// create a render target object
    virtual nTexture2* NewRenderTarget(const char* rsrcName, int width, int height, nTexture2::Format fmt, int usageFlags);
    /// create a new occlusion query object
    virtual nOcclusionQuery* NewOcclusionQuery();

    /// set display mode
    virtual void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    virtual const nDisplayMode2& GetDisplayMode() const;
    /// set the window title
    virtual void SetWindowTitle(const char* title);
    /// set the current camera description
    virtual void SetCamera(nCamera2& cam);
    /// set the viewport
    virtual void SetViewport(nViewport& vp);
    /// open the display
    virtual bool OpenDisplay();
    /// close the display
    virtual void CloseDisplay();
    /// get the best supported feature set
    virtual FeatureSet GetFeatureSet();
    /// return true if vertex shader run in software emulation
    virtual bool AreVertexShadersEmulated();
    /// parent window handle
    virtual HWND GetParentHwnd() const;
    /// returns the number of available stencil bits
    virtual int GetNumStencilBits() const;
    /// returns the number of available z bits
    virtual int GetNumDepthBits() const;
    /// set scissor rect
    virtual void SetScissorRect(const rectangle& r);
    /// set or clear user defined clip planes in clip space
    virtual void SetClipPlanes(const nArray<plane>& planes);

    /// set a new render target texture
    virtual void SetRenderTarget(int index, nTexture2* t);
    
    /// start rendering the current frame
    virtual bool BeginFrame();
    /// start rendering to current render target
    virtual bool BeginScene();
    /// finish rendering to current render target
    virtual void EndScene();
    /// present the contents of the back buffer
    virtual void PresentScene();
    /// end rendering the current frame
    virtual void EndFrame();
    /// clear buffers
    virtual void Clear(int bufferTypes, float red, float green, float blue, float alpha, float z, int stencil);

    /// reset the light array
    virtual void ClearLights();
    /// remove a light
    virtual void ClearLight(int index);
    /// add a light to the light array (reset in BeginScene)
    virtual int AddLight(const nLight& light);
    /// set current mesh
    virtual void SetMesh(nMesh2* vbMesh, nMesh2* ibMesh);
    /// set current mesh array, clearing the single mesh
    virtual void SetMeshArray(nMeshArray* meshArray);
    /// set current shader
    virtual void SetShader(nShader2* shader);
    /// set transform
    virtual void SetTransform(TransformType type, const matrix44& matrix);
    /// draw the current mesh with indexed primitives
    virtual void DrawIndexed(PrimitiveType primType);
    /// draw the current mesh witn non-indexed primitives
    virtual void Draw(PrimitiveType primType);
    /// render indexed primitives without applying shader state (NS == No Shader)
    virtual void DrawIndexedNS(PrimitiveType primType);
    /// render non-indexed primitives without applying shader state (NS == No Shader)
    virtual void DrawNS(PrimitiveType primType);

    /// trigger the window system message pump
    virtual bool Trigger();

    /// draw text (immediately)
    virtual void DrawText(const nString& text, const vector4& color, const rectangle& rect, uint flags, bool immediate=true);
    /// get text extents
    virtual vector2 GetTextExtent(const nString& text);
    /// draw accumulated text buffer
    virtual void DrawTextBuffer();

    /// enter dialog box mode (display mode must have DialogBoxMode enabled!)
    virtual void EnterDialogBoxMode();
    /// leave dialog box mode
    virtual void LeaveDialogBoxMode();

    /// save a screen shot
    virtual bool SaveScreenshot(const char* fileName, nTexture2::FileFormat fileFormat);

    /// begin rendering lines
    virtual void BeginLines();
    /// draw 2d lines in screen space
    virtual void DrawLines2d(const vector2* vertexList, int numVertices, const vector4& color);
    /// finish line rendering
    virtual void EndLines();

    /// begin shape rendering (for debug visualizations)
    virtual void BeginShapes();
    /// draw a shape with the given model matrix with given color
    virtual void DrawShape(ShapeType type, const matrix44& model, const vector4& color);
    /// draw a shape without shader management
    virtual void DrawShapeNS(ShapeType type, const matrix44& model);
    /// draw direct primitives
    virtual void DrawShapePrimitives(PrimitiveType type, int numPrimitives, const vector3* vertexList, int vertexWidth, const matrix44& model, const vector4& color);
    /// draw direct indexed primitives (slow, use for debug visual visualization only!)
    virtual void DrawShapeIndexedPrimitives(PrimitiveType type, int numPrimitives, const vector3* vertexList, int numVertices, int vertexWidth, void* indices, IndexType indexType, const matrix44& model, const vector4& color);
    /// end shape rendering
    virtual void EndShapes();

    /// Access Direct3D Device directly. Might be Null!
    IDirect3DDevice9* GetD3DDevice() const;
    /// Access Direct3D object directly. Might be Null!
    IDirect3D9* GetD3D() const;
    /// adjust gamma.
    virtual void AdjustGamma();
    /// restore gamma.
    virtual void RestoreGamma();
    /// skip message loop in trigger
    virtual void SetSkipMsgLoop(bool skip);

private:
    /// initialize the text renderer
    void OpenTextRenderer();
    /// shutdown the text renderer
    void CloseTextRenderer();
    /// check for lost device, and reset if possible
    bool TestResetDevice();
    /// check a buffer format combination for compatibility
    bool CheckDepthFormat(D3DFORMAT adapterFormat, D3DFORMAT backbufferFormat, D3DFORMAT depthFormat);
    /// find the best possible buffer format combination
    void FindBufferFormats(nDisplayMode2::Bpp bpp, D3DFORMAT& dispFormat, D3DFORMAT& backFormat, D3DFORMAT& zbufFormat);
    /// open d3d
    void D3dOpen();
    /// close d3d
    void D3dClose();
    /// create the d3d device
    bool DeviceOpen();
    /// release the d3d device
    void DeviceClose();
    /// called before device destruction, or when device lost
    void OnDeviceCleanup(bool shutdown);
    /// called after device created or restored
    void OnDeviceInit(bool startup);
    /// initialize device default state
    void InitDeviceState();
    /// update the feature set member
    void UpdateFeatureSet();
    #ifdef __NEBULA_STATS__
    /// query the d3d resource manager and fill the watched variables
    void QueryStatistics();
    #endif
    /// get d3d primitive type and num primitives for indexed drawing
    int GetD3DPrimTypeAndNumIndexed(PrimitiveType primType, D3DPRIMITIVETYPE& d3dPrimType) const;
    /// get d3d primitive type and num primitives
    int GetD3DPrimTypeAndNum(PrimitiveType primType, D3DPRIMITIVETYPE& d3dPrimType) const;
    /// update the mouse cursor image and visibility
    void UpdateCursor();
    /// get a pointer to the global d3dx effect pool
    ID3DXEffectPool* GetEffectPool() const;
    /// instancer version of DrawIndexedNS()
    void DrawIndexedInstancedNS(PrimitiveType primType);
    /// instancer version of DrawNS()
    void DrawInstancedNS(PrimitiveType primType);
    /// update shared shader parameters per frame
    void UpdatePerFrameSharedShaderParams();
    /// update shared shader parameters per scene
    void UpdatePerSceneSharedShaderParams();
    /// return number of bits for a D3DFORMAT
    int GetD3DFormatNumBits(D3DFORMAT fmt);
    /// enable/disable software vertex processing
    void SetSoftwareVertexProcessing(bool b);
    /// get current software vertex processing state
    bool GetSoftwareVertexProcessing();
    /// initialize device identifier field
    void InitDeviceIdentifier();
    /// update the scissor rectangle in Direct3D
    void UpdateScissorRect();
    /// returns the current render target size in pixels
    vector2 GetCurrentRenderTargetSize() const;
    /// check for multisample AA compatibility
    D3DMULTISAMPLE_TYPE CheckMultiSampleType(D3DFORMAT backbufferFormat, D3DFORMAT depthFormat, bool windowed);
    /// draw text immediately
    void DrawTextImmediate(nFont2* font, const nString& text, const vector4& color, const rectangle& rect, uint flags);
    /// add nEnvs describing display modes (for all adapters) to the NOH
    void CreateDisplayModeEnvVars();

    friend class nD3D9Mesh;
    friend class nD3D9Texture;
    friend class nD3D9Shader;

    static nD3D9Server* Singleton;

    nD3D9WindowHandler windowHandler; ///< a Win32 window handler object

    DWORD deviceBehaviourFlags;     ///< the behaviour flags at device creation time
    D3DCAPS9 devCaps;               ///< device caps
    D3DDISPLAYMODE d3dDisplayMode;  ///< the current d3d display mode
    FeatureSet featureSet;

    class TextElement
    {
    public:
        /// default constructor
        TextElement();
        /// constructor
        TextElement(nFont2* f, const nString& t, const vector4& c, const rectangle& r, uint flg);
        
        nRef<nFont2> refFont;
        nString text;
        vector4 color;
        rectangle rect;
        uint flags;
    };
    nArray<TextElement> textElements;
    ID3DXSprite* d3dSprite;
    nRef<nFont2> refDefaultFont;

    ID3DXLine* d3dxLine;                        ///< line drawing stuff
    ID3DXMesh* shapeMeshes[NumShapeTypes];      ///< shape rendering
    nRef<nD3D9Shader> refShapeShader;           ///< the shader used for rendering shapes

    D3DPRESENT_PARAMETERS presentParams;        ///< current presentation parameters
    IDirect3DSurface9* backBufferSurface;       ///< the original back buffer surface
    IDirect3DSurface9* depthStencilSurface;     ///< the original depth stencil surface
    IDirect3DSurface9* captureSurface;          ///< an offscreen surface for fast screenshot capture
    ID3DXEffectPool* effectPool;                ///< global pool for shared effect parameters
    nRef<nD3D9Shader> refSharedShader;          ///< reference shader for shared effect parameters

    #ifdef __NEBULA_STATS__
    IDirect3DQuery9*   queryResourceManager;    ///< for quering the d3d resource manager
    nTime timeStamp;                            ///< time stamp for FPS computation
    // query watcher variables
/*
    nWatched dbgQueryTextureTrashing;
    nWatched dbgQueryTextureApproxBytesDownloaded;
    nWatched dbgQueryTextureNumEvicts;
    nWatched dbgQueryTextureNumVidCreates;
    nWatched dbgQueryTextureLastPri;
    nWatched dbgQueryTextureNumUsed;
    nWatched dbgQueryTextureNumUsedInVidMem;
    nWatched dbgQueryTextureWorkingSet;
    nWatched dbgQueryTextureWorkingSetBytes;
    nWatched dbgQueryTextureTotalManaged;
    nWatched dbgQueryTextureTotalBytes;
*/
    nWatched dbgQueryNumPrimitives;
    nWatched dbgQueryFPS;
    nWatched dbgQueryNumDrawCalls;
    nWatched dbgQueryNumRenderStateChanges;
    nWatched dbgQueryNumTextureChanges;

    int statsFrameCount;
    int statsNumTextureChanges;
    int statsNumRenderStateChanges;
    int statsNumDrawCalls;
    int statsNumPrimitives;
    #endif

public:
    // NOTE: this stuff is public because WinProcs may need to access it
    IDirect3DDevice9* d3d9Device;               ///< pointer to device object
    IDirect3D9* d3d9;                           ///< pointer to D3D9 object
};

//------------------------------------------------------------------------------
/**
*/
inline
nD3D9Server*
nD3D9Server::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
nD3D9Server::TextElement::TextElement() :
    flags(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nD3D9Server::TextElement::TextElement(nFont2* f, const nString& t, const vector4& c, const rectangle& r, uint flg) :
    refFont(f),
    text(t),
    color(c),
    rect(r),
    flags(flg)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nD3D9Server::GetD3DPrimTypeAndNumIndexed(PrimitiveType primType, D3DPRIMITIVETYPE& d3dPrimType) const
{
    int d3dNumPrimitives = 0;
    switch (primType)
    {
        case PointList:     
            d3dPrimType = D3DPT_POINTLIST;
            d3dNumPrimitives = this->indexRangeNum;
            break;

        case LineList:      
            d3dPrimType = D3DPT_LINELIST; 
            d3dNumPrimitives = this->indexRangeNum / 2;            
            break;

        case LineStrip:     
            d3dPrimType = D3DPT_LINESTRIP; 
            d3dNumPrimitives = this->indexRangeNum - 1;            
            break;

        case TriangleList:  
            d3dPrimType = D3DPT_TRIANGLELIST; 
            d3dNumPrimitives = this->indexRangeNum / 3;            
            break;

        case TriangleStrip: 
            d3dPrimType = D3DPT_TRIANGLESTRIP; 
            d3dNumPrimitives = this->indexRangeNum - 2;
            break;

        case TriangleFan:   
            d3dPrimType = D3DPT_TRIANGLEFAN; 
            d3dNumPrimitives = this->indexRangeNum - 2;
            break;
    }
    return d3dNumPrimitives;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nD3D9Server::GetD3DPrimTypeAndNum(PrimitiveType primType, D3DPRIMITIVETYPE& d3dPrimType) const
{
    int d3dNumPrimitives = 0;
    switch (primType)
    {
        case PointList:     
            d3dPrimType = D3DPT_POINTLIST;
            d3dNumPrimitives = this->vertexRangeNum;
            break;

        case LineList:      
            d3dPrimType = D3DPT_LINELIST; 
            d3dNumPrimitives = this->vertexRangeNum / 2;            
            break;

        case LineStrip:     
            d3dPrimType = D3DPT_LINESTRIP; 
            d3dNumPrimitives = this->vertexRangeNum - 1;            
            break;

        case TriangleList:  
            d3dPrimType = D3DPT_TRIANGLELIST; 
            d3dNumPrimitives = this->vertexRangeNum / 3;            
            break;

        case TriangleStrip: 
            d3dPrimType = D3DPT_TRIANGLESTRIP; 
            d3dNumPrimitives = this->vertexRangeNum - 2;
            break;

        case TriangleFan:   
            d3dPrimType = D3DPT_TRIANGLEFAN; 
            d3dNumPrimitives = this->vertexRangeNum - 2;
            break;
    }
    return d3dNumPrimitives;
}

//------------------------------------------------------------------------------
/**
*/
inline
HWND
nD3D9Server::GetParentHwnd() const
{
    return this->windowHandler.GetParentHwnd();
}

//------------------------------------------------------------------------------
/**
*/
inline
ID3DXEffectPool*
nD3D9Server::GetEffectPool() const
{
    return this->effectPool;
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DDevice9*
nD3D9Server::GetD3DDevice() const
{
    return this->d3d9Device;
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3D9*
nD3D9Server::GetD3D() const
{
    return this->d3d9;
}

//------------------------------------------------------------------------------
#endif
