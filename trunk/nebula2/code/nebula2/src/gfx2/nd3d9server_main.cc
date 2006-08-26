//------------------------------------------------------------------------------
//  nd3d9server_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"
#include "gfx2/nd3d9texture.h"

nNebulaClass(nD3D9Server, "ngfxserver2");
nD3D9Server* nD3D9Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nD3D9Server::nD3D9Server() :
    #ifdef __NEBULA_STATS__
    timeStamp(0.0),
    queryResourceManager(0),
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
    captureSurface(0),
    effectPool(0),
    featureSet(InvalidFeatureSet),
    textElements(64, 64),
#if __NEBULA_STATS__
    statsFrameCount(0),
    statsNumTextureChanges(0),
    statsNumRenderStateChanges(0),
    statsNumDrawCalls(0),
    statsNumPrimitives(0),
#endif
    d3dxLine(0)
{
    n_assert(0 == Singleton);
    Singleton = this;

    memset(&(this->devCaps), 0, sizeof(this->devCaps));
    memset(&(this->presentParams), 0, sizeof(this->presentParams));
    memset(&(this->shapeMeshes), 0, sizeof(this->shapeMeshes));

    // HACK:
    // we used to open the window here, but we now do it lazily in OpenDisplay()
    // to give the calling app the chance to set (e.g.) the window's name and
    // icon before it is opened.

    // initialize Direct3D
    this->D3dOpen();

    // initialize the device identifier
    this->InitDeviceIdentifier();
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
    if (this->windowHandler.IsWindowOpen())
    {
        this->windowHandler.CloseWindow();
    }

    n_assert(0 != Singleton);
    Singleton = 0;
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

    // update the feature set
    this->UpdateFeatureSet();
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
    if (!this->windowHandler.IsWindowOpen())
    {
        // lazy initialization: open the app window
        // don't do this in the constructor because the window's name and icon
        // won't have been set at that time.
        this->windowHandler.OpenWindow();
    }

    if (this->DeviceOpen())
    {
        nGfxServer2::OpenDisplay();

        // clear display
        if (this->BeginFrame())
        {
            if (this->BeginScene())
            {
                this->Clear(AllBuffers, 0.0f, 0.0f, 0.0f, 1.0f, 1.0, 0);
                this->EndScene();
                this->PresentScene();
            }
            this->EndFrame();
        }
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
    Create screen shot and save it to given filename. (.jpg file)

    @param  fileName    filename for screen shot.

    - 25-Apr-05 floh    rewritten for performance
*/
bool
nD3D9Server::SaveScreenshot(const char* fileName, nTexture2::FileFormat fileFormat)
{
    n_assert(fileName);
    n_assert(this->d3d9Device);
    HRESULT hr;

    // get mangled path name
    nString mangledPath = nFileServer2::Instance()->ManglePath(fileName);

    // copy back buffer to offscreen surface
    hr = this->d3d9Device->GetRenderTargetData(this->backBufferSurface, this->captureSurface);
    n_assert(SUCCEEDED(hr));

    // save image
    D3DXIMAGE_FILEFORMAT d3dxFormat = nD3D9Texture::FileFormatToD3DX(fileFormat);
    hr = D3DXSaveSurfaceToFile(mangledPath.Get(), d3dxFormat, this->captureSurface, NULL, NULL);
    n_assert(SUCCEEDED(hr));

    // all ok
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
    HRESULT hr;

    nGfxServer2::EnterDialogBoxMode();

    // reset the device with lockable backbuffer flag
    this->OnDeviceCleanup(false);
    D3DPRESENT_PARAMETERS p = this->presentParams;
    p.MultiSampleType = D3DMULTISAMPLE_NONE;
    p.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    hr = this->d3d9Device->Reset(&p);
    this->InitDeviceState();
    this->OnDeviceInit(false);

    hr = this->d3d9Device->SetDialogBoxMode(TRUE);
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

    // only reset the device if it is currently valid
    hr = this->d3d9Device->TestCooperativeLevel();
    if (SUCCEEDED(hr))
    {
        this->OnDeviceCleanup(false);
        hr = this->d3d9Device->Reset(&this->presentParams);
        this->InitDeviceState();
        this->OnDeviceInit(false);
    }
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

//-----------------------------------------------------------------------------
/**
*/
void
nD3D9Server::SetWindowTitle(const char* title)
{
    this->windowHandler.SetWindowTitle(title);
}

//-----------------------------------------------------------------------------
/**
    Return true when vertex shaders are running in emulation.
*/
bool
nD3D9Server::AreVertexShadersEmulated()
{
    n_assert(this->d3d9Device);
#if N_D3D9_FORCEMIXEDVERTEXPROCESSING
    return true;
#else
    if (DX7 == this->GetFeatureSet())
    {
        return true;
    }
    else
    {
        return 0 == (this->deviceBehaviourFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING);
    }
#endif
}

//------------------------------------------------------------------------------
/**
    - 24-Nov-04  kims added
*/
void
nD3D9Server::SetSkipMsgLoop(bool skip)
{
    this->windowHandler.SetSkipMsgLoop(skip);
}
