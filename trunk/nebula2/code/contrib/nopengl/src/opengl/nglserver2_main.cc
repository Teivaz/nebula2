//------------------------------------------------------------------------------
//  nglserver2_main.cc
//  2003-cubejk
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"
#include "il/il.h"
#include "il/ilu.h"

#include "opengl/nglmesh.h"
#include "opengl/ngltexture.h"

#include "kernel/ntimeserver.h"

nNebulaClass(nGLServer2, "ngfxserver2");

//------------------------------------------------------------------------------
/**
*/
nGLServer2::nGLServer2() :
    #ifdef __NEBULA_STATS__
    timeStamp(0.0),
    //queryResourceManager(0),
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
#ifdef __WIN32__
    hDC(0),
    context(0),
/*    hInst(0),
    hWnd(0),
    parentHWnd(0),
    hAccel(0),
    windowedStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE),
    fullscreenStyle(WS_POPUP | WS_SYSMENU | WS_VISIBLE),
    windowOpen(false),
    windowMinimized(false),
    quitRequested(false),
    pixelFormat(-1),*/
#endif
#ifdef __LINUX__
    IsInit(false),ext_XF86VIDMODE(false),isFullscreen(false),
    display(NULL),screen(0),xvi(NULL),
    x(0),y(0),height(0),width(0),border(0),
    wm_win(0),ctx_win(0),fs_win(0),
    glx_ctx_win(0),glx_fs_win(0),
#endif
    refInputServer("/sys/servers/input"),
    support_GL_ARB_vertex_buffer_object(false),
    support_WGL_ARB_render_texture(false),
    support_WGL_ARB_make_current_read(false),
    support_GL_ARB_texture_compression(false),
    support_GL_EXT_texture_compression_s3tc(false),
    support_GL_ARB_texture_cube_map(false),
    support_GL_ARB_multitexture(false),
    featureSet(InvalidFeatureSet)
{
    // open the app window
    this->windowHandler.OpenWindow();

    // initialize DevIL
    ilInit();
    iluInit();
}

//------------------------------------------------------------------------------
/**
*/
nGLServer2::~nGLServer2()
{
    if (this->displayOpen)
    {
        this->CloseDisplay();
    }

    this->windowHandler.CloseWindow();
    //n_assert(this->textNodeList.IsEmpty());

    ilShutDown();
    //n_assert(this->textNodeList.IsEmpty());
}

