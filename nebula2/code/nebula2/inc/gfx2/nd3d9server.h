#ifndef N_D3D9SERVER_H
#define N_D3D9SERVER_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Server
    @ingroup NebulaD3D9GraphicsSystem

    D3D9 based gfx server.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_GFXSERVER2_H
#include "gfx2/ngfxserver2.h"
#endif

#ifndef N_INPUTSERVER_H
#include "input/ninputserver.h"
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include "gfx2/d3dfont9.h"

#undef N_DEFINES
#define N_DEFINES nD3D9Server
#include "kernel/ndefdllclass.h"

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
    /// create a render target object
    virtual nTexture2* NewRenderTarget(const char* rsrcName, int width, int height, nTexture2::Format fmt, bool hasColor, bool hasDepth, bool hasStencil);
    /// claim access rights to dynamic mesh
    virtual nMesh2* LockDynamicMesh();
    /// give up access to dynamic mesh
    virtual void UnlockDynamicMesh(nMesh2* dynMesh);

    /// set the current window title
    virtual void SetWindowTitle(const char* title);
    /// set the current camera description
    virtual void SetCamera(const nCamera2& cam);
    /// open the display
    virtual bool OpenDisplay();
    /// close the display
    virtual void CloseDisplay();

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
    /// draw the current mesh, texture and shader
    virtual void Draw();

    /// trigger the window system message pump
    virtual bool Trigger();
    /// draw 2d text (will be buffered until end of frame)
    virtual void Text(const char* text, float x, float y);
    /// draw the text buffer
    virtual void DrawTextBuffer();

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
    void UnloadResources();
    /// reload resource data (call when device restored)
    void ReloadResources();
    /// initialize device default state
    void InitDeviceState();
    /// return true if device is in software vertex processing mode
    bool GetSoftwareVertexProcessing() const;

    friend class nD3D9Mesh;
    friend class nD3D9Texture;
    friend class nD3D9Shader;

    enum 
    {                          
        DYNAMIC_VERTEXBUFFER_SIZE = (1<<16),    // number of floats in buffer
        DYNAMIC_INDEXBUFFER_SIZE  = (1<<16),    // number of indices in buffer
    };

    HINSTANCE hInst;
    HWND      hWnd;
    HACCEL    hAccel;
    DWORD windowedStyle;            // WS_* flags for windowed mode
    DWORD fullscreenStyle;          // WS_* flags for fullscreen mode
    DWORD deviceBehaviourFlags;     // the behaviour flags at device creation time
    D3DCAPS9 devCaps;               // device caps
    bool windowOpen;                // window has been opened

    class TextNode : public nNode
    {
    public:
        /// constructor
        TextNode(const char* str, float x, float y);
        nString string;
        float xpos;
        float ypos;
    };
    nList textNodeList;
    CD3DFont9* d3dFont;

    D3DPRESENT_PARAMETERS presentParams;        // current presentation parameters
    D3DPRIMITIVETYPE d3dPrimType;
    IDirect3DSurface9* backBufferSurface;       // the original back buffer surface
    IDirect3DSurface9* depthStencilSurface;     // the original depth stencil surface
    nRef<nMesh2> refDynMesh;                    // the global dynamic mesh
    bool dynMeshLocked;                         // is the dynamic mesh currently owned by some client?

public:
    enum
    {
        ACCEL_TOGGLEFULLSCREEN = 1001,
    };

    // NOTE: this stuff is public because WinProcs may need to access it
    IDirect3DDevice9* d3d9Device;               // pointer to device object
    IDirect3D9* d3d9;                           // pointer to D3D9 object

    /// translate win32 keycode into Nebula keycode
    nKey TranslateKey(int vkey);

    nAutoRef<nInputServer> refInputServer;
    bool windowMinimized;           // window is currently minimized
    bool quitRequested;             // quit requested by WinProc()
};

//------------------------------------------------------------------------------
/**
*/
inline
nD3D9Server::TextNode::TextNode(const char* str, float x, float y) :
    string(str),
    xpos(x),
    ypos(y)
{
    // empty
}

//------------------------------------------------------------------------------
#endif
