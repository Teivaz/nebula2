//------------------------------------------------------------------------------
//  nd3d9server_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nD3D9Server, "ngfxserver2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Server::nD3D9Server() :
    #ifdef __NEBULA_STATS__
    timeStamp(0.0),
    queryResourceManager(0),
    dbgQueryTextureTrashing("gfxTexTrashing", nArg::Bool),
    dbgQueryTextureApproxBytesDownloaded("gfxTexApproxBytesDownloaded", nArg::Int),
    dbgQueryTextureNumEvicts("gfxTexNumEvicts", nArg::Int),
    dbgQueryTextureNumVidCreates("gfxTexNumVidCreates", nArg::Int),
    dbgQueryTextureLastPri("gfxTexLastPri", nArg::Int),
    dbgQueryTextureNumUsed("gfxTexNumUsed", nArg::Int),
    dbgQueryTextureNumUsedInVidMem("gfxTexNumUsedInVidMem", nArg::Int),
    dbgQueryTextureWorkingSet("gfxTexWorkingSet", nArg::Int),
    dbgQueryTextureWorkingSetBytes("gfxTexWorkingSetBytes", nArg::Int),
    dbgQueryTextureTotalManaged("gfxTexTotalManaged", nArg::Int),
    dbgQueryTextureTotalBytes("gfxTexTotalBytes", nArg::Int),
    dbgQueryNumPrimitives("gfxNumPrimitives", nArg::Int),
    dbgQueryFPS("gfxFPS", nArg::Float),
    dbgQueryNumDrawCalls("gfxNumDrawCalls", nArg::Int),
    dbgQueryNumRenderStateChanges("gfxNumRenderStateChanges", nArg::Int),
    dbgQueryNumTextureChanges("gfxNumTextureChanges", nArg::Int),
    #endif
    windowHandler(this),
	deviceBehaviourFlags(0),
    d3dSprite(0),
    d3d9(0),
    d3d9Device(0),
    depthStencilSurface(0),
    backBufferSurface(0),
    featureSet(InvalidFeatureSet),
    d3dxEffectPool(0)
{
    memset(&(this->devCaps), 0, sizeof(this->devCaps));
    memset(&(this->presentParams), 0, sizeof(this->presentParams));

    // open the app window
    this->windowHandler.OpenWindow();

    // initialize Direct3D
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
    this->windowHandler.CloseWindow();
    n_assert(this->textNodeList.IsEmpty());
}

//------------------------------------------------------------------------------
/**
    Initialize Direct3D.
*/
void
nD3D9Server::D3dOpen()
{
    n_assert(0 == this->d3d9);

    // create the d3d object
    this->d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!this->d3d9)
    {
        n_error("nD3D9Server: could not initialize Direct3D!\n");
    }

    // create the global effect parameter pool
    HRESULT hr = D3DXCreateEffectPool(&(this->d3dxEffectPool));
    n_assert(SUCCEEDED(hr));
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
    n_assert(this->d3dxEffectPool);

    // release the global effect parameter pool
    this->d3dxEffectPool->Release();
    this->d3dxEffectPool = 0;

    // release the d3d object
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
    return this->windowHandler.Trigger();
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

    // mangle filename
    char mangledPath[N_MAXPATH];
    kernelServer->GetFileServer()->ManglePath(fileName, mangledPath, sizeof(mangledPath));

    // save the front buffer surface to given filename.
    hr = D3DXSaveSurfaceToFile(mangledPath, D3DXIFF_BMP, surf, 0, &rc);
    if (FAILED(hr))
    {
        n_printf("nD3D9Server::Screenshot(): Failed to save file '%s'!\n", fileName);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Enter dialog box mode.
*/
void
nD3D9Server::EnterDialogBoxMode()
{
    n_assert(this->windowHandler.GetDisplayMode().GetDialogBoxMode());
    n_assert(this->d3d9Device);
    nGfxServer2::EnterDialogBoxMode();
    HRESULT hr = this->d3d9Device->SetDialogBoxMode(TRUE);
    n_assert(SUCCEEDED(hr));
}

//-----------------------------------------------------------------------------
/**
    Leave dialog box mode.
*/
void
nD3D9Server::LeaveDialogBoxMode()
{
    n_assert(this->windowHandler.GetDisplayMode().GetDialogBoxMode());
    n_assert(this->d3d9Device);
    nGfxServer2::LeaveDialogBoxMode();
    HRESULT hr = this->d3d9Device->SetDialogBoxMode(FALSE);
    n_assert(SUCCEEDED(hr));
}

//-----------------------------------------------------------------------------
/**
    Set the current display mode. This will not take effect until
    OpenDisplay() has been called!
*/
void
nD3D9Server::SetDisplayMode(const nDisplayMode2& mode)
{
    this->windowHandler.SetDisplayMode(mode);
}

//-----------------------------------------------------------------------------
/**
    Get the current display mode.
*/
const nDisplayMode2&
nD3D9Server::GetDisplayMode() const
{
    return this->windowHandler.GetDisplayMode();
}