//-----------------------------------------------------------------------------
/**
    Open the display, show the window
*/
bool
nGLServer2::OpenDisplay()
{
    n_assert(!this->displayOpen);
    if (this->DeviceOpen())
    {
        nGfxServer2::OpenDisplay();

        // clear display
        if (this->BeginScene())
        {
            this->Clear(AllBuffers, 0.0f, 0.0f, 0.0f, 1.0f, 1.0, 0);
            this->EndScene();
            this->PresentScene();
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Close the display
*/
void
nGLServer2::CloseDisplay()
{
    n_assert(this->displayOpen);
    this->DeviceClose();
    nGfxServer2::CloseDisplay();
}

//-----------------------------------------------------------------------------
/**
    Implements the Windows message pump. Must be called once a
    frame OUTSIDE of BeginScene() / EndScene().

    @return     false if the system requests to shutdown the application
*/
bool
nGLServer2::Trigger()
{
    return this->windowHandler.Trigger();
}

//-----------------------------------------------------------------------------
/**
    Enter dialog box mode.
*/
void
nGLServer2::EnterDialogBoxMode()
{
    n_assert(this->windowHandler.GetDisplayMode().GetDialogBoxMode());
    n_assert(this->hDC);
    nGfxServer2::EnterDialogBoxMode();
    //HRESULT hr = this->d3d9Device->SetDialogBoxMode(TRUE);
    //n_assert(SUCCEEDED(hr));
}

//-----------------------------------------------------------------------------
/**
    Leave dialog box mode.
*/
void
nGLServer2::LeaveDialogBoxMode()
{
    n_assert(this->windowHandler.GetDisplayMode().GetDialogBoxMode());
    n_assert(this->hDC);
    nGfxServer2::LeaveDialogBoxMode();
    //HRESULT hr = this->d3d9Device->SetDialogBoxMode(FALSE);
    //n_assert(SUCCEEDED(hr));
}

//-----------------------------------------------------------------------------
/**
    Set the current display mode. This will not take effect until
    OpenDisplay() has been called!
*/
void
nGLServer2::SetDisplayMode(const nDisplayMode2& mode)
{
    this->windowHandler.SetDisplayMode(mode);
}

//-----------------------------------------------------------------------------
/**
    Get the current display mode.
*/
const nDisplayMode2&
nGLServer2::GetDisplayMode() const
{
    return this->windowHandler.GetDisplayMode();
}

//------------------------------------------------------------------------------
/**
    Check for lost device, and try to reset the device.
*/
bool
nGLServer2::TestResetDevice()
{
    n_assert(this->hDC);
/*    HRESULT hr;

    hr = this->d3d9Device->TestCooperativeLevel();
    if (SUCCEEDED(hr))
    {
        return true;
    }
    else if (D3DERR_DEVICENOTRESET == hr)
    {
        // device is ready to be reset, invoke the reanimation procedure...
        this->OnDeviceLost();

        // if we are in windowed mode, the cause for the reset may be a display
        // mode change of the desktop, in this case we need to find new
        // buffer pixel formats
        if (this->displayMode.GetType() == nDisplayMode2::Windowed)
        {
            D3DFORMAT dispFormat;
            D3DFORMAT backFormat;
            D3DFORMAT zbufFormat;
            this->FindBufferFormats(dispFormat, backFormat, zbufFormat);

            this->presentParams.BackBufferFormat       = backFormat;
            this->presentParams.AutoDepthStencilFormat = zbufFormat;
        }

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
    else*/
    {
        // device cannot be restored at this time
        n_sleep(0.1);
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    Get the supported feature set.
*/
nGfxServer2::FeatureSet
nGLServer2::GetFeatureSet()
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

#ifdef __NEBULA_STATS__
//------------------------------------------------------------------------------
/**
    Query the resource manager and fill the watcher variables with the
    result.
*/
void
nGLServer2::QueryStatistics()
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
/*
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
*/
}
#endif

//-----------------------------------------------------------------------------
/**
    hasExtension()
    check if a extensition supported by GL
    19-Jun-99   floh    aus "Using OpenGL Extensions"
    @param    extName    the name of the requested extensition
    @return    true    if extensition is supported
    @return false    if extensition is not supported
*/
bool nGLServer2::hasExtension(const char *extName)
{
    char *p = (char *) glGetString(GL_EXTENSIONS);
    char *end;
    int extNameLen;
    extNameLen = strlen(extName);
    end = p + strlen(p);
    while (p < end) {
        int n = strcspn(p, " ");
        if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
            return true;
        }
        p += (n + 1);
    }
    return false;
}

//
#define EXTENTION_BEGIN(extname)\
    n_printf("-> " #extname " ");\
    if (this->hasExtension(#extname))\
    {\
        n_printf(".");\
        this->support_##extname = true;


#define procAddress(token,type,procname,extname) \
        this->proc##procname = (token) wglGetProcAddress(#type #procname); \
        if ( this->proc##procname == NULL ) \
        { \
            this->support_##extname = false; \
            n_printf(" failed: " #type #procname "\n"); \
        } \
        else \
            n_printf (".");

#define EXTENTION_END(extname)\
        if (this->support_##extname)\
        {\
            n_printf(" good!\n");\
        }\
    }\
    else n_printf(" not supported!\n");

//-----------------------------------------------------------------------------
/**
    init opengl extensitions, obtain proc adresses for windows
    09-Sep-2003    cubejk    converted from nebula1
    09-Sep-2003    cubejk    +ARB_vertex_buffer_object
    10-Dec-2003    Haron    + other extention support
*/
void nGLServer2::initExtensions(void)
{
    n_printf("Init OpenGL extensitions:\n");
    #ifdef GL_ARB_vertex_buffer_object
    EXTENTION_BEGIN(GL_ARB_vertex_buffer_object)
        #ifdef __WIN32__
        procAddress(PFNGLBINDBUFFERARBPROC, gl,
                    BindBufferARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLDELETEBUFFERSARBPROC, gl,
                    DeleteBuffersARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLGENBUFFERSARBPROC, gl,
                    GenBuffersARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLISBUFFERARBPROC, gl,
                    IsBufferARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLBUFFERDATAARBPROC, gl,
                    BufferDataARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLBUFFERSUBDATAARBPROC, gl,
                    BufferSubDataARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLGETBUFFERSUBDATAARBPROC, gl,
                    GetBufferSubDataARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLMAPBUFFERARBPROC, gl,
                    MapBufferARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLUNMAPBUFFERARBPROC, gl,
                    UnmapBufferARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLGETBUFFERPARAMETERIVARBPROC, gl,
                    GetBufferParameterivARB, GL_ARB_vertex_buffer_object);
        procAddress(PFNGLGETBUFFERPOINTERVARBPROC, gl,
                    GetBufferPointervARB, GL_ARB_vertex_buffer_object);
        #endif __WIN32__
    EXTENTION_END(GL_ARB_vertex_buffer_object)
    #endif

    #ifdef WGL_ARB_render_texture
    EXTENTION_BEGIN(WGL_ARB_render_texture)
        #ifdef __WIN32__
        procAddress(PFNWGLBINDTEXIMAGEARBPROC, wgl,
                    BindTexImageARB, WGL_ARB_render_texture);
        procAddress(PFNWGLRELEASETEXIMAGEARBPROC, wgl,
                    ReleaseTexImageARB, WGL_ARB_render_texture);
        procAddress(PFNWGLSETPBUFFERATTRIBARBPROC, wgl,
                    SetPbufferAttribARB, WGL_ARB_render_texture);
        procAddress(PFNWGLCREATEPBUFFERARBPROC, wgl,
                    CreatePbufferARB, WGL_ARB_render_texture);
        procAddress(PFNWGLGETPBUFFERDCARBPROC, wgl,
                    GetPbufferDCARB, WGL_ARB_render_texture);
        procAddress(PFNWGLRELEASEPBUFFERDCARBPROC, wgl,
                    ReleasePbufferDCARB, WGL_ARB_render_texture);
        procAddress(PFNWGLDESTROYPBUFFERARBPROC, wgl,
                    DestroyPbufferARB, WGL_ARB_render_texture);
        procAddress(PFNWGLQUERYPBUFFERARBPROC, wgl,
                    QueryPbufferARB, WGL_ARB_render_texture);
        procAddress(PFNWGLGETPIXELFORMATATTRIBIVARBPROC, wgl,
                    GetPixelFormatAttribivARB, WGL_ARB_render_texture);
        procAddress(PFNWGLGETPIXELFORMATATTRIBFVARBPROC, wgl,
                    GetPixelFormatAttribfvARB, WGL_ARB_render_texture);
        procAddress(PFNWGLCHOOSEPIXELFORMATARBPROC, wgl,
                    ChoosePixelFormatARB, WGL_ARB_render_texture);
        #endif __WIN32__
    EXTENTION_END(WGL_ARB_render_texture)
    #endif

    #ifdef WGL_ARB_make_current_read
    EXTENTION_BEGIN(WGL_ARB_make_current_read)
        #ifdef __WIN32__
        procAddress(PFNWGLMAKECONTEXTCURRENTARBPROC, wgl,
                    MakeContextCurrentARB, WGL_ARB_make_current_read);
        procAddress(PFNWGLGETCURRENTREADDCARBPROC, wgl,
                    GetCurrentReadDCARB, WGL_ARB_make_current_read);
        #endif __WIN32__
    EXTENTION_END(WGL_ARB_make_current_read)
    #endif

    #ifdef GL_ARB_texture_compression
    EXTENTION_BEGIN(GL_ARB_texture_compression)
        #ifdef __WIN32__
        procAddress(PFNGLCOMPRESSEDTEXIMAGE3DARBPROC, gl,
                    CompressedTexImage3DARB, GL_ARB_texture_compression);
        procAddress(PFNGLCOMPRESSEDTEXIMAGE2DARBPROC, gl,
                    CompressedTexImage2DARB, GL_ARB_texture_compression);
        procAddress(PFNGLCOMPRESSEDTEXIMAGE1DARBPROC, gl,
                    CompressedTexImage1DARB, GL_ARB_texture_compression);
        procAddress(PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC, gl,
                    CompressedTexSubImage3DARB, GL_ARB_texture_compression);
        procAddress(PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC, gl,
                    CompressedTexSubImage2DARB, GL_ARB_texture_compression);
        procAddress(PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC, gl,
                    CompressedTexSubImage1DARB, GL_ARB_texture_compression);
        procAddress(PFNGLGETCOMPRESSEDTEXIMAGEARBPROC, gl,
                    GetCompressedTexImageARB, GL_ARB_texture_compression);
        #endif __WIN32__
    EXTENTION_END(GL_ARB_texture_compression)
    #endif

    #ifdef GL_EXT_texture_compression_s3tc
    EXTENTION_BEGIN(GL_EXT_texture_compression_s3tc)
    EXTENTION_END(GL_EXT_texture_compression_s3tc)
    #endif

    #ifdef GL_ARB_texture_cube_map
    EXTENTION_BEGIN(GL_ARB_texture_cube_map)
    EXTENTION_END(GL_ARB_texture_cube_map)
    #endif

    #ifdef GL_ARB_multitexture
    EXTENTION_BEGIN(GL_ARB_multitexture)
        #ifdef __WIN32__
        procAddress(PFNGLCLIENTACTIVETEXTUREARBPROC, gl,
                    ActiveTextureARB, GL_ARB_multitexture);
        procAddress(PFNGLMULTITEXCOORD2FARBPROC, gl,
                    MultiTexCoord2fARB, GL_ARB_multitexture);
        procAddress(PFNGLACTIVETEXTUREARBPROC, gl,
                    ClientActiveTextureARB, GL_ARB_multitexture);
        #endif __WIN32__
    EXTENTION_END(GL_ARB_multitexture)
    #endif

    if (!this->support_GL_ARB_vertex_buffer_object || !this->support_WGL_ARB_render_texture)
    {
        n_printf("Required extensions were not supported by your hardware.\n");
        n_printf("Notice: if you have a nVidia grafic card, please update your driver to >=45.23 and retry.\n");
    }
}

//-----------------------------------------------------------------------------
/**
    check for GL errors
*/
bool
nGLServer2::getGLErrors(char *src)
{
    //TODO: GL Error handling
    bool res = false;
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        n_printf("\n--- GL error in (%s): FIXME ", src);
        res = true;
        switch (error)
        {
            case GL_OUT_OF_MEMORY:
                n_printf("<GL_OUT_OF_MEM>");
                //n_assert(false);
                break;
            case GL_INVALID_ENUM:
                n_printf("<GL_INVALID_ENUM>");
                break;
            case GL_INVALID_VALUE:
                n_printf("<GL_INVALID_VALUE>");
                break;
            case GL_INVALID_OPERATION:
                n_printf("<GL_INVALID_OPERATION>");
                break;
            default:
                n_printf("<GL_ERROR_TYPE:%u>",error);
                //n_assert(false);
        }
        n_printf(". ---\n\n");
    }
    return res;
}

//--------------------------------------------------------------------
/**
    print available GL extension to log file
*/
void
nGLServer2::printExtensions(const char* ext)
{
    if (ext)
    {
        char buf[2048];
        char c;
        int i = 0;
        do {
            c = *ext++;
            if ((c == ' ') || (c == 0)) {
                buf[i] = 0;
                n_printf("%s\n",buf);
                i = 0;
            } else buf[i++] = c;
        } while (c);
    }
}

