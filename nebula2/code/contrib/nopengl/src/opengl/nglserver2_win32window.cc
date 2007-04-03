#ifdef __WIN32__
//------------------------------------------------------------------------------
//  win32 specific window handling
//  07-Sep-2003 cubejk
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "opengl/nglextensionserver.h"
#include "opengl/ngltexture.h"
//#include "opengl/ncgfxshader.h"
#include "opengl/nglslshader.h"

//------------------------------------------------------------------------------
/**
*/
static
int
ChooseAcceleratedPixelFormat(HDC hdc, CONST PIXELFORMATDESCRIPTOR* ppfd)
{
    int pixelformat = ChoosePixelFormat(hdc, ppfd);

    if (pixelformat != 0)
    {
        return pixelformat;
    }

    PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(hdc, pixelformat, sizeof(pfd), &pfd))
    {
        return 0;
    }

    int accel = ((pfd.dwFlags)&PFD_GENERIC_FORMAT) > 0 ? 0 : 1;
    if (!accel)
    {
        return 0;
    }

    return pixelformat;
}

//-----------------------------------------------------------------------------
/**
    history:
     - 31-Mar-2007   je.a.le    modified
*/
void
nGLServer2::ContextOpen(void)
{
    //n_assert(this->windowOpen);
    n_assert(this->hDC);

    glGetError();

    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd
        1,                                // version number
        PFD_DRAW_TO_WINDOW |              // support window
        PFD_DOUBLEBUFFER |                // support double buffering
        PFD_SUPPORT_OPENGL,               // support OpenGL
        PFD_TYPE_RGBA,                    // RGBA type
        32,                               // color depth /GetDeviceCaps(this->hDC,BITSPIXEL)/
        0, 0, 0, 0, 0, 0,                 // color bits/shifts
        0, 0,                             // alpha bits/shifts
        0,                                // accum bits
        0, 0, 0, 0,                       // r/g/b/a accum bits
        16,                               // depth bits
        0,                                // stencil bits
        0,                                // auxiliary buffers
        PFD_MAIN_PLANE,                   // main layer
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
    Set a new render target. This method must be called outside
    BeginScene()/EndScene() with a pointer to a nTexture2 object
    which must have been created as render target. A 0 pointer
    restores the original back buffer as render target.

    @param  t   pointer to nTexture2 object or 0
*/
void
nGLServer2::SetRenderTarget(int index, nTexture2* t)
{
    n_assert(!this->inBeginScene);
    n_assert(this->hDC);

    nGfxServer2::SetRenderTarget(index, t);

    BOOL res;
    if (t)
    {
        nGLTexture* glTex = (nGLTexture*) t;
        res = wglMakeCurrent(glTex->hPBufferDC, glTex->hPBufferRC);
    }
    else
    {
        res = wglMakeCurrent(this->hDC, this->context);
    }
    n_assert(res == TRUE);
    n_gltrace("nGLServer2::SetRenderTarget().");
}

//------------------------------------------------------------------------------
/**
    Start rendering the scene. This can be called several times per frame
    (each render target requires its own BeginScene()/EndScene().
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
        //if (!this->TestResetDevice())
        //{
        //    // device could not be restored at this time
        //    return false;
        //}

        // update scene shader parameters
        this->UpdatePerSceneSharedShaderParams();

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

        n_gltrace("nGLServer2::BeginScene().");

        this->inBeginScene = true;
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

    //#ifdef __NEBULA_STATS__
    //// query statistics
    //this->QueryStatistics();
    //#endif

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

    bool res = SwapBuffers(this->hDC) == GL_TRUE;
    n_assert(res);

    nGfxServer2::PresentScene();
}
#endif
