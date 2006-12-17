//------------------------------------------------------------------------------
//  nd3d9server_device.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9texture.h"
#include "kernel/ntimeserver.h"
#include "gfx2/nd3d9shader.h"
#include "resource/nresourceserver.h"

//------------------------------------------------------------------------------
/**
    Initialize the device identifier fields. This should be called from
    the constructor after Direct3D has been opened.
*/
void
nD3D9Server::InitDeviceIdentifier()
{
    n_assert(this->d3d9);

    // extract device identifier
    D3DADAPTER_IDENTIFIER9 identifier;
    HRESULT hr = this->d3d9->GetAdapterIdentifier(N_D3D9_ADAPTER, 0, &identifier);
    n_dxtrace(hr, "nD3D9Server::InitDeviceState(): GetAdapterIdentifier() failed!");

    if (0x8086 == identifier.VendorId)
    {
        // Intel
        if (0x2572 == identifier.DeviceId)
        {
            // Intel(R) 82865G Graphics Controller
            this->deviceIdentifier = Intel_82865G;
            n_printf("Intel(R) 82865G Graphics Controller detected.\n");
        }
    }
    else if (0x1039 == identifier.VendorId)
    {
        // SiS
        if (0x6330 == identifier.DeviceId)
        {
            // SiS 741
            this->deviceIdentifier = SiS_741;
            n_printf("SiS 741 detected.\n");
        }
        else if (0x6300 == identifier.DeviceId)
        {
            // SiS 630
            this->deviceIdentifier = SiS_630;
            n_printf("SiS 630 detected.\n");
        }
    }
}

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
    hr = this->d3d9->CheckDeviceFormat(N_D3D9_ADAPTER,
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
    hr = this->d3d9->CheckDepthStencilMatch(N_D3D9_ADAPTER,
                                            N_D3D9_DEVICETYPE,
                                            adapterFormat,
                                            backbufferFormat,
                                            depthFormat);
    return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
/**
    Checks whether the requested multisample type is supported, and returns
    a D3D-Multisample type which will definitely work.
*/
D3DMULTISAMPLE_TYPE
nD3D9Server::CheckMultiSampleType(D3DFORMAT backbufferFormat,
                                  D3DFORMAT depthFormat,
                                  bool windowed)
{
    n_assert(this->d3d9);
    HRESULT hr;
    D3DMULTISAMPLE_TYPE d3dMsType = (D3DMULTISAMPLE_TYPE) this->GetDisplayMode().GetAntiAliasSamples();
    if (D3DMULTISAMPLE_NONE != d3dMsType)
    {
        // need to check both back buffer and zbuffer
        hr = this->d3d9->CheckDeviceMultiSampleType(N_D3D9_ADAPTER,
                                                    N_D3D9_DEVICETYPE,
                                                    backbufferFormat,
                                                    windowed,
                                                    d3dMsType,
                                                    NULL);
        if (FAILED(hr))
        {
            // not supported
            d3dMsType = D3DMULTISAMPLE_NONE;
        }
        else
        {
            hr = this->d3d9->CheckDeviceMultiSampleType(N_D3D9_ADAPTER,
                                                        N_D3D9_DEVICETYPE,
                                                        depthFormat,
                                                        windowed,
                                                        d3dMsType,
                                                        NULL);

            if (FAILED(hr))
            {
                // not supported
                d3dMsType = D3DMULTISAMPLE_NONE;
            }
        }
    }
    return d3dMsType;
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
        // find full-screen buffer formats
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
        hr = this->d3d9->GetAdapterDisplayMode(N_D3D9_ADAPTER, &desktopMode);
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
    HRESULT hr = this->d3d9->GetDeviceCaps(N_D3D9_ADAPTER, N_D3D9_DEVICETYPE, &(this->devCaps));
    n_dxtrace(hr, "GetDeviceCaps() in nD3D9Server::UpdateFeatureSet() failed!");
    if (this->devCaps.VertexShaderVersion >= D3DVS_VERSION(2, 0))
    {
        // check if floating point textures are available as render target
        HRESULT hr = this->d3d9->CheckDeviceFormat(N_D3D9_ADAPTER,
                                                N_D3D9_DEVICETYPE,
                                                D3DFMT_X8R8G8B8,
                                                D3DUSAGE_RENDERTARGET,
                                                D3DRTYPE_TEXTURE,
                                                D3DFMT_A16B16G16R16F);
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
        HRESULT hr = this->d3d9->CheckDeviceFormat(N_D3D9_ADAPTER,
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
    n_assert(0 == this->captureSurface);
    #ifdef __NEBULA_STATS__
    n_assert(0 == this->queryResourceManager);
    #endif

    n_printf("nD3D9Server::DeviceOpen()\n");

    HRESULT hr;

    // prepare window...
    this->windowHandler.AdjustWindowForChange();

    // find a valid combination of buffer formats
    D3DFORMAT dispFormat;
    D3DFORMAT backFormat;
    D3DFORMAT zbufFormat;
    this->FindBufferFormats(this->GetDisplayMode().GetBpp(), dispFormat, backFormat, zbufFormat);

    // get d3d multisample type
    D3DMULTISAMPLE_TYPE d3dMultiSampleType = this->CheckMultiSampleType(backFormat, zbufFormat, this->windowHandler.GetDisplayMode().GetType() != nDisplayMode2::Fullscreen);

    // define device behavior flags
    this->deviceBehaviourFlags = D3DCREATE_FPU_PRESERVE;

    #if N_D3D9_DEBUG
        this->deviceBehaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        this->presentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        n_printf("nD3D9Server: using enforced software vertex processing\n");
    #else
        // check if hardware vertex shaders are supported, if not,
        // activate mixed vertex processing
        #if N_D3D9_FORCEMIXEDVERTEXPROCESSING
            this->deviceBehaviourFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
            n_printf("nD3D9Server: using FORCED mixed vertex processing\n");
        #elif N_D3D9_FORCESOFTWAREVERTEXPROCESSING
            this->deviceBehaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            n_printf("nD3D9Server: using FORCED software vertex processing\n");
        #else
            if (this->devCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
            {
                if (this->GetFeatureSet() >= DX9)
                {
                    this->deviceBehaviourFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
                    n_printf("nD3D9Server: using hardware vertex processing\n");
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
        #endif
        this->presentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    #endif

    // check for dialog box mode
/*
    if (this->windowHandler.GetDisplayMode().GetDialogBoxMode())
    {
        this->presentParams.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    }
*/

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
    this->presentParams.MultiSampleType                 = d3dMultiSampleType;
    this->presentParams.MultiSampleQuality              = 0;
    this->presentParams.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    this->presentParams.hDeviceWindow                   = this->windowHandler.GetAppHwnd();
    this->presentParams.EnableAutoDepthStencil          = TRUE;
    this->presentParams.AutoDepthStencilFormat          = zbufFormat;
    this->presentParams.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;

    // create d3d device
    hr = this->d3d9->CreateDevice(N_D3D9_ADAPTER,
                                  N_D3D9_DEVICETYPE,
                                  this->windowHandler.GetAppHwnd(),
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
    hr = D3DXCreateSphere(this->d3d9Device, 1.0f, 12, 6, &this->shapeMeshes[Sphere], NULL);
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): D3DXCreateSphere() failed!");
    hr = D3DXCreateTorus(this->d3d9Device, 1.0f, 0.5f, 18, 12, &this->shapeMeshes[Torus], NULL);
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): D3DXCreateTorus() failed!");
    hr = D3DXCreateTeapot(this->d3d9Device, &this->shapeMeshes[Teapot], NULL);
    n_dxtrace(hr, "nD3D9Server::DeviceOpen(): D3DXCreateTeapot() failed!");

    // reload any resources if necessary
    this->OnDeviceInit(true);

    // initialize d3d device state
    this->InitDeviceState();

    // restore window
    this->windowHandler.RestoreWindow();

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
    n_assert(this->windowHandler.GetAppHwnd());

    n_printf("nD3D9Server::DeviceClose()\n");

    // unload all resources
    this->OnDeviceCleanup(true);

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

    // minimize the app window
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
        this->OnDeviceCleanup(false);

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

        // reload the resource
        this->OnDeviceInit(false);

        // initialize the device
        this->InitDeviceState();

        return true;
    }
    else
    {
        // device cannot be restored at this time
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
    Query the resource manager and fill the watcher variables with the
    result.
*/
#ifdef __NEBULA_STATS__
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
    WATCHER_SET_FLOAT(watchFPS, float(1.0f / diff));
    WATCHER_SET_INT(watchNumPrimitives, this->statsNumPrimitives);
    WATCHER_SET_INT(watchNumDrawCalls, this->statsNumDrawCalls);
    WATCHER_SET_INT(watchNumRenderStateChanges, this->statsNumRenderStateChanges);

    this->timeStamp = curTime;
    this->statsNumRenderStateChanges = 0;
    this->statsNumTextureChanges = 0;
    this->statsNumDrawCalls = 0;
    this->statsNumPrimitives = 0;

    // no resource manager query if not running the debug runtime
/*
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
*/
}
#endif

//------------------------------------------------------------------------------
/**
    Updates the mouse cursor image and visibility. Should be called
    once per frame.

    11-Jul-05   floh    fixed cursor visibility bug
*/
void
nD3D9Server::UpdateCursor()
{
    if (this->cursorDirty)
    {
        this->cursorDirty = false;

        nTexture2* tex = this->curMouseCursor.GetTexture();
        if (tex)
        {
            if (!tex->IsLoaded())
            {
                bool mouseCursorLoaded = tex->Load();
                n_assert(mouseCursorLoaded);
                this->cursorDirty = true;
            }

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
        }

        switch (this->cursorVisibility)
        {
            case nGfxServer2::None:
            case nGfxServer2::Gui:
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
    Called either when the d3d device is lost, or when the display is closed.
    This should perform the required cleanup work on all affected resources.
*/
void
nD3D9Server::OnDeviceCleanup(bool shutdown)
{
    // close the text renderer
    this->CloseTextRenderer();

    // handle resources
    if (shutdown)
    {
        // the display is about to be closed, do a real unload
        this->refResource->UnloadResources(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);
    }
    else
    {
        // the device has been lost, tell the resources about it
        this->refResource->OnLost(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);
    }

    // release shape shader
    if (this->refShapeShader.isvalid())
    {
        this->refShapeShader->Release();
        this->refShapeShader.invalidate();
    }

    // release the shared state shader
    if (this->refSharedShader.isvalid())
    {
        this->refSharedShader->Release();
        this->refSharedShader.invalidate();
    }

    // release refs on original backbuffer and depth/stencil surfaces
    if (this->captureSurface)
    {
        this->captureSurface->Release();
        this->captureSurface = 0;
    }
    if (this->backBufferSurface)
    {
        this->backBufferSurface->Release();
        this->backBufferSurface = 0;
    }
    if (this->depthStencilSurface)
    {
        this->depthStencilSurface->Release();
        this->depthStencilSurface = 0;
    }

    // inform line renderer
    HRESULT hr = this->d3dxLine->OnLostDevice();
    n_dxtrace(hr, "OnLostDevice() on d3dxLine failed");

    #ifdef __NEBULA_STATS__
    // release the d3d query object
    if (this->queryResourceManager)
    {
        this->queryResourceManager->Release();
        this->queryResourceManager = 0;
    }
    #endif
}

//------------------------------------------------------------------------------
/**
    This method is called either after the software device has been
    created, or after the device has been restored.
*/
void
nD3D9Server::OnDeviceInit(bool startup)
{
    n_assert(!this->refSharedShader.isvalid());
    n_assert(0 == this->depthStencilSurface);
    n_assert(0 == this->backBufferSurface);
    n_assert(0 == this->captureSurface);

    HRESULT hr;

    // get a pointer to the back buffer and depth/stencil surface
    hr = this->d3d9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &(this->backBufferSurface));
    n_dxtrace(hr, "GetBackBuffer() on device failed.");
    n_assert(this->backBufferSurface);

    hr = this->d3d9Device->GetDepthStencilSurface(&this->depthStencilSurface);
    n_dxtrace(hr, "GetDepthStencilSurface() on device failed.");
    n_assert(this->depthStencilSurface);

    // create an offscreen surface for capturing data
    hr = this->d3d9Device->CreateOffscreenPlainSurface(this->presentParams.BackBufferWidth,
                                                       this->presentParams.BackBufferHeight,
                                                       this->presentParams.BackBufferFormat,
                                                       D3DPOOL_SYSTEMMEM,
                                                       &(this->captureSurface), NULL);
    n_dxtrace(hr, "CreateOffscreenPlainSurface() failed.");
    n_assert(this->captureSurface);

    #ifdef __NEBULA_STATS__
    // create a query object for resource manager queries
    // this will fail if not running the debug runtime, so this is not critical
    hr = this->d3d9Device->CreateQuery(D3DQUERYTYPE_RESOURCEMANAGER, &(this->queryResourceManager));
    #endif

    // restore or load resources
    if (startup)
    {
        // this is a real startup
        this->refResource->LoadResources(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);
    }
    else
    {
        // the device has been restored...
        this->refResource->OnRestored(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);
    }

    // open the text renderer
    this->OpenTextRenderer();

    // inform line renderer
    hr = this->d3dxLine->OnResetDevice();
    n_dxtrace(hr, "OnResetDevice() on d3dxLine failed");

    // update mouse cursor
    this->cursorDirty = true;
    this->UpdateCursor();

    // create the shape shader
    this->refShapeShader = (nD3D9Shader*)this->NewShader("shape");
    if (!this->refShapeShader->IsLoaded())
    {
        this->refShapeShader->SetFilename("shaders:shape.fx");
        if (!this->refShapeShader->Load())
        {
            this->refShapeShader->Release();
            this->refShapeShader.invalidate();
        }
    }

    // create the shared effect parameter reference shader
    this->refSharedShader = (nD3D9Shader*)this->NewShader("shared");
    if (!this->refSharedShader->IsLoaded())
    {
        this->refSharedShader->SetFilename("shaders:shared.fx");
        if (!this->refSharedShader->Load())
        {
            this->refSharedShader->Release();
            this->refSharedShader.invalidate();
        }
    }

    // refresh projection matrix (necessary AFTER sharedShader has been created
    // so that the shared transform matrices can be set after a DeviceReset)
    this->SetCamera(this->GetCamera());
}

//------------------------------------------------------------------------------
/**
    Enable/disable software vertex processing. This is only a valid call
    when the device has been created with mixed vertex processing.
*/
void
nD3D9Server::SetSoftwareVertexProcessing(bool b)
{
    n_assert(this->d3d9Device);
    if (this->deviceBehaviourFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    {
        HRESULT hr = this->d3d9Device->SetSoftwareVertexProcessing((BOOL)b);
        n_assert(SUCCEEDED(hr));
    }
}

//------------------------------------------------------------------------------
/**
    Get the current software processing state of the device.
*/
bool
nD3D9Server::GetSoftwareVertexProcessing()
{
    n_assert(this->d3d9Device);
    if (this->deviceBehaviourFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
    {
        return false;
    }
    else if (this->deviceBehaviourFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
    {
        return true;
    }
    else if (this->deviceBehaviourFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    {
        BOOL b = this->d3d9Device->GetSoftwareVertexProcessing();
        return b ? true : false;
    }
    else
    {
        n_error("nD3D9Server::GetSoftwareProcessing(): can't happen!");
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Returns the current actual render target size.
*/
vector2
nD3D9Server::GetCurrentRenderTargetSize() const
{
    vector2 size;
    nTexture2* renderTarget = this->GetRenderTarget(0);
    if (renderTarget)
    {
        size.x = (float)renderTarget->GetWidth();
        size.y = (float)renderTarget->GetHeight();
    }
    else
    {
        const nDisplayMode2& mode = this->GetDisplayMode();
        size.x = (float)mode.GetWidth();
        size.y = (float)mode.GetHeight();
    }
    return size;
}

//------------------------------------------------------------------------------
/**
    Set the scissor rectangle. Note that this method doesn't enable/disable
    scissoring, this must be done externally in the shader.
*/
void
nD3D9Server::SetScissorRect(const rectangle& r)
{
    n_assert(this->d3d9Device);
    nGfxServer2::SetScissorRect(r);
    this->UpdateScissorRect();
}

//------------------------------------------------------------------------------
/**
    Updates the current scissor rectangle. This must be called from
    SetScissorRect() and SetRenderTarget() (since SetRenderTarget() resets
    the current scissor rectangle).
*/
void
nD3D9Server::UpdateScissorRect()
{
    // convert to D3D screen coordinates
    vector2 rtSize = this->GetCurrentRenderTargetSize();
    RECT rect;
    rect.left   = int(this->scissorRect.v0.x * rtSize.x);
    rect.right  = int(this->scissorRect.v1.x * rtSize.x);
    rect.top    = int(this->scissorRect.v0.y * rtSize.y);
    rect.bottom = int(this->scissorRect.v1.y * rtSize.y);
    HRESULT hr = this->d3d9Device->SetScissorRect(&rect);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
    Set user defined clip planes in clip space. Clip space is where
    outgoing vertex shader vertices live in. Up to 6 clip planes
    can be defined. Provide an empty array to clear all clip planes.

    NOTE: this method does not work in the DX7 render path (check the D3D docs why)
*/
void
nD3D9Server::SetClipPlanes(const nArray<plane>& planes)
{
    n_assert(this->d3d9Device);
    nGfxServer2::SetClipPlanes(planes);
    if (this->GetFeatureSet() > DX7)
    {
        HRESULT hr;
        DWORD clipPlaneMask = 0;
        uint num = planes.Size();
        if (num > this->devCaps.MaxUserClipPlanes)
        {
            num = this->devCaps.MaxUserClipPlanes;
        }
        uint i;
        for (i = 0; i < num; i++)
        {
            hr = this->d3d9Device->SetClipPlane(i, &(planes[i].a));
            n_dxtrace(hr, "SetClipPlane() failed!");
            clipPlaneMask |= (1 << i);
        }
        hr = this->d3d9Device->SetRenderState(D3DRS_CLIPPLANEENABLE, clipPlaneMask);
        n_dxtrace(hr, "Enable clip planes failed!");
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
    D3DGAMMARAMP ramp;

    for (int i = 0; i < 256; i++)
    {
        int val = (int)((this->contrast + 0.5f) * pow(i / 255.f, 1.0f / this->gamma) * 65535.f +
                  (this->brightness - 0.5f) * 32768.f - this->contrast * 32768.f + 16384.f);
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
        HWND hwnd = this->windowHandler.GetAppHwnd();
        HDC hdc = GetDC(hwnd);
        SetDeviceGammaRamp(hdc, &ramp);
        ReleaseDC(hwnd, hdc);
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
    D3DGAMMARAMP ramp;

    for (int i = 0; i < 256; i++)
        ramp.red[i] = ramp.green[i] = ramp.blue[i] = i << 8;

    nDisplayMode2 dispMode = this->windowHandler.GetDisplayMode();
    if ((dispMode.GetType() == nDisplayMode2::Fullscreen) &&
        this->d3d9Device &&
        (this->devCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA))
    {
        this->d3d9Device->SetGammaRamp(0, D3DSGR_CALIBRATE, &ramp);
    }
    else
    {
        HWND hwnd = GetDesktopWindow();
        HDC hdc = GetDC(hwnd);
        SetDeviceGammaRamp(hdc, &ramp);
        ReleaseDC(hwnd, hdc);
    }
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
    if (!kernelServer->Lookup("/sys/share/display"))
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
            sprintf(adaptorDirStr, "/sys/share/display/adapter%u", adapterIdx);
            nRoot* adaptorDir = kernelServer->New("nroot", adaptorDirStr);
            kernelServer->SetCwd(adaptorDir);

            // Adapter identifier info
            hr = this->d3d9->GetAdapterIdentifier(adapterIdx, 0, &identifier);
            n_dxtrace(hr, "GetAdapterIdentifier() failed in CreateDisplayModeEnvVars.");
            nEnv* envVar = (nEnv*)kernelServer->New("nenv", "deviceName");
            envVar->SetS(identifier.DeviceName);
            envVar = (nEnv*)kernelServer->New("nenv", "description");
            envVar->SetS(identifier.Description);

            // adapter mode enumeration
            const uint numModes = this->d3d9->GetAdapterModeCount(adapterIdx, dispFormat);
            for (uint modeIdx = 0; modeIdx < numModes; ++modeIdx)
            {
                sprintf(modeSubDirStr, "modes/mode%u", modeIdx);
                nRoot* modeSubDir = kernelServer->New("nroot", modeSubDirStr);
                kernelServer->SetCwd(modeSubDir);

                hr = this->d3d9->EnumAdapterModes(adapterIdx, dispFormat, modeIdx, &mode);
                n_dxtrace(hr, "EnumAdapterModes failed in CreateDisplayModeEnvVars.")
                envVar = (nEnv*)kernelServer->New("nenv", "width");
                envVar->SetI(mode.Width);
                envVar = (nEnv*)kernelServer->New("nenv", "height");
                envVar->SetI(mode.Height);
                envVar = (nEnv*)kernelServer->New("nenv", "refreshRate");
                envVar->SetI(mode.RefreshRate);

                kernelServer->SetCwd(adaptorDir);
            }

            // device caps info
            nRoot* capsSubDir = kernelServer->New("nroot", "caps");
            kernelServer->SetCwd(capsSubDir);
            hr = this->d3d9->GetDeviceCaps(adapterIdx, N_D3D9_DEVICETYPE, &caps);
            n_dxtrace(hr, "GetDeviceCaps() failed in CreateDisplayModeEnvVars.");

            // the caps that appear here are those that I thought might be useful;
            // feel free to extend it if you need to know something else.
            envVar = (nEnv*)kernelServer->New("nenv", "maxTexHeight");
            envVar->SetI(caps.MaxTextureHeight);
            envVar = (nEnv*)kernelServer->New("nenv", "maxTexWidth");
            envVar->SetI(caps.MaxTextureWidth);
            envVar = (nEnv*)kernelServer->New("nenv", "pixelShader/majorVerNum");
            envVar->SetI(D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion));
            envVar = (nEnv*)kernelServer->New("nenv", "pixelShader/minorVerNum");
            envVar->SetI(D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion));
            envVar = (nEnv*)kernelServer->New("nenv", "vertexShader/majorVerNum");
            envVar->SetI(D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion));
            envVar = (nEnv*)kernelServer->New("nenv", "vertexShader/minorVerNum");
            envVar->SetI(D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion));
        }

        kernelServer->SetCwd(cwd);
    }
}
