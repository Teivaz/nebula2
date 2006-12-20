#ifndef N_GLSERVER2_H
#define N_GLSERVER2_H
//------------------------------------------------------------------------------
/**
    @class nGLServer2
    @ingroup OpenGL

    @brief OpenGL 2 based gfx server.

    2003        cubejk    created
    2003-2006   Haron
*/
#include "gfx2/ngfxserver2.h"
#include "input/ninputserver.h"
#include "misc/nwatched.h"
#include "opengl/nglwin32windowhandler.h"

#include "opengl/nglincludes.h"

void n_gltrace(const char *msg);

//------------------------------------------------------------------------------
class nGLServer2 : public nGfxServer2
{
public:
    /// constructor
    nGLServer2();
    /// destructor
    virtual ~nGLServer2();
    /// get instance pointer
    static nGLServer2* Instance();

    /// create a shared mesh object
    virtual nMesh2* NewMesh(const nString& rsrcName);
    /// create a mesh array object
    virtual nMeshArray* NewMeshArray(const nString& rsrcName);
    /// create a shared texture object
    virtual nTexture2* NewTexture(const nString& rsrcName);
    /// create a shared shader object
    virtual nShader2* NewShader(const nString& rsrcName);
    /// create a font object
    virtual nFont2* NewFont(const nString& rsrcName, const nFontDesc& fontDesc);
    /// create a render target object
    virtual nTexture2* NewRenderTarget(const nString& rsrcName, int width, int height, nTexture2::Format fmt, int usageFlags);
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
    //virtual bool AreVertexShadersEmulated();
    /// parent window handle
    virtual HWND GetParentHwnd() const;
    /// returns the number of available stencil bits
    //virtual int GetNumStencilBits() const;
    /// returns the number of available z bits
    //virtual int GetNumDepthBits() const;
    /// set scissor rect
    //virtual void SetScissorRect(const rectangle& r);
    /// set or clear user defined clip planes in clip space
    //virtual void SetClipPlanes(const nArray<plane>& planes);

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
    //virtual void DrawText(const nString& text, const vector4& color, const rectangle& rect, uint flags, bool immediate=true);
    /// get text extents
    //virtual vector2 GetTextExtent(const nString& text);
    /// draw the text buffer
    //virtual void DrawTextBuffer();

    /// enter dialog box mode (display mode must have DialogBoxMode enabled!)
    virtual void EnterDialogBoxMode();
    /// leave dialog box mode
    virtual void LeaveDialogBoxMode();

    /// save a screen shot
    virtual bool SaveScreenshot(const char* fileName, nTexture2::FileFormat fileFormat);

    /// begin rendering lines
    virtual void BeginLines();
    /// draw 3d lines, using the current transforms
    virtual void DrawLines3d(const vector3* vertexList, int numVertices, const vector4& color);
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

    /// adjust gamma.
    //virtual void AdjustGamma();
    /// restore gamma.
    //virtual void RestoreGamma();
    /// skip message loop in trigger
    //virtual void SetSkipMsgLoop(bool skip);

    enum GLShaderSystem
    {
        GLSL,
        CGFX,
        CG,
        GLES
    };

private:
    /// initialize the text renderer
    void OpenTextRenderer();
    /// shutdown the text renderer
    void CloseTextRenderer();
    /// check for lost device, and reset if possible
    bool TestResetDevice();
    /// create the gl context
    void ContextOpen();
    /// release the gl context
    void ContextClose();
    /// open the gl device
    bool DeviceOpen();
    /// close the gl device
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
    /// query the gl resource manager and fill the watched variables
    void QueryStatistics();
    #endif
    /// get gl primitive type and num primitives for indexed drawing
    int GetGLPrimTypeAndNumIndexed(PrimitiveType primType, GLenum& glPrimType) const;
    /// get gl primitive type and num primitives
    int GetGLPrimTypeAndNum(PrimitiveType primType, GLenum& glPrimType) const;
    /// update the mouse cursor image and visibility
    void UpdateCursor();

    /// open the window
    //bool WindowOpen();
    /// close the window
    //void WindowClose();
    #ifdef __WIN32__
    /// prepare window for switching between windowd/fullscreen mode
    //void AdjustWindowForChange();
    #endif
    /// restore window from minimized state after d3d device has been created
    //void RestoreWindow();
    /// minimize window when d3d device is destroyed
    //void MinimizeWindow();

    friend class nGLMesh;
    friend class nGLTexture;
    friend class nCgFXShader;
    friend class nGLSLShader;

    static nGLServer2* Singleton;

#ifdef __WIN32__
    nGLWin32WindowHandler windowHandler; ///< a Win32 window handler object
#endif

#ifdef __LINUX__
    nGLLinuxWindowHandler windowHandler; ///< a Linux window handler object
#endif

    //bool windowOpen;               ///< window has been opened
    FeatureSet featureSet;

private:

    nRef<nShader2> refSharedShader; ///< reference shader for shared effect parameters

    #ifdef __NEBULA_STATS__
    nTime timeStamp;                 ///< time stamp for FPS computation
    // query watcher variables
    WATCHER_DECLARE(watchNumPrimitives);
    WATCHER_DECLARE(watchFPS);
    WATCHER_DECLARE(watchNumDrawCalls);
    WATCHER_DECLARE(watchNumRenderStateChanges);

