#define N_IMPLEMENTS nD3D9Server
//------------------------------------------------------------------------------
//  nd3d9server_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "kernel/nenv.h"
#include "il/il.h"
#include "il/ilu.h"

nNebulaClass(nD3D9Server, "ngfxserver2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Server::nD3D9Server() :
    refInputServer(kernelServer, "/sys/servers/input"),
    hInst(0),
    hWnd(0),
    hAccel(0),
    windowedStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE),
    fullscreenStyle(WS_POPUP | WS_SYSMENU | WS_VISIBLE),
    deviceBehaviourFlags(0),
    windowOpen(false),
    windowMinimized(false),
    quitRequested(false),
    d3dFont(0),
    d3d9(0),
    d3d9Device(0),
    d3dPrimType(D3DPT_LINELIST),
    depthStencilSurface(0),
    backBufferSurface(0),
    dynMeshLocked(false),
    childStyle(WS_CHILD | WS_TABSTOP),
    parentHWnd(0)
{
    memset(&(this->devCaps), 0, sizeof(this->devCaps));

    // get applications module handle
    this->hInst = GetModuleHandle(0);

    // open the app window
    this->WindowOpen();

    // publish the window handle
    nEnv *env;
    if ((env = (nEnv *) kernelServer->New("nenv","/sys/env/hwnd"))) 
    {
        env->SetI((int)this->hWnd);
    }

    // initialize DevIL
    ilInit();
    iluInit();

    memset(&(this->presentParams), 0, sizeof(this->presentParams));
    this->D3dOpen();
}
  
//------------------------------------------------------------------------------
/**
*/
nD3D9Server::~nD3D9Server()
{
    // shut down all
    if (this->displayOpen)
    {
        this->CloseDisplay();
    }
    this->D3dClose();
    ilShutDown();
    this->WindowClose();
    n_assert(this->textNodeList.IsEmpty());
    n_assert(0 == this->d3dFont);
}

//------------------------------------------------------------------------------
/**
    Initialize Direct3D.
*/
void
nD3D9Server::D3dOpen()
{
    n_assert(0 == this->d3d9);

    this->d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!this->d3d9)
    {
        n_error("nD3D9Server: could not initialize Direct3D!\n");
    }
}

//------------------------------------------------------------------------------
/**
    Shutdown Direct3D.
*/
void
nD3D9Server::D3dClose()
{
    n_assert(this->d3d9);
    n_assert(0 == this->d3d9Device);

    int refCount = this->d3d9->Release();
    if (0 < refCount) 
    {
        n_printf("WARNING: Direct3D9 interface was still referenced (count = %d)\n", refCount);
    }
    this->d3d9 = 0;
}

//-----------------------------------------------------------------------------
/**
    Open the display.
*/
bool
nD3D9Server::OpenDisplay()
{
    n_assert(!this->displayOpen);
    if (this->DeviceOpen())
    {
        nGfxServer2::OpenDisplay();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Close the display.
*/
void
nD3D9Server::CloseDisplay()
{
    n_assert(this->displayOpen);
    this->DeviceClose();
    nGfxServer2::CloseDisplay();
}

//-----------------------------------------------------------------------------
/**
    Implements the Windows message pump. Must be called once a
    frame OUTSIDE of BeginScene() / EndScene().

    @return     false if nD3D9Server requests to shutdown the application
*/
bool
nD3D9Server::Trigger()
{
    // handle all pending WM's
    MSG msg;
    // if exist parent window, this window is in child mode
    if (this->displayMode.GetType() == nDisplayMode2::CHILDWINDOWED)
    {
        if (PeekMessage(&msg,NULL,WM_SIZE,WM_SIZE,PM_NOREMOVE))
        {
            int w = LOWORD(msg.lParam);
            int h = HIWORD(msg.lParam);
        }
    }
    while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
    {
        if (GetMessage(&msg, NULL, 0, 0)) 
        {
            int msgHandled = false;
            if (this->hWnd && this->hAccel) 
            {
                msgHandled = TranslateAccelerator(this->hWnd, this->hAccel, &msg);
            }
            if (!msgHandled) 
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    nGfxServer2::Trigger();

    return (!this->quitRequested);
}

//-----------------------------------------------------------------------------
/**
    Create screen shot and save it to given filename. (.bmp file)

    @param fileName filename for screen shot.
*/
bool
nD3D9Server::SaveScreenshot(const char* fileName)
{
    n_assert(fileName);
    HRESULT hr;

    // get adapter number and device window
    D3DDEVICE_CREATION_PARAMETERS dcp;
    this->d3d9Device->GetCreationParameters(&dcp);

    // get width and height for the front buffer surface
    D3DDISPLAYMODE dispMode;
    hr = this->d3d9->GetAdapterDisplayMode(dcp.AdapterOrdinal, &dispMode);
    if (FAILED(hr))
    {
        n_printf("nD3D9Server::Screenshot(): Failed to get 'adapter display mode'!\n");
        return false;
    }

    // create the front buffer surface to save for screenshot. 
    IDirect3DSurface9* surf;
    hr = this->d3d9Device->CreateOffscreenPlainSurface(dispMode.Width, dispMode.Height, 
                                                       D3DFMT_A8R8G8B8,
                                                       D3DPOOL_SCRATCH,
                                                       &surf,
                                                       NULL);
    if (FAILED(hr))
    {
        n_printf("nD3D9Server::Screenshot(): Failed to 'create offscreen plain surface'!\n");
        return false;
    }

    // get a copy of the front buffer surface.
    this->d3d9Device->GetFrontBufferData(0, surf);

    // get the rectangle into which rendering is drawn
    // it's the client rectangle of the focus window in screen coordinates
    RECT rc;
    GetClientRect(dcp.hFocusWindow, &rc);
    ClientToScreen(dcp.hFocusWindow, LPPOINT(&rc.left));
    ClientToScreen(dcp.hFocusWindow, LPPOINT(&rc.right));

    // save the front buffer surface to given filename.
    hr = D3DXSaveSurfaceToFile(fileName, D3DXIFF_BMP, surf, 0, &rc);
    if (FAILED(hr))
    {
        n_printf("nD3D9Server::Screenshot(): Failed to save file '%s'!\n", fileName);
        return false;
    }

    return true;
}
