#ifndef N_D3D9SERVER_H
#define N_D3D9SERVER_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Server
    @ingroup NebulaD3D9GraphicsSystem

    D3D9 based gfx server.

    (C) 2002 RadonLabs GmbH
*/
#include "gfx2/ngfxserver2.h"
#include "input/ninputserver.h"
#include "misc/nwatched.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d9.h>
#include <d3dx9.h>

#if D3D_SDK_VERSION < 31
#error You must be using the DirectX 9 Summer Update SDK!  You may download it from http://www.microsoft.com/downloads/details.aspx?FamilyId=9216652F-51E0-402E-B7B5-FEB68D00F298&displaylang=en
#endif

//------------------------------------------------------------------------------
//  Debugging definitions (for shader debugging etc...)
//------------------------------------------------------------------------------
#define N_D3D9_DEBUG (0)
#define N_D3D9_DEVICETYPE D3DDEVTYPE_HAL

//------------------------------------------------------------------------------
class nD3D9Server : public nGfxServer2
{
public:
    /// constructor
    nD3D9Server();
    /// destructor
    virtual ~nD3D9Server();

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

    /// set the current camera description
    virtual void SetCamera(nCamera2& cam);
    /// open the display
    virtual bool OpenDisplay();
    /// close the display
    virtual void CloseDisplay();
    /// get the best supported feature set
    virtual FeatureSet GetFeatureSet();
    /// parent window handle
    virtual int GetParentHWnd();

    /// set a new render target texture
    virtual void SetRenderTarget(nTexture2* t);
    
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
    /// set current texture
    virtual void SetTexture(int stage, nTexture2* tex);
    /// set current shader
    virtual void SetShader(nShader2* shader);
    /// draw the current mesh with indexed primitives
    virtual void DrawIndexed(nPrimitiveType primType);
    /// draw the current mesh witn non-indexed primitives
    virtual void Draw(nPrimitiveType primType);
    /// render indexed primitives without applying shader state (NS == No Shader)
    virtual void DrawIndexedNS(nPrimitiveType primType);
    /// render non-indexed primitives without applying shader state (NS == No Shader)
    virtual void DrawNS(nPrimitiveType primType);

    /// trigger the window system message pump
    virtual bool Trigger();
    /// draw 2d text (will be buffered until end of frame)
    virtual void Text(const char* text, const vector4& color, float x, float y);
    /// draw the text buffer
    virtual void DrawTextBuffer();

    /// draw text (immediately)
    virtual void DrawText(const char* text, const vector4& color, float xPos, float yPos);
    /// get text extents
    virtual vector2 GetTextExtent(const char* text);

    /// save a screen shot
    virtual bool SaveScreenshot(const char*);

    static nKernelServer* kernelServer;

private:
    /// open the window
    bool WindowOpen();
    /// close the window
    void WindowClose();
    /// prepare window for switching between windowd/fullscreen mode
    void AdjustWindowForChange();
    /// restore window from minimized state after d3d device has been created
    void RestoreWindow();
    /// minimize window when d3d device is destroyed
    void MinimizeWindow();
    /// initialize the text renderer
    void OpenTextRenderer();
    /// shutdown the text renderer
    void CloseTextRenderer();
    /// check for lost device, and reset if possible
    bool TestResetDevice();
    /// check a buffer format combination for compatibility
    bool CheckDepthFormat(D3DFORMAT adapterFormat, D3DFORMAT backbufferFormat, D3DFORMAT depthFormat);
    /// find the best possible buffer format combination
    void FindBufferFormats(D3DFORMAT& dispFormat, D3DFORMAT& backFormat, D3DFORMAT& zbufFormat);
    /// open d3d
    void D3dOpen();
    /// close d3d
    void D3dClose();
    /// create the d3d device
    bool DeviceOpen();
    /// release the d3d device
    void DeviceClose();
    /// unload resource data (call when device lost)
    void OnDeviceLost();
    /// reload resource data (call when device restored)
    void OnRestoreDevice();
    /// initialize device default state
    void InitDeviceState();
    /// return true if device is in software vertex processing mode
    bool GetSoftwareVertexProcessing() const;
    /// update the feature set member
    void UpdateFeatureSet();
    #ifdef __NEBULA_STATS__
    /// query the d3d resource manager and fill the watched variables
    void QueryStatistics();
    #endif
    /// get d3d primitive type and num primitives for indexed drawing
    int GetD3DPrimTypeAndNumIndexed(nPrimitiveType primType, D3DPRIMITIVETYPE& d3dPrimType) const;
    /// get d3d primitive type and num primitives
    int GetD3DPrimTypeAndNum(nPrimitiveType primType, D3DPRIMITIVETYPE& d3dPrimType) const;
    /// update the mouse cursor image and visibility
    void UpdateCursor();

