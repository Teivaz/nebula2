//------------------------------------------------------------------------------
//  nglextensionserver_main.cc
//  2004 Haron
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "opengl/nglextensionserver.h"

//nNebulaClass(nGLExtensionServer, "nroot");

#define EXTENSION_BEGIN(extname) bool nGLExtensionServer::support_##extname = false;
#define DECL_GLEXT_PROC(type, procname) type procname = NULL;
#define EXTENSION_END(extname)
#include "opengl\nglextensions.h"
#undef EXTENSION_BEGIN
#undef DECL_GLEXT_PROC
#undef EXTENSION_END

//------------------------------------------------------------------------------
/**
*/
nGLExtensionServer::nGLExtensionServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGLExtensionServer::~nGLExtensionServer()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    HasExtension()
    check if a extensition supported by GL
    19-Jun-99   floh    aus "Using OpenGL Extensions"
    16-Oct-2004 Haron   moved to nGLExtensionServer class. Use nString.
    @param    extName    the name of the requested extensition
    @return    true    if extensition is supported
    @return false    if extensition is not supported
*/
bool nGLExtensionServer::HasExtension(nString extName)
{
    nString extStr((const char*) glGetString(GL_EXTENSIONS));
    const char* ext = extStr.GetFirstToken(" ");
    while (ext)
    {
        if (extName == ext)
        {
            return true;
        }
        ext = extStr.GetNextToken(" ");
    }
    return false;
}

//--------------------------------------------------------------------
/**
    print available GL extension to log file
    16-Oct-2004 Haron updated
*/
void
nGLExtensionServer::PrintExtensions(nString extStr)
{
    const char* ext;
    ext = extStr.GetFirstToken(" ");
    while (ext != NULL)
    {
        n_printf("    %s\n", ext);
        ext = extStr.GetNextToken(" ");
    }
}

//-----------------------------------------------------------------------------
/**
    init opengl extensitions, obtain proc adresses for windows
    09-Sep-2003    cubejk    converted from nebula1
    09-Sep-2003    cubejk    +ARB_vertex_buffer_object
    10-Dec-2003    Haron    + other extention support
*/
void nGLExtensionServer::InitExtensions()
{
    n_printf("\nInit OpenGL extensitions:");

#define EXTENSION_BEGIN(extname)\
    support_##extname = HasExtension(#extname);\
    n_printf("\n[%c] " #extname "\n", support_##extname ? '+' : '-');\
    if (support_##extname)\
    {
#ifdef __WIN32__
#define DECL_GLEXT_PROC(type, procname)\
        procname = (type) wglGetProcAddress(#procname);\
        n_printf("    [%c] " #procname "\n", procname ? '+' : '-');
#else // other OS not implemented yet
#define DECL_GLEXT_PROC(type, procname)\
        procname = NULL;
#endif

#define EXTENSION_END(extname)\
    }

#include "opengl\nglextensions.h"

#undef EXTENSION_BEGIN
#undef DECL_GLEXT_PROC
#undef EXTENSION_END

    n_printf("\n");

    //if (!nGLExtensionServer::support_GL_ARB_vertex_buffer_object || !nGLExtensionServer::support_WGL_ARB_render_texture)
    //{
    //    n_printf("Required extensions were not supported by your hardware.\n");
    //    n_printf("Notice: if you have a nVidia grafic card, please update your driver to >=45.23 and retry.\n");
    //}
    //n_gltrace("nGLExtensionServer::InitExtensions().");
}
