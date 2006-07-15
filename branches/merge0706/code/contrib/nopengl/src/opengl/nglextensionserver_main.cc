//------------------------------------------------------------------------------
//  nglextensionserver_main.cc
//  2004 Haron
//------------------------------------------------------------------------------
#include "opengl/nglextensionserver.h"
#include "opengl/nglserver2.h"

nNebulaClass(nGLExtensionServer, "nroot");
nGLExtensionServer* nGLExtensionServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nGLExtensionServer::nGLExtensionServer() :
    support_GL_ARB_vertex_buffer_object(false),
    support_WGL_ARB_render_texture(false),
    //support_WGL_ARB_make_current_read(false),
    //support_WGL_ARB_pbuffer(false),
    //support_WGL_ARB_pixel_format(false),
    support_GL_EXT_texture_compression_s3tc(false),
    support_GL_ARB_texture_compression(false),
    support_GL_ARB_texture_cube_map(false),
    support_GL_ARB_multitexture(false)
{
    n_assert(0 == Singleton);
    Singleton = this;

    //this->InitExtensions();
}

//------------------------------------------------------------------------------
/**
*/
nGLExtensionServer::~nGLExtensionServer()
{
    n_assert(0 != Singleton);
    Singleton = 0;
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

//
#define EXTENSION_BEGIN(extname)\
    n_printf("" #extname " -");\
    if (this->HasExtension(#extname))\
    {\
        n_printf(" supported!\n");\
        this->support_##extname = true;

#ifdef __WIN32__
#define procAddress(token,type,procname,extname)\
        this->proc##procname = (token) wglGetProcAddress(#type #procname);\
        if ( this->proc##procname == NULL )\
        { \
            n_printf("    " #type #procname " not supported.\n");\
        } \
        else \
        {\
            n_printf("    " #type #procname " supported.\n");\
        }
#else // other OS not supported yet
#define procAddress(token,type,procname,extname)\
        this->proc##procname = NULL;\
        if ( this->proc##procname == NULL )\
        { \
            n_printf("    " #type #procname " not supported.\n");\
        } \
        else \
        {\
            n_printf("    " #type #procname " supported.\n");\
        }
#endif

#define EXTENSION_END(extname)\
        n_printf("\n");\
    }\
    else n_printf(" not supported!\n\n");

//-----------------------------------------------------------------------------
/**
    init opengl extensitions, obtain proc adresses for windows
    09-Sep-2003    cubejk    converted from nebula1
    09-Sep-2003    cubejk    +ARB_vertex_buffer_object
    10-Dec-2003    Haron    + other extention support
*/
void nGLExtensionServer::InitExtensions()
{
    n_printf("\nInit OpenGL extensitions:\n");

    #ifdef GL_ARB_vertex_buffer_object
    EXTENSION_BEGIN(GL_ARB_vertex_buffer_object)
        procAddress(PFNGLBINDBUFFERARBPROC, gl,
                    BindBufferARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLDELETEBUFFERSARBPROC, gl,
                    DeleteBuffersARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLGENBUFFERSARBPROC, gl,
                    GenBuffersARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLISBUFFERARBPROC, gl,
                    IsBufferARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLBUFFERDATAARBPROC, gl,
                    BufferDataARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLBUFFERSUBDATAARBPROC, gl,
                    BufferSubDataARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLGETBUFFERSUBDATAARBPROC, gl,
                    GetBufferSubDataARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLMAPBUFFERARBPROC, gl,
                    MapBufferARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLUNMAPBUFFERARBPROC, gl,
                    UnmapBufferARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLGETBUFFERPARAMETERIVARBPROC, gl,
                    GetBufferParameterivARB, GL_ARB_vertex_buffer_object)
        procAddress(PFNGLGETBUFFERPOINTERVARBPROC, gl,
                    GetBufferPointervARB, GL_ARB_vertex_buffer_object)
    EXTENSION_END(GL_ARB_vertex_buffer_object)
    #endif

    #ifdef WGL_ARB_render_texture
    EXTENSION_BEGIN(WGL_ARB_render_texture)
        #ifdef __WIN32__
        procAddress(PFNWGLBINDTEXIMAGEARBPROC, wgl,
                    BindTexImageARB, WGL_ARB_render_texture)
        procAddress(PFNWGLRELEASETEXIMAGEARBPROC, wgl,
                    ReleaseTexImageARB, WGL_ARB_render_texture)

        procAddress(PFNWGLCREATEPBUFFERARBPROC, wgl,
                    CreatePbufferARB, WGL_ARB_pbuffer)
        procAddress(PFNWGLGETPBUFFERDCARBPROC, wgl,
                    GetPbufferDCARB, WGL_ARB_pbuffer)
        procAddress(PFNWGLRELEASEPBUFFERDCARBPROC, wgl,
                    ReleasePbufferDCARB, WGL_ARB_pbuffer)
        procAddress(PFNWGLDESTROYPBUFFERARBPROC, wgl,
                    DestroyPbufferARB, WGL_ARB_pbuffer)
        procAddress(PFNWGLQUERYPBUFFERARBPROC, wgl,
                    QueryPbufferARB, WGL_ARB_pbuffer)
        procAddress(PFNWGLSETPBUFFERATTRIBARBPROC, wgl,
                    SetPbufferAttribARB, WGL_ARB_pbuffer)

        procAddress(PFNWGLCHOOSEPIXELFORMATARBPROC, wgl,
                    ChoosePixelFormatARB, WGL_ARB_pixel_format)
        procAddress(PFNWGLGETPIXELFORMATATTRIBIVARBPROC, wgl,
                    GetPixelFormatAttribivARB, WGL_ARB_pixel_format)
        procAddress(PFNWGLGETPIXELFORMATATTRIBFVARBPROC, wgl,
                    GetPixelFormatAttribfvARB, WGL_ARB_pixel_format)

        procAddress(PFNWGLMAKECONTEXTCURRENTARBPROC, wgl,
                    MakeContextCurrentARB, WGL_ARB_make_current_read)
        procAddress(PFNWGLGETCURRENTREADDCARBPROC, wgl,
                    GetCurrentReadDCARB, WGL_ARB_make_current_read)
        #endif __WIN32__
    EXTENSION_END(WGL_ARB_render_texture)
    #endif

    #ifdef GL_ARB_texture_compression
    EXTENSION_BEGIN(GL_ARB_texture_compression)
        procAddress(PFNGLCOMPRESSEDTEXIMAGE3DARBPROC, gl,
                    CompressedTexImage3DARB, GL_ARB_texture_compression)
        procAddress(PFNGLCOMPRESSEDTEXIMAGE2DARBPROC, gl,
                    CompressedTexImage2DARB, GL_ARB_texture_compression)
        procAddress(PFNGLCOMPRESSEDTEXIMAGE1DARBPROC, gl,
                    CompressedTexImage1DARB, GL_ARB_texture_compression)
        procAddress(PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC, gl,
                    CompressedTexSubImage3DARB, GL_ARB_texture_compression)
        procAddress(PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC, gl,
                    CompressedTexSubImage2DARB, GL_ARB_texture_compression)
        procAddress(PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC, gl,
                    CompressedTexSubImage1DARB, GL_ARB_texture_compression)
        procAddress(PFNGLGETCOMPRESSEDTEXIMAGEARBPROC, gl,
                    GetCompressedTexImageARB, GL_ARB_texture_compression)
    EXTENSION_END(GL_ARB_texture_compression)
    #endif

    #ifdef GL_EXT_texture_compression_s3tc
    EXTENSION_BEGIN(GL_EXT_texture_compression_s3tc)
    EXTENSION_END(GL_EXT_texture_compression_s3tc)
    #endif

    #ifdef GL_ARB_texture_cube_map
    EXTENSION_BEGIN(GL_ARB_texture_cube_map)
    EXTENSION_END(GL_ARB_texture_cube_map)
    #endif

    #ifdef GL_ARB_multitexture
    EXTENSION_BEGIN(GL_ARB_multitexture)
        procAddress(PFNGLCLIENTACTIVETEXTUREARBPROC, gl,
                    ActiveTextureARB, GL_ARB_multitexture)
        procAddress(PFNGLMULTITEXCOORD2FARBPROC, gl,
                    MultiTexCoord2fARB, GL_ARB_multitexture)
        procAddress(PFNGLACTIVETEXTUREARBPROC, gl,
                    ClientActiveTextureARB, GL_ARB_multitexture)
    EXTENSION_END(GL_ARB_multitexture)
    #endif

    if (!this->support_GL_ARB_vertex_buffer_object || !this->support_WGL_ARB_render_texture)
    {
        n_printf("Required extensions were not supported by your hardware.\n");
        n_printf("Notice: if you have a nVidia grafic card, please update your driver to >=45.23 and retry.\n");
    }
    n_gltrace("nGLExtensionServer::InitExtensions().");
}