    friend class nD3D9Mesh;
    friend class nD3D9Texture;
    friend class nD3D9Shader;

    HINSTANCE hInst;
    HWND      hWnd;                 ///< handle of this window
    HWND      parentHWnd;           ///< handle of parent window  (child mode)

    HACCEL    hAccel;
    DWORD windowedStyle;            ///< WS_* flags for windowed mode
    DWORD childStyle;               ///< WS_* flags for child mode
    DWORD fullscreenStyle;          ///< WS_* flags for fullscreen mode
    DWORD deviceBehaviourFlags;     ///< the behaviour flags at device creation time
    D3DCAPS9 devCaps;               ///< device caps
    D3DDISPLAYMODE d3dDisplayMode;  ///< the current d3d display mode
    bool windowOpen;                ///< window has been opened
    FeatureSet featureSet;

    class TextNode : public nNode
    {
    public:
        /// constructor
        TextNode(const char* str, const vector4& clr, float x, float y);
        nString string;
        vector4 color;
        float xpos;
        float ypos;
    };
    nList textNodeList;
    ID3DXSprite* d3dSprite;
    nRef<nFont2> refDefaultFont;

    D3DPRESENT_PARAMETERS presentParams;        ///< current presentation parameters
    IDirect3DSurface9* backBufferSurface;       ///< the original back buffer surface
    IDirect3DSurface9* depthStencilSurface;     ///< the original depth stencil surface
    ID3DXEffectPool* d3dxEffectPool;            ///< pool for sharing shader parameters

    #ifdef __NEBULA_STATS__
    IDirect3DQuery9*   queryResourceManager;    ///< for quering the d3d resource manager
    nTime timeStamp;                            ///< time stamp for FPS computation
    // query watcher variables
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
    nWatched dbgQueryNumPrimitives;
    nWatched dbgQueryFPS;
    nWatched dbgQueryNumDrawCalls;
    nWatched dbgQueryNumRenderStateChanges;
    nWatched dbgQueryNumTextureChanges;

    int statsNumTextureChanges;
    int statsNumRenderStateChanges;
    #endif
    
public:
    enum
    {
        ACCEL_TOGGLEFULLSCREEN = 1001,
    };

    // NOTE: this stuff is public because WinProcs may need to access it
    IDirect3DDevice9* d3d9Device;               ///< pointer to device object
    IDirect3D9* d3d9;                           ///< pointer to D3D9 object

    /// translate win32 keycode into Nebula keycode
    nKey TranslateKey(int vkey);

    nAutoRef<nInputServer> refInputServer;
    bool windowMinimized;           ///< window is currently minimized
    bool quitRequested;             ///< quit requested by WinProc()
};

//------------------------------------------------------------------------------
/**
*/
inline
nD3D9Server::TextNode::TextNode(const char* str, const vector4& clr, float x, float y) :
    string(str),
    color(clr),
    xpos(x),
    ypos(y)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nD3D9Server::GetParentHWnd()
{
    return (int)this->parentHWnd;
}
//------------------------------------------------------------------------------
/**
*/
inline
int
nD3D9Server::GetD3DPrimTypeAndNumIndexed(nPrimitiveType primType, D3DPRIMITIVETYPE& d3dPrimType) const
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
nD3D9Server::GetD3DPrimTypeAndNum(nPrimitiveType primType, D3DPRIMITIVETYPE& d3dPrimType) const
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
#endif
