#ifdef __WIN32__
//------------------------------------------------------------------------------
//  win32 specific window handling
//  07-Sep-2003 cubejk
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "opengl/nglextensionserver.h"
#include "opengl/ngltexture.h"
#include "opengl/ncgfxshader.h"

//------------------------------------------------------------------------------
/**
*/
static
int
ChooseAcceleratedPixelFormat(HDC hdc, CONST PIXELFORMATDESCRIPTOR *  ppfd)
{
    int pixelformat = ChoosePixelFormat(hdc, ppfd);

    if (pixelformat == 0) return pixelformat;

    PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(hdc, pixelformat, sizeof(pfd), &pfd)) return 0;

    int accel = ((pfd.dwFlags)&PFD_GENERIC_FORMAT) > 0 ? 0 : 1;
    if ( !accel ) return 0;

    return pixelformat;
}

//------------------------------------------------------------------------------
/**
    Initialize the default state of the device. Must be called after
    creating or resetting the gl device.
*/
void
nGLServer2::InitDeviceState()
{
    n_assert(this->context);

    // update the projection matrix
    this->SetCamera(this->GetCamera());

    // set initial renderstates
    glFrontFace(GL_CW);
    glEnable(GL_DITHER);
    glDisable(GL_LIGHTING);
    glCullFace(GL_FRONT_AND_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //GL_FILL

    n_gltrace("nGLServer2::InitDeviceState().");
}

//-----------------------------------------------------------------------------
/**
*/
void
nGLServer2::ContextOpen(void)
{
    //n_assert(this->windowOpen);
    n_assert(this->hDC);

    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd
        1,                                // version number
        PFD_DRAW_TO_WINDOW |            // support window
        PFD_DOUBLEBUFFER |                // support double buffering
        PFD_SUPPORT_OPENGL,                // support OpenGL
        PFD_TYPE_RGBA,                    // RGBA type
        32,                                // color depth /GetDeviceCaps(this->hDC,BITSPIXEL)/
        0, 0, 0, 0, 0, 0,                // color bits/shifts
        8, 0,                            // alpha bits/shifts
        0,                                // accum bits
        0, 0, 0, 0,                        // r/g/b/a accum bits
        24,                                // depth bits
        8,                                // stencil bits
        0,                                // auxiliary buffers
        PFD_MAIN_PLANE,                    // main layer
        0, 0, 0, 0                        // masks
    };

    PIXELFORMATDESCRIPTOR* pPFDtoUse;

    // let the user override the default pixel format
    pPFDtoUse = &pfd;

    int pixelformat;

    // First try to get a 24b depth buffer
    pixelformat = ChooseAcceleratedPixelFormat(this->hDC, pPFDtoUse);

    if (pixelformat == 0) {

        // Now try a 16b depth buffer
        pPFDtoUse->cDepthBits = 16;
        pixelformat = ChooseAcceleratedPixelFormat(this->hDC, pPFDtoUse);
        if (pixelformat == 0) {

            // Now try a 24b depth buffer without stencil
            pPFDtoUse->cDepthBits = 24;
            pPFDtoUse->cStencilBits = 0;
            pixelformat = ChooseAcceleratedPixelFormat(this->hDC, pPFDtoUse);
            if (pixelformat == 0) {

                // Now try a 16b depth buffer without stencil
                pPFDtoUse->cDepthBits = 16;
                pixelformat = ChooseAcceleratedPixelFormat(this->hDC, pPFDtoUse);
                if (pixelformat == 0) {

                    // Try 16b color formats.
                    pPFDtoUse->cColorBits = 16;
                    pPFDtoUse->cAlphaBits = 0;
                    pPFDtoUse->cDepthBits = 24;
                    pPFDtoUse->cStencilBits = 8;

                    // First try to get a 24b depth buffer
                    pixelformat = ChooseAcceleratedPixelFormat(this->hDC, pPFDtoUse);
                    if (pixelformat == 0) {

                        // Now try a 16b depth buffer
                        pPFDtoUse->cDepthBits = 16;
                        pixelformat = ChooseAcceleratedPixelFormat(this->hDC, pPFDtoUse);
                        if (pixelformat == 0) {

                            // Now try a 24b depth buffer without stencil
                            pPFDtoUse->cDepthBits = 24;
                            pPFDtoUse->cStencilBits = 0;
                            pixelformat = ChooseAcceleratedPixelFormat(this->hDC, pPFDtoUse);
                            if (pixelformat == 0) {

                                // Now try a 16b depth buffer without stencil
                                pPFDtoUse->cDepthBits = 16;
                                pixelformat = ChooseAcceleratedPixelFormat(this->hDC, pPFDtoUse);
                                if (pixelformat == 0) {

                                    // Ok, no good buffer, fail
                                    if (pixelformat == 0)
                                    {
                                        n_error("nGLServer2: ChoosePixelFormat() failed!");
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (!SetPixelFormat(this->hDC, pixelformat, pPFDtoUse))
    {
        n_error("nGLServer2: SetPixelFormat() failed!");
        return;
    }

    // Now squirrel away the pixelformat
    this->pixelFormat = pixelformat;
    if (!DescribePixelFormat(this->hDC, pixelformat, sizeof(this->pfDesc), &this->pfDesc))
    {
        n_error("nGLServer2: DescribePixelFormat() failed!");
    }

    //create a gl context
    this->context = wglCreateContext(this->hDC);
    if (this->context == NULL)
    {
        n_error("nGLServer2: wglCreateContext() failed!");
    }

    // make it the calling thread's current rendering context
    if (!wglMakeCurrent(this->hDC, this->context))
    {
        n_error("nGLServer2: wglMakeCurrent() failed!");
    }

    n_gltrace("nGLServer2::ContextOpen().");
}
//-----------------------------------------------------------------------------
/**
*/
void
nGLServer2::ContextClose()
{
    n_assert(this->context);

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(this->context);
    this->context = NULL;

    n_gltrace("nGLServer2::ContextClose().");
}

//------------------------------------------------------------------------------
/**
    Update the feature set member.
*/
void
nGLServer2::UpdateFeatureSet()
{
        /*
    if (this->devCaps.VertexShaderVersion >= D3DVS_VERSION(2, 0))
    {
        this->featureSet = DX9;
        n_printf("nD3D9Server: DX9 feature set implemented.\n");

        // check if floating point textures are available as render target
        HRESULT hr = this->d3d9->CheckDeviceFormat(D3DADAPTER_DEFAULT,
                                                   N_D3D9_DEVICETYPE,
                                                   D3DFMT_X8R8G8B8,
                                                   D3DUSAGE_RENDERTARGET,
                                                   D3DRTYPE_TEXTURE,
                                                   D3DFMT_R32F);
        if (SUCCEEDED(hr))
        {
            this->featureSet = DX9;
            n_printf("nD3D9Server: DX9 feature set implemented.\n");
        }
        else

        {
            this->featureSet = DX7;
            n_printf("nD3D9Server: DX7 feature set implemented.\n");
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
            n_printf("nD3D9Server: DX8SB feature set available.\n");
        }
        else
        {
            this->featureSet = DX8;
            n_printf("nD3D9Server: DX8 feature set available.\n");
        }
    }
    else
    {
        this->featureSet = DX7;
        n_printf("nD3D9Server: DX7 feature set implemented.\n");
    }
        */
    //n_printf("nGLServer2: OpenGL feature set implemented.\n");
}

//------------------------------------------------------------------------------
/**
    Open and initialize the gl device, make the app window visible.
*/
bool
nGLServer2::DeviceOpen()
{
    n_assert(!this->hDC);
    //n_assert(this->context);
    //n_assert(this->windowOpen);
    //n_assert(this->windowMinimized);
    n_assert(this->windowHandler.IsWindowOpen());
    n_assert(this->windowHandler.IsWindowMinimized());
    n_assert(!this->inBeginScene);
/*
    #ifdef __NEBULA_STATS__
    n_assert(0 == this->queryResourceManager);
    #endif
*/

    // prepare window...
    this->windowHandler.AdjustWindowForChange();

    this->hDC = GetDC(this->windowHandler.GetHwnd());

    // init open opengl context
    this->ContextOpen();

    // create shader device
    bool res = nCgFXShader::CreateDevice();

    nGLExtensionServer* extServer = nGLExtensionServer::Instance();

    //display info
    n_printf("GL - Extensions\n");
    n_printf("    Vendor:   %s\n", glGetString(GL_VENDOR));
    n_printf("    Renderer: %s\n", glGetString(GL_RENDERER));
    n_printf("    Version:  %s\n", glGetString(GL_VERSION));
    n_printf("Supported Extensions:\n");
    extServer->PrintExtensions(nString((const char*)glGetString(GL_EXTENSIONS)));

    //init extensitions
    extServer->InitExtensions();

    // reload any resources if necessary
    this->OnRestoreDevice();

    // initialize d3d device state
    this->InitDeviceState();

    // restore window
    this->windowHandler.RestoreWindow();

    n_gltrace("nGLServer2::DeviceOpen().");
    return res;
}

//------------------------------------------------------------------------------
/**
    Close the gl device, minimize the app window.
*/
void
nGLServer2::DeviceClose()
{
    n_assert(this->hDC);
    n_assert(this->windowHandler.IsWindowOpen());
    n_assert(!this->windowHandler.IsWindowMinimized());
    n_assert(this->windowHandler.GetHwnd());

    // destroy shader device
    nCgFXShader::ReleaseDevice();

    // close gl context
    this->ContextClose();

    // unload all resources
    this->OnDeviceLost(true);

    // destroy gl device
    ReleaseDC(this->windowHandler.GetHwnd(), this->hDC);
    this->hDC = NULL;

    // minimze the app window
    this->windowHandler.MinimizeWindow();
    n_gltrace("nGLServer2::DeviceClose().");
}

//------------------------------------------------------------------------------
/**
    Updates the mouse cursor image and visibility. Should be called
    once per frame.
*/
void
nGLServer2::UpdateCursor()
{
    if (this->cursorDirty)
    {
        this->cursorDirty = false;
/*
        nTexture2* tex = this->curMouseCursor.GetTexture();
        if (tex)
        {
            if (!tex->IsValid())
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
            n_assert(SUCCEEDED(hr));
            n_assert(surfPtr);

            int hotspotX = this->curMouseCursor.GetHotspotX();
            int hotspotY = this->curMouseCursor.GetHotspotY();
            hr = this->d3d9Device->SetCursorProperties(hotspotX, hotspotY, surfPtr);
            n_assert(SUCCEEDED(hr));

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
        }*/
    }
    // NOTE: cursor visibility is handled inside WinProc!
}

//------------------------------------------------------------------------------
/**
    Set a new render target. This method must be called outside
    BeginScene()/EndScene() with a pointer to a nTexture2 object
    which must have been created as render target. A 0 pointer
    restores the original back buffer as render target.

    @param  t   pointer to nTexture2 object or 0
*/
void
nGLServer2::SetRenderTarget(nTexture2* t)
{
    n_assert(!this->inBeginScene);
    n_assert(this->hDC);

    BOOL res;
    nGfxServer2::SetRenderTarget(t);
    if (t)
    {
        nGLTexture* glTex = (nGLTexture*) t;
        res = wglMakeCurrent(glTex->hPBufferDC, glTex->hPBufferRC);
        n_assert(res == TRUE);
    }
    else
    {
        res = wglMakeCurrent(this->hDC, this->context);
        n_assert(res == TRUE);
    }
    n_gltrace("nGLServer2::SetRenderTarget().");
}


//------------------------------------------------------------------------------
/**
    Start rendering the scene.
*/
bool
nGLServer2::BeginScene()
{
    n_assert(this->displayOpen);

    if (nGfxServer2::BeginScene())
    {
        n_assert(this->hDC);
        this->inBeginScene = false;

        // check if gl device is in a valid state
        if (!this->TestResetDevice())
        {
            // device could not be restored at this time
            return false;
        }

        // update mouse cursor image if necessary
        //this->UpdateCursor();

        //check for context lost
        HDC cur_hDC = wglGetCurrentDC();
        HGLRC cur_context = wglGetCurrentContext();
        if ((cur_hDC != hDC) || cur_context != context)
        {
            this->hDC = cur_hDC;
            this->context = cur_context;
            if (false == wglMakeCurrent(this->hDC, this->context))
            {
                n_printf("nGLServer: BeginScene() on gl device failed!\n");
                n_gltrace("nGLServer2::BeginScene(1).");
                return false;
            }
        }

        this->inBeginScene = true;

        #ifdef __NEBULA_STATS__
        // reset statistic variables
        this->dbgQueryNumPrimitives->SetI(0);
        this->dbgQueryNumDrawCalls->SetI(0);
        this->statsNumRenderStateChanges = 0;
        this->statsNumTextureChanges = 0;
        #endif

        n_gltrace("nGLServer2::BeginScene().");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    finish rendering to current render target
*/
void
nGLServer2::EndScene()
{
    n_assert(this->inBeginScene);
    n_assert(this->hDC);

    #ifdef __NEBULA_STATS__
    // query statistics
    this->QueryStatistics();
    #endif

    nGfxServer2::EndScene();
}

//------------------------------------------------------------------------------
/**
    Present the scene.
*/
void
nGLServer2::PresentScene()
{
    n_assert(!this->inBeginScene);
    n_assert(this->hDC);
    bool res = SwapBuffers(this->hDC) == GL_TRUE ? true:false;;
    n_assert(res);

    nGfxServer2::PresentScene();
}
#endif
