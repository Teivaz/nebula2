#ifndef N_GLINCLUDES_H
#define N_GLINCLUDES_H
//------------------------------------------------------------------------------
/**
    @ingroup OpenGL

    @brief Include platform specific OpenGL headers

    2006   Haron
*/
#ifdef __WIN32__
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif
#ifdef __LINUX__
    #include <GL/glx.h>
    #include <X11/Xatom.h>
    #include <X11/Xmu/StdCmap.h>
    #include <X11/keysym.h>
    //#include <X11/extensions/xf86vmode.h>
#endif
#ifdef __MACOSX__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <Carbon/carbon.h>
    #include <AGL/agl.h>
#endif

#ifdef __WIN32__
    #include <GL/wglext.h>

    #if WGL_WGLEXT_VERSION < 6
        #error "A newer version of <GL/wglext.h> should be supported. Please download it from http://oss.sgi.com/projects/ogl-sample/ABI/"
    #endif
#else
    #include "GL/glxext.h"

    #if GLX_GLXEXT_VERSION < 11
        #error "A newer version of <GL/glxext.h> should be supported. Please download it from http://oss.sgi.com/projects/ogl-sample/ABI/"
    #endif
#endif

#include <GL/glext.h>

#if GL_GLEXT_VERSION < 27
    #error "A newer version of <GL/glext.h> should be supported. Please download it from http://oss.sgi.com/projects/ogl-sample/ABI/"
#endif

//#ifndef GL_VERSION_2_0
//#error "There is no support for GL_VERSION_2_0 in <GL/glext.h>. Please download a newer version from http://oss.sgi.com/projects/ogl-sample/ABI/"
//#endif

//------------------------------------------------------------------------------
#endif
