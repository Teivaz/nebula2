//------------------------------------------------------------------------------
//  nd3d9server_device.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9texture.h"
#include "kernel/ntimeserver.h"

//------------------------------------------------------------------------------
/**
    Initialize the default state of the device. Must be called after
    creating or resetting the d3d device.
*/
void
nD3D9Server::InitDeviceState()
{
    n_assert(this->d3d9Device);

    // update the viewport rectangle
    D3DVIEWPORT9 dvp;
    this->d3d9Device->GetViewport(&dvp);
    this->viewport.x      = (float) dvp.X;
    this->viewport.y      = (float) dvp.Y;
    this->viewport.width  = (float) dvp.Width;
    this->viewport.height = (float) dvp.Height;
    this->viewport.nearz  = dvp.MinZ;
    this->viewport.farz   = dvp.MaxZ;

    // update the projection matrix
    this->SetCamera(this->GetCamera());

    // set initial renderstates
    this->d3d9Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    this->d3d9Device->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    this->d3d9Device->SetRenderState(D3DRS_LIGHTING, FALSE);

    // update the device caps
    this->d3d9Device->GetDeviceCaps(&(this->devCaps));
}

//------------------------------------------------------------------------------
/**
    Helper method to verify a given pixelformat combination for the frame
    and depth buffer.
*/
bool
nD3D9Server::CheckDepthFormat(D3DFORMAT adapterFormat,
                              D3DFORMAT backbufferFormat,
                              D3DFORMAT depthFormat)
{
    n_assert(this->d3d9);

    HRESULT hr;

    // verify that the depth format exists
    hr = this->d3d9->CheckDeviceFormat(D3DADAPTER_DEFAULT,
                                       N_D3D9_DEVICETYPE,
                                       adapterFormat,
                                       D3DUSAGE_DEPTHSTENCIL,
                                       D3DRTYPE_SURFACE,
                                       depthFormat);
    if (FAILED(hr))
    {
        return false;
    }

    // check if compatible with display mode
    hr = this->d3d9->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,
                                            N_D3D9_DEVICETYPE,
                                            adapterFormat,
                                            backbufferFormat,
                                            depthFormat);
    return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