    //nWatched dbgQueryTextureTrashing;
    //nWatched dbgQueryTextureApproxBytesDownloaded;
    //nWatched dbgQueryTextureNumEvicts;
    //nWatched dbgQueryTextureNumVidCreates;
    //nWatched dbgQueryTextureLastPri;
    //nWatched dbgQueryTextureNumUsed;
    //nWatched dbgQueryTextureNumUsedInVidMem;
    //nWatched dbgQueryTextureWorkingSet;
    //nWatched dbgQueryTextureWorkingSetBytes;
    //nWatched dbgQueryTextureTotalManaged;
    //nWatched dbgQueryTextureTotalBytes;
    //nWatched dbgQueryNumPrimitives;
    //nWatched dbgQueryFPS;
    //nWatched dbgQueryNumDrawCalls;
    //nWatched dbgQueryNumRenderStateChanges;
    //nWatched dbgQueryNumTextureChanges;

    int statsFrameCount;
    int statsNumTextureChanges;
    int statsNumRenderStateChanges;
    int statsNumDrawCalls;
    int statsNumPrimitives;
    #endif

    #ifdef __WIN32__
    HDC        hDC;                  ///< the device context
    HGLRC    context;                ///< the gl render context
    int pixelFormat;
    PIXELFORMATDESCRIPTOR pfDesc;

public:
    // NOTE: this stuff is public because WinProcs may need to access it
/*
    enum
    {
        ACCEL_TOGGLEFULLSCREEN = 1001,
    };
    /// translate win32 keycode into Nebula keycode
    //nKey TranslateKey(int vkey);
*/

    //bool windowMinimized;           ///< window is currently minimized
    //bool quitRequested;             ///< quit requested by WinProc()
    #endif

    #ifdef __LINUX__
private:
    xWindowOpen();
    xWindowClose();

    ///xkey to nebulaKey translation
    nKey translateKey(KeySym xkey);

    nAutoRef<nInputServer> refInputServer;

    ///update the window to the current settings
    void updateWindow(void);
    ///true if the current mode is fullscreen
    bool isFullscreen;
    //the position of the current window
    int x, y;
    //the size of the current window
    unsigned int height, width;
    //the border of the current window
    unsigned int border;
    //holds flags for resize and other update needs of a window.
    unsigned int updateWindowMask;

    //return a reusable or a new Colormap
    Colormap getColormap(void);

    ///the connection to the xserver
    Display *display;
    ///the default screennumber of this display
    int screen;
    ///true if XF86VIDMODE extension/real fullscreen is supported
    bool ext_XF86VIDMODE;
    ///XVisualInfo - description for the X window based on the seleced Framebufferconfig
    XVisualInfo *xvi;

    //the event to receive from X if the Window was destroyed
    XEvent event;
    //the event description to recive if the Window was destroyed
    Atom wmDeleteWindow;

    ///windowmanger window - needed as group leader ?!
    Window  wm_win;
    ///the render window for winodowed mode
    Window  ctx_win;
    ///the render window for fullscreen mode
    Window  fs_win;
    ///a pointer to the current window
    Window* current_win;

    /*GLX - Handling*/
    ///retrive bit depth of a GLXFBConfig
    int getBufferAttr(GLXFBConfig config,const int attribute);
    //display Various informations about GLX
    void showGLXInfo(void);

    ///the rendercontext
    GLXContext context;
    ///the selected frame buffer config
    GLXFBConfig selectedConfig;

    ///the GLXDrawable for the fullscreen window
    GLXWindow  glx_fs_win;
    ///the GLXDrawable for the windowed window
    GLXWindow  glx_ctx_win;
    ///a GLXDrawable pointer to current active window
    GLXWindow* glx_current_win;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nGLServer2*
nGLServer2::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
HWND
nGLServer2::GetParentHwnd() const
{
    return this->windowHandler.GetParentHwnd();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGLServer2::GetGLPrimTypeAndNumIndexed(PrimitiveType primType, GLenum& glPrimType) const
{
    int glNumPrimitives = 0;
    switch (primType)
    {
        case PointList:
            glPrimType = GL_POINTS;
            glNumPrimitives = this->indexRangeNum;
            break;

        case LineList:
            glPrimType = GL_LINES;
            glNumPrimitives = this->indexRangeNum / 2;
            break;

        case LineStrip:
            glPrimType = GL_LINE_STRIP;
            glNumPrimitives = this->indexRangeNum - 1;
            break;

        case TriangleList:
            glPrimType = GL_TRIANGLES;
            glNumPrimitives = this->indexRangeNum / 3;
            break;

        case TriangleStrip:
            glPrimType = GL_TRIANGLE_STRIP;
            glNumPrimitives = this->indexRangeNum - 2;
            break;

        case TriangleFan:
            glPrimType = GL_TRIANGLE_FAN;
            glNumPrimitives = this->indexRangeNum - 2;
            break;
    }
    return glNumPrimitives;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGLServer2::GetGLPrimTypeAndNum(PrimitiveType primType, GLenum& glPrimType) const
{
    int glNumPrimitives = 0;
    switch (primType)
    {
        case PointList:
            glPrimType = GL_POINTS;
            glNumPrimitives = this->vertexRangeNum;
            break;

        case LineList:
            glPrimType = GL_LINES;
            glNumPrimitives = this->vertexRangeNum / 2;
            break;

        case LineStrip:
            glPrimType = GL_LINE_STRIP;
            glNumPrimitives = this->vertexRangeNum - 1;
            break;

        case TriangleList:
            glPrimType = GL_TRIANGLES;
            glNumPrimitives = this->vertexRangeNum / 3;
            break;

        case TriangleStrip:
            glPrimType = GL_TRIANGLE_STRIP;
            glNumPrimitives = this->vertexRangeNum - 2;
            break;

        case TriangleFan:
            glPrimType = GL_TRIANGLE_FAN;
            glNumPrimitives = this->vertexRangeNum - 2;
            break;
    }
    return glNumPrimitives;
}

//------------------------------------------------------------------------------
#endif