/**
    Returns the number of bits in a d3d format.
*/
int
nD3D9Server::GetD3DFormatNumBits(D3DFORMAT fmt)
{
    switch (fmt)
    {
        case D3DFMT_R8G8B8:
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_G16R16:
        case D3DFMT_A4L4:
        case D3DFMT_X8L8V8U8:
        case D3DFMT_Q8W8V8U8:
        case D3DFMT_V16U16:
        case D3DFMT_A2B10G10R10:
        case D3DFMT_A2W10V10U10:
            return 32;

        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_A8R3G3B2:
        case D3DFMT_X4R4G4B4:
        case D3DFMT_A8P8:
        case D3DFMT_A8L8:
        case D3DFMT_V8U8:
        case D3DFMT_L6V5U5:
            return 16;

        case D3DFMT_P8:
        case D3DFMT_A8:
        case D3DFMT_L8:
        case D3DFMT_R3G3B2:
            return 8;

        default:
            return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Find the best possible combination of buffer formats. The method will
    fail hard if no matching buffer formats could be found!
*/
void
nD3D9Server::FindBufferFormats(nDisplayMode2::Bpp bpp, D3DFORMAT& dispFormat, D3DFORMAT& backFormat, D3DFORMAT& zbufFormat)
{
    HRESULT hr;

    // table of valid pixel format combinations
    // { adapterFormat, backbufferFormat, zbufferFormat }
    D3DFORMAT formatTable[][3] = {
        { D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_D24S8 },
        { D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_D24X4S4 },
        { D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_D24X8 },
        { D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_D16 },
        { D3DFMT_R5G6B5,   D3DFMT_R5G6B5,   D3DFMT_D16 },
        { D3DFMT_UNKNOWN,  D3DFMT_UNKNOWN,  D3DFMT_UNKNOWN },
    };

    if (this->windowHandler.GetDisplayMode().GetType() == nDisplayMode2::Fullscreen)
    {
        // find fullscreen buffer formats
        dispFormat = D3DFMT_UNKNOWN;
        backFormat = D3DFMT_UNKNOWN;
        zbufFormat = D3DFMT_UNKNOWN;
        int i;
        for (i = 0; formatTable[i][0] != D3DFMT_UNKNOWN; i++)
        {
            dispFormat = formatTable[i][0];
            backFormat = formatTable[i][1];
            zbufFormat = formatTable[i][2];

            // skip 32 bit formats?
            if ((nDisplayMode2::Bpp16 == bpp) && (this->GetD3DFormatNumBits(formatTable[i][0]) > 16))
            {
                continue;
            }
            else if (this->CheckDepthFormat(dispFormat, backFormat, zbufFormat))
            {
                break;
            }
        }
    }
    else
    {
        // find windowed mode buffer format, the display and backbuffer formats
        // are defined by the current desktop color depth
        D3DDISPLAYMODE desktopMode;
        hr = this->d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &desktopMode);
        n_dxtrace(hr, "GetAdapterDisplayMode() failed.");

        dispFormat = desktopMode.Format;
        switch (dispFormat)
        {
            case D3DFMT_R8G8B8:
            case D3DFMT_A8R8G8B8:
            case D3DFMT_X8R8G8B8:
                backFormat = D3DFMT_X8R8G8B8;
                break;

            default:
                backFormat = dispFormat;
                break;
        }
        int i;
        for (i = 0; formatTable[i][0] != D3DFMT_UNKNOWN; i++)
        {
            zbufFormat = formatTable[i][2];
            if (this->CheckDepthFormat(dispFormat, backFormat, zbufFormat))
            {
                break;
            }
        }
    }

    if (D3DFMT_UNKNOWN == zbufFormat)
    {
        n_error("nD3D9Server: No valid Direct3D display format found!\n");
    }
}

//------------------------------------------------------------------------------
/**
    Update the feature set member.
*/
void
nD3D9Server::UpdateFeatureSet()
{
    // get d3d device caps
    HRESULT hr = this->d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, N_D3D9_DEVICETYPE, &(this->devCaps));
    n_dxtrace(hr, "GetDeviceCaps() in nD3D9Server::UpdateFeatureSet() failed!");
    if (this->devCaps.VertexShaderVersion >= D3DVS_VERSION(2, 0))
    {
        // check if floating point textures are available as render target
        HRESULT hr = this->d3d9->CheckDeviceFormat(D3DADAPTER_DEFAULT,
                                                   N_D3D9_DEVICETYPE,
                                                   D3DFMT_X8R8G8B8,
                                                   D3DUSAGE_RENDERTARGET,
                                                   D3DRTYPE_TEXTURE,
                                                   D3DFMT_R32F);
        if (SUCCEEDED(hr))
        {
            this->featureSet = DX9FLT;
        }
        else
        {
            this->featureSet = DX9;
        }
    }
    else if (this->devCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
    {
        // check if depth buffers are supported as textures
        HRESULT hr = this->d3d9->CheckDeviceFormat(D3DADAPTER_DEFAULT,
                                                   N_D3D9_DEVICETYPE,
                                                   D3DFMT_X8R8G8B8,
                                                   0,
                                                   D3DRTYPE_TEXTURE,
                                                   D3DFMT_D24S8);
        if (SUCCEEDED(hr))
        {
            this->featureSet = DX8SB;
        }
        else
        {
            this->featureSet = DX8;
        }
    }
    else
    {
        this->featureSet = DX7;
    }
    n_printf("nD3D9Server detected feature set '%s' (pre-override)\n", nGfxServer2::FeatureSetToString(this->featureSet));
}

//------------------------------------------------------------------------------
/**
    Open and initialize the d3d device, make the app window visible.
*/
bool
nD3D9Server::DeviceOpen()
{
    n_assert(this->d3d9);
    n_assert(!this->d3d9Device);
    n_assert(this->windowHandler.IsWindowOpen());
    n_assert(this->windowHandler.IsWindowMinimized());
    n_assert(!this->inBeginScene);
    n_assert(0 == this->d3dxLine);
    n_assert(0 == this->depthStencilSurface);
    n_assert(0 == this->backBufferSurface);
    #ifdef __NEBULA_STATS__
    n_assert(0 == this->queryResourceManager);
    #endif

    HRESULT hr;

    // prepare window...
    this->windowHandler.AdjustWindowForChange();

    // find a valid combination of buffer formats
    D3DFORMAT dispFormat;
    D3DFORMAT backFormat;
    D3DFORMAT zbufFormat;
    this->FindBufferFormats(this->GetDisplayMode().GetBpp(), dispFormat, backFormat, zbufFormat);

    // define device behaviour flags
    #ifdef __NEBULA_NO_THREADS__
    this->deviceBehaviourFlags = D3DCREATE_FPU_PRESERVE;
    #else
    this->deviceBehaviourFlags = D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED;
    #endif

    #if N_D3D9_DEBUG

        this->deviceBehaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        this->presentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        n_printf("nD3D9Server: using enforced software vertex processing\n");

    #else
        // check if hardware vertex shaders are supported, if not,
        // activate mixed vertex processing
        if (this->devCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            if (this->GetFeatureSet() >= DX9)
            {
                this->deviceBehaviourFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
                n_printf("nD3D9Server: using hardware vertex processing\n");

                // NOTE: do not use a pure device so that the D3D runtime
                // will filter redundant renderstate changes for us
                if (this->devCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
                {
                    this->deviceBehaviourFlags |= D3DCREATE_PUREDEVICE;
                }
            }
            else
            {
                this->deviceBehaviourFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
                n_printf("nD3D9Server: using mixed vertex processing\n");
            }
        }
        else
        {
            this->deviceBehaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            n_printf("nD3D9Server: using software vertex processing\n");
        }
        this->presentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    #endif

    // check for dialog box mode
    if (this->windowHandler.GetDisplayMode().GetDialogBoxMode())
    {
        this->presentParams.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    }

    // define presentation parameters
    if (this->windowHandler.GetDisplayMode().GetType() == nDisplayMode2::Fullscreen)
    {
        this->presentParams.BackBufferCount  = 1;
        this->presentParams.Windowed         = FALSE;
    }
    else
    {
        this->presentParams.BackBufferCount  = 1;
        this->presentParams.Windowed         = TRUE;
    }

    if (this->windowHandler.GetDisplayMode().GetVerticalSync())
    {
        this->presentParams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    }
    else
    {
        this->presentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    this->presentParams.BackBufferWidth                 = this->windowHandler.GetDisplayMode().GetWidth();
    this->presentParams.BackBufferHeight                = this->windowHandler.GetDisplayMode().GetHeight();
    this->presentParams.BackBufferFormat                = backFormat;
    this->presentParams.MultiSampleType                 = D3DMULTISAMPLE_NONE;
    this->presentParams.MultiSampleQuality              = 0;
    this->presentParams.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    this->presentParams.hDeviceWindow                   = this->windowHandler.GetHwnd();
    this->presentParams.EnableAutoDepthStencil          = TRUE;
    this->presentParams.AutoDepthStencilFormat          = zbufFormat;
    this->presentParams.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;

    // create d3d device
    hr = this->d3d9->CreateDevice(D3DADAPTER_DEFAULT,
                                  N_D3D9_DEVICETYPE,
                                  this->windowHandler.GetHwnd(),
                                  this->deviceBehaviourFlags,
                                  &(this->presentParams),
                                  &(this->d3d9Device));
    if (FAILED(hr))
    {
        n_error("nD3D9Server: Could not create d3d device!\nDirectX Error is: %s\n", DXGetErrorString9(hr));
        return false;
    }
    n_assert(this->d3d9Device);

    // create effect pool
    hr = D3DXCreateEffectPool(&this->effectPool);
    if (FAILED(hr))
    {
        n_error("nD3D9Server: Could not create effect pool!\nDirectX Error is: %s\n", DXGetErrorString9(hr));
        return false;
    }

    // fill display mode structure
    memset(&(this->d3dDisplayMode), 0, sizeof(this->d3dDisplayMode));
    hr = this->d3d9Device->GetDisplayMode(0, &(this->d3dDisplayMode));
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): GetDisplayMode() failed!");

    // create line object
    hr = D3DXCreateLine(this->d3d9Device, &this->d3dxLine);
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): D3DXCreateLine() failed!");

    // create shape objects
    hr = D3DXCreateBox(this->d3d9Device, 1.0f, 1.0f, 1.0f, &this->shapeMeshes[Box], NULL);
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): D3DXCreateBox() failed!");
    hr = D3DXCreateCylinder(this->d3d9Device, 1.0f, 1.0f, 1.0f, 18, 1, &this->shapeMeshes[Cylinder], NULL);
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): D3DXCreateCylinder() failed!");
    hr = D3DXCreateSphere(this->d3d9Device, 1.0f, 18, 6, &this->shapeMeshes[Sphere], NULL);
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): D3DXCreateSphere() failed!");
    hr = D3DXCreateTorus(this->d3d9Device, 1.0f, 0.5f, 18, 12, &this->shapeMeshes[Torus], NULL);
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): D3DXCreateTorus() failed!");
    hr = D3DXCreateTeapot(this->d3d9Device, &this->shapeMeshes[Teapot], NULL);
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): D3DXCreateTeapot() failed!");

    // reload any resources if necessary
    this->OnRestoreDevice();

    // initialize d3d device state
    this->InitDeviceState();

    // restore window
    this->windowHandler.RestoreWindow();

    this->CreateDisplayModeEnvVars();

    return true;
}

//------------------------------------------------------------------------------
/**
    Close the d3d device, minimize the app window.
*/
void
nD3D9Server::DeviceClose()
{
    n_assert(this->d3d9);
    n_assert(this->d3d9Device);
    n_assert(this->effectPool);
    n_assert(this->d3dxLine);
    n_assert(this->windowHandler.IsWindowOpen());
    n_assert(this->windowHandler.GetHwnd());

    // unload all resources
    this->OnDeviceLost(true);

    // destroy primitive shapes
    int i;
    for (i = 0; i < NumShapeTypes; i++)
    {
        if (this->shapeMeshes[i])
        {
            this->shapeMeshes[i]->Release();
            this->shapeMeshes[i] = 0;
        }
    }

    // destroy d3dx line object
    this->d3dxLine->Release();
    this->d3dxLine = 0;

    // destroy the effect pool
    this->effectPool->Release();
    this->effectPool = 0;

    // destroy d3d device
    this->d3d9Device->Release();
    this->d3d9Device = 0;

    // minimze the app window
    this->windowHandler.MinimizeWindow();
}

//------------------------------------------------------------------------------
/**
    Check for lost device, and try to reset the device.
*/
bool
nD3D9Server::TestResetDevice()
{
    n_assert(this->d3d9Device);
    HRESULT hr;

    hr = this->d3d9Device->TestCooperativeLevel();
    if (SUCCEEDED(hr))
    {
        return true;
    }
    else if (D3DERR_DEVICENOTRESET == hr)
    {
        // device is ready to be reset, invoke the reanimation procedure...
        this->OnDeviceLost(false);

        // if we are in windowed mode, the cause for the reset may be a display
        // mode change of the desktop, in this case we need to find new
        // buffer pixel formats
        //if (this->displayMode.GetType() == nDisplayMode2::Windowed)
        //{
            D3DFORMAT dispFormat;
            D3DFORMAT backFormat;
            D3DFORMAT zbufFormat;
            this->FindBufferFormats(this->GetDisplayMode().GetBpp(), dispFormat, backFormat, zbufFormat);
            this->presentParams.BackBufferFormat       = backFormat;
            this->presentParams.AutoDepthStencilFormat = zbufFormat;
        //}

        // try to reset the device
        hr = this->d3d9Device->Reset(&this->presentParams);
        if (D3DERR_INVALIDCALL == hr)
        {
            n_assert(false);
        }
        if (FAILED(hr))
        {
            n_printf("nD3D9Server: Failed to reset d3d device!\n");
            return false;
        }
        n_printf("nD3D9Server: Device reset!\n");

        // initialize the device
        this->InitDeviceState();

        // reload the resource
        this->OnRestoreDevice();

        return true;
    }
    else
    {
        // device cannot be restored at this time
        // n_sleep(0.1);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Get the supported feature set.
*/
nGfxServer2::FeatureSet
nD3D9Server::GetFeatureSet()
{
    if (InvalidFeatureSet != this->featureSetOverride)
    {
        return this->featureSetOverride;
    }
    else
    {
        return this->featureSet;
    }
}

//------------------------------------------------------------------------------
/**
    Returns true if device is a software vertex processing device.
    This is a private method called by nD3D9Mesh to decide whether
    vertex buffer must be created in software vertex processing mode.
*/
bool
nD3D9Server::GetSoftwareVertexProcessing() const
{
    return 0 != (this->deviceBehaviourFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING);
}

#ifdef __NEBULA_STATS__
//------------------------------------------------------------------------------
/**
    Query the resource manager and fill the watcher variables with the
    result.
*/
void
nD3D9Server::QueryStatistics()
{
    // compute frames per second
    nTime curTime = kernelServer->GetTimeServer()->GetTime();
    nTime diff = curTime - this->timeStamp;
    if (diff <= 0.000001f)
    {
        diff = 0.000001f;
    }
    this->dbgQueryFPS->SetF(float(1.0 / diff));
    this->timeStamp = curTime;

    this->dbgQueryNumRenderStateChanges->SetI(this->statsNumRenderStateChanges);
    this->dbgQueryNumTextureChanges->SetI(this->statsNumTextureChanges);

    // no resource manager query if not running the debug runtime
    if (0 == this->queryResourceManager)
    {
        return;
    }

    HRESULT hr = this->queryResourceManager->Issue(D3DISSUE_END);
    if (SUCCEEDED(hr))
    {
        D3DDEVINFO_RESOURCEMANAGER result;
        hr = this->queryResourceManager->GetData(&result, sizeof(result), 0);
        if (SUCCEEDED(hr))
        {
            D3DRESOURCESTATS stats;

            // sum texture and cube texture stats
            memcpy(&stats, &(result.stats[D3DRTYPE_TEXTURE]), sizeof(stats));
            stats.bThrashing            |= result.stats[D3DRTYPE_CUBETEXTURE].bThrashing;
            stats.ApproxBytesDownloaded += result.stats[D3DRTYPE_CUBETEXTURE].ApproxBytesDownloaded;
            stats.NumEvicts             += result.stats[D3DRTYPE_CUBETEXTURE].NumEvicts;
            stats.NumVidCreates         += result.stats[D3DRTYPE_CUBETEXTURE].NumVidCreates;
            stats.LastPri               += result.stats[D3DRTYPE_CUBETEXTURE].LastPri;
            stats.NumUsed               += result.stats[D3DRTYPE_CUBETEXTURE].NumUsed;
            stats.NumUsedInVidMem       += result.stats[D3DRTYPE_CUBETEXTURE].NumUsedInVidMem;
            stats.WorkingSet            += result.stats[D3DRTYPE_CUBETEXTURE].WorkingSet;
            stats.WorkingSetBytes       += result.stats[D3DRTYPE_CUBETEXTURE].WorkingSetBytes;
            stats.TotalManaged          += result.stats[D3DRTYPE_CUBETEXTURE].TotalManaged;
            stats.TotalBytes            += result.stats[D3DRTYPE_CUBETEXTURE].TotalBytes;

            // fill watcher variables
            this->dbgQueryTextureTrashing->SetB(stats.bThrashing ? true : false);
            this->dbgQueryTextureApproxBytesDownloaded->SetI(stats.ApproxBytesDownloaded);
            this->dbgQueryTextureNumEvicts->SetI(stats.NumEvicts);
            this->dbgQueryTextureNumVidCreates->SetI(stats.NumVidCreates);
            this->dbgQueryTextureLastPri->SetI(stats.LastPri);
            this->dbgQueryTextureNumUsed->SetI(stats.NumUsed);
            this->dbgQueryTextureNumUsedInVidMem->SetI(stats.NumUsedInVidMem);
            this->dbgQueryTextureWorkingSet->SetI(stats.WorkingSet);
            this->dbgQueryTextureWorkingSetBytes->SetI(stats.WorkingSetBytes);
            this->dbgQueryTextureTotalManaged->SetI(stats.TotalManaged);
            this->dbgQueryTextureTotalBytes->SetI(stats.TotalBytes);
        }
    }
}
#endif

//------------------------------------------------------------------------------
/**
    Updates the mouse cursor image and visibility. Should be called
    once per frame.
*/
void
nD3D9Server::UpdateCursor()
{
    if (this->cursorDirty)
    {
        this->cursorDirty = false;
	
        if (this->curMouseCursor.GetFilename())
        {
            if (!this->curMouseCursor.IsLoaded())
            {
                bool mouseCursorLoaded = this->curMouseCursor.Load();
                n_assert(mouseCursorLoaded);
                this->cursorDirty = true;
            }

            nTexture2* tex = this->curMouseCursor.GetTexture();
            n_assert(tex && tex->IsValid());

            HRESULT hr;
            IDirect3DTexture9* d3d9Tex = ((nD3D9Texture*)tex)->GetTexture2D();
            n_assert(d3d9Tex);

            IDirect3DSurface9* surfPtr = 0;
            hr = d3d9Tex->GetSurfaceLevel(0, &surfPtr);
            n_dxtrace(hr, "In nD3D9Server::UpdateCursor(): GetSurfaceLevel() failed!");
            n_assert(surfPtr);

            int hotspotX = this->curMouseCursor.GetHotspotX();
            int hotspotY = this->curMouseCursor.GetHotspotY();
            hr = this->d3d9Device->SetCursorProperties(hotspotX, hotspotY, surfPtr);
            n_dxtrace(hr, "In nD3D9Server::UpdateCursor(): SetCursorProperties() failed!");

            switch (this->cursorVisibility)
            {
                case nGfxServer2::None:
                    SetCursor(NULL);
                    this->d3d9Device->ShowCursor(FALSE);
                    break;

                case nGfxServer2::System:
                    this->d3d9Device->ShowCursor(FALSE);
                    break;

                case nGfxServer2::Custom:
                    SetCursor(NULL);
                    this->d3d9Device->ShowCursor(TRUE);
                    break;
            }
        }
    }
    // NOTE: cursor visibility is handled inside WinProc!
}

//------------------------------------------------------------------------------
/**
    This method should return the number of currently available stencil bits
    (override in subclass).
*/
int
nD3D9Server::GetNumStencilBits() const
{
    n_assert(this->d3d9Device);
    switch (this->presentParams.AutoDepthStencilFormat)
    {
        case D3DFMT_D24S8:      return 8;
        case D3DFMT_D24X4S4:    return 4;
        default:                return 0;
    }
}

//------------------------------------------------------------------------------
/**
    This method should return the number of currently available depth bits
    (override in subclass).
*/
int
nD3D9Server::GetNumDepthBits() const
{
    n_assert(this->d3d9Device);
    switch (this->presentParams.AutoDepthStencilFormat)
    {
        case D3DFMT_D24S8:
        case D3DFMT_D24X4S4:
        case D3DFMT_D24X8:
            return 24;

        default:
            return 16;
    }
}

//------------------------------------------------------------------------------
/**
    Adjust gamma with given gamma, brightness and contrast.
    The default values are 1.0 for gamma, and roughly 0.5 for brightness
    and contrast.
    If you want to change one of these, first call
    SetGamma/Brightness/Contrast(), then call AdjustGamma()
    to apply the specified value.

    23-Aug-04    kims    created
*/
void nD3D9Server::AdjustGamma()
{
    D3DGAMMARAMP	ramp;

    int val;

    for (int i=0; i<256; i++)
    {
        val = (int)((this->contrast+0.5f)*pow(i/255.f,1.0f/this->gamma)*65535.f +
                    (this->brightness-0.5f)*32768.f - this->contrast*32768.f + 16384.f);
        ramp.red[i] = ramp.green[i] = ramp.blue[i] = n_iclamp(val, 0, 65535);
    }

    nDisplayMode2 dispMode = this->windowHandler.GetDisplayMode();
    if ((dispMode.GetType() == nDisplayMode2::Fullscreen) &&
        this->d3d9Device &&
        (this->devCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA))
    {
        this->d3d9Device->SetGammaRamp(0, D3DSGR_CALIBRATE, &ramp);
    }
    else
    {
        HWND hwnd = this->windowHandler.GetHwnd();
        HDC hdc = GetDC(hwnd);
        SetDeviceGammaRamp(hdc, &ramp);
    }
}

//------------------------------------------------------------------------------
/**
    RestoreGamma() is automatically called when the display is closed.
    (called in nGfxServer2::CloseDisplay())

    23-Aug-04    kims    created
*/
void nD3D9Server::RestoreGamma()
{
    D3DGAMMARAMP	ramp;

    for (int i=0; i<256; i++)
        ramp.red[i] = ramp.green[i] = ramp.blue[i] = i << 8;

    nDisplayMode2 dispMode = this->windowHandler.GetDisplayMode();
    if ((dispMode.GetType() == nDisplayMode2::Fullscreen) &&
        this->d3d9Device &&
        (this->devCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA))
    {
        this->d3d9Device->SetGammaRamp(0, D3DSGR_CALIBRATE, &ramp);
    }
    else
        SetDeviceGammaRamp(GetDC(GetDesktopWindow()), &ramp );
}

//------------------------------------------------------------------------------
/**
    When the display is first opened, we create a collection of nEnvs
    that describe the possible video modes for all adapters.  We
    only include those whose format corresponds to the "best" display
    format, as returned by FindBufferFormats.  Note that, at the moment,
    N2 always uses the default adapter, so knowing about the rest isn't
    very useful.

    Note: nEnv doesn't support unsigned ints or longs, so I used ints -- risky!

    7-Sep-04    rafael    created
*/
void nD3D9Server::CreateDisplayModeEnvVars()
{
    if (!kernelServer->Lookup( "/sys/share/display" ))
    {
        nRoot* cwd = kernelServer->GetCwd();

        D3DFORMAT dispFormat, dummy1, dummy2;
        this->FindBufferFormats(this->GetDisplayMode().GetBpp(), dispFormat, dummy1, dummy2);

        HRESULT hr;
        D3DADAPTER_IDENTIFIER9 identifier;
        D3DDISPLAYMODE mode;
        D3DCAPS9 caps;
        const uint numAdapters = this->d3d9->GetAdapterCount();
        char adaptorDirStr[64];
        char modeSubDirStr[64];
        for (uint adapterIdx = 0; adapterIdx < numAdapters; ++adapterIdx)
        {
            sprintf( adaptorDirStr, "/sys/share/display/adapter%u", adapterIdx );
            nRoot* adaptorDir = kernelServer->New( "nroot", adaptorDirStr );
            kernelServer->SetCwd( adaptorDir );

            // Adapter identifier info
            hr = this->d3d9->GetAdapterIdentifier( adapterIdx, 0, &identifier );
            n_dxtrace(hr, "GetAdapterIdentifier() failed in CreateDisplayModeEnvVars.");
            nEnv* envVar = (nEnv*)kernelServer->New( "nenv", "deviceName" );
            envVar->SetS( identifier.DeviceName );
            envVar = (nEnv*)kernelServer->New( "nenv", "description" );
            envVar->SetS( identifier.Description );

            // adapter mode enumeration
            const uint numModes = this->d3d9->GetAdapterModeCount( adapterIdx, dispFormat );
            for (uint modeIdx = 0; modeIdx < numModes; ++modeIdx)
            {
                sprintf( modeSubDirStr, "modes/mode%u", modeIdx );
                nRoot* modeSubDir = kernelServer->New( "nroot", modeSubDirStr );
                kernelServer->SetCwd( modeSubDir );

                hr = this->d3d9->EnumAdapterModes( adapterIdx, dispFormat, modeIdx, &mode );
                n_dxtrace( hr, "EnumAdapterModes failed in CreateDisplayModeEnvVars." )
                envVar = (nEnv*)kernelServer->New( "nenv", "width" );
                envVar->SetI( mode.Width );
                envVar = (nEnv*)kernelServer->New( "nenv", "height" );
                envVar->SetI( mode.Height );
                envVar = (nEnv*)kernelServer->New( "nenv", "refreshRate" );
                envVar->SetI( mode.RefreshRate );

                kernelServer->SetCwd( adaptorDir );
            }

            // device caps info
            nRoot* capsSubDir = kernelServer->New( "nroot", "caps" );
            kernelServer->SetCwd( capsSubDir );
            hr = this->d3d9->GetDeviceCaps( adapterIdx, N_D3D9_DEVICETYPE, &caps );
            n_dxtrace(hr, "GetDeviceCaps() failed in CreateDisplayModeEnvVars.");

            // the caps that appear here are those that I thought might be useful;
            // feel free to extend it if you need to know something else.
            envVar = (nEnv*)kernelServer->New( "nenv", "maxTexHeight" );
            envVar->SetI( caps.MaxTextureHeight );
            envVar = (nEnv*)kernelServer->New( "nenv", "maxTexWidth" );
            envVar->SetI( caps.MaxTextureWidth );
            envVar = (nEnv*)kernelServer->New( "nenv", "pixelShader/majorVerNum" );
            envVar->SetI( D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion) );
            envVar = (nEnv*)kernelServer->New( "nenv", "pixelShader/minorVerNum" );
            envVar->SetI( D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion) );
            envVar = (nEnv*)kernelServer->New( "nenv", "vertexShader/majorVerNum" );
            envVar->SetI( D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion) );
            envVar = (nEnv*)kernelServer->New( "nenv", "vertexShader/minorVerNum" );
            envVar->SetI( D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion) );
        }

        kernelServer->SetCwd( cwd );
    }
}