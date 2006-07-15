#ifndef N_GLEXTENSIONSERVER_H
#define N_GLEXTENSIONSERVER_H
//------------------------------------------------------------------------------
/**
    @class nGLExtensionServer
    @ingroup OpenGL

    @brief OpenGL extension menegment

    2004 Haron
*/
#include "kernel/nroot.h"

#ifdef __WIN32__
    #include <GL/gl.h>
    #include "GL/wglext.h"
#else
    #include <GL/glx.h>
    #include "GL/glxext.h"
#endif

#include "GL/glext.h"

//#define DECLARE_GL_FUNC(name) #define gl##name nGLExtensionServer::Instance()->proc##name
//#define DECLARE_WGL_FUNC(name) #define wgl##name nGLExtensionServer::Instance()->proc##name

//GL_ARB_vertex_buffer_object functions
#define glBindBufferARB           nGLExtensionServer::Instance()->procBindBufferARB
#define glDeleteBuffersARB        nGLExtensionServer::Instance()->procDeleteBuffersARB
#define glGenBuffersARB           nGLExtensionServer::Instance()->procGenBuffersARB
#define glIsBufferARB             nGLExtensionServer::Instance()->procIsBufferARB
#define glBufferDataARB           nGLExtensionServer::Instance()->procBufferDataARB
#define glBufferSubDataARB        nGLExtensionServer::Instance()->procBufferSubDataARB
#define glGetBufferSubDataARB     nGLExtensionServer::Instance()->procGetBufferSubDataARB
#define glMapBufferARB            nGLExtensionServer::Instance()->procMapBufferARB
#define glUnmapBufferARB          nGLExtensionServer::Instance()->procUnmapBufferARB
#define glGetBufferParameterivARB nGLExtensionServer::Instance()->procGetBufferParameterivARB
#define glGetBufferPointervARB    nGLExtensionServer::Instance()->procGetBufferPointervARB

#ifdef __WIN32__
//WGL_ARB_render_texture functions
#define wglBindTexImageARB        nGLExtensionServer::Instance()->procBindTexImageARB
#define wglReleaseTexImageARB     nGLExtensionServer::Instance()->procReleaseTexImageARB
#define wglSetPbufferAttribARB    nGLExtensionServer::Instance()->procSetPbufferAttribARB
//WGL_ARB_pbuffer functions
#define wglCreatePbufferARB       nGLExtensionServer::Instance()->procCreatePbufferARB;
#define wglGetPbufferDCARB        nGLExtensionServer::Instance()->procGetPbufferDCARB;
#define wglReleasePbufferDCARB    nGLExtensionServer::Instance()->procReleasePbufferDCARB;
#define wglDestroyPbufferARB      nGLExtensionServer::Instance()->procDestroyPbufferARB;
#define wglQueryPbufferARB        nGLExtensionServer::Instance()->procQueryPbufferARB;
//WGL_ARB_pixel_format functions
#define wglGetPixelFormatAttribivARB nGLExtensionServer::Instance()->procGetPixelFormatAttribivARB;
#define wglGetPixelFormatAttribfvARB nGLExtensionServer::Instance()->procGetPixelFormatAttribfvARB;
#define wglChoosePixelFormatARB      nGLExtensionServer::Instance()->procChoosePixelFormatARB;
//WGL_ARB_make_current_read
#define wglMakeContextCurrentARB  nGLExtensionServer::Instance()->procMakeContextCurrentARB;
#define wglGetCurrentReadDCARB    nGLExtensionServer::Instance()->procGetCurrentReadDCARB;
#endif

//GL_ARB_texture_compression
#define glCompressedTexImage3DARB    nGLExtensionServer::Instance()->procCompressedTexImage3DARB;
#define glCompressedTexImage2DARB    nGLExtensionServer::Instance()->procCompressedTexImage2DARB;
#define glCompressedTexImage1DARB    nGLExtensionServer::Instance()->procCompressedTexImage1DARB;
#define glCompressedTexSubImage3DARB nGLExtensionServer::Instance()->procCompressedTexSubImage3DARB;
#define glCompressedTexSubImage2DARB nGLExtensionServer::Instance()->procCompressedTexSubImage2DARB;
#define glCompressedTexSubImage1DARB nGLExtensionServer::Instance()->procCompressedTexSubImage1DARB;
#define glGetCompressedTexImageARB   nGLExtensionServer::Instance()->procGetCompressedTexImageARB;

//GL_ARB_multitexture functions
#define glActiveTextureARB        nGLExtensionServer::Instance()->procActiveTextureARB
#define glMultiTexCoord2fARB      nGLExtensionServer::Instance()->procMultiTexCoord2fARB
#define glClientActiveTextureARB  nGLExtensionServer::Instance()->procClientActiveTextureARB

//------------------------------------------------------------------------------
class nGLExtensionServer : public nRoot
{
public:
    /// constructor
    nGLExtensionServer();
    /// destructor
    virtual ~nGLExtensionServer();
    /// get instance pointer
    static nGLExtensionServer* Instance();
    
    /// GL Extensition Lookup
    bool HasExtension(nString extName);
    /// print extensions to Log file
    void PrintExtensions(nString extStr);

private:
    static nGLExtensionServer* Singleton;

protected:
    friend class nGLServer2;
    friend class nGLMesh;
    friend class nGLTexture;

    /// initialize all possible extention that we are need
    void InitExtensions();

    /// OpenGL extensitions
    bool support_GL_ARB_vertex_buffer_object;
    #ifdef GL_ARB_vertex_buffer_object
    PFNGLBINDBUFFERARBPROC              procBindBufferARB;
    PFNGLDELETEBUFFERSARBPROC           procDeleteBuffersARB;
    PFNGLGENBUFFERSARBPROC              procGenBuffersARB;
    PFNGLISBUFFERARBPROC                procIsBufferARB;
    PFNGLBUFFERDATAARBPROC              procBufferDataARB;
    PFNGLBUFFERSUBDATAARBPROC           procBufferSubDataARB;
    PFNGLGETBUFFERSUBDATAARBPROC        procGetBufferSubDataARB;
    PFNGLMAPBUFFERARBPROC               procMapBufferARB;
    PFNGLUNMAPBUFFERARBPROC             procUnmapBufferARB;
    PFNGLGETBUFFERPARAMETERIVARBPROC    procGetBufferParameterivARB;
    PFNGLGETBUFFERPOINTERVARBPROC       procGetBufferPointervARB;
    #endif

    bool support_WGL_ARB_render_texture;
    #ifdef __WIN32__
    #ifdef WGL_ARB_render_texture
    PFNWGLBINDTEXIMAGEARBPROC           procBindTexImageARB;
    PFNWGLRELEASETEXIMAGEARBPROC        procReleaseTexImageARB;
    #endif

    #ifdef WGL_ARB_pbuffer
    PFNWGLCREATEPBUFFERARBPROC          procCreatePbufferARB;
    PFNWGLGETPBUFFERDCARBPROC           procGetPbufferDCARB;
    PFNWGLRELEASEPBUFFERDCARBPROC       procReleasePbufferDCARB;
    PFNWGLDESTROYPBUFFERARBPROC         procDestroyPbufferARB;
    PFNWGLQUERYPBUFFERARBPROC           procQueryPbufferARB;
    PFNWGLSETPBUFFERATTRIBARBPROC       procSetPbufferAttribARB;    
    #endif

    #ifdef WGL_ARB_pixel_format
    PFNWGLCHOOSEPIXELFORMATARBPROC      procChoosePixelFormatARB;
    PFNWGLGETPIXELFORMATATTRIBIVARBPROC procGetPixelFormatAttribivARB;
    PFNWGLGETPIXELFORMATATTRIBFVARBPROC procGetPixelFormatAttribfvARB;
    #endif

    #ifdef WGL_ARB_make_current_read
    PFNWGLMAKECONTEXTCURRENTARBPROC procMakeContextCurrentARB;
    PFNWGLGETCURRENTREADDCARBPROC   procGetCurrentReadDCARB;
    #endif
    #endif __WIN32__

    bool support_GL_EXT_texture_compression_s3tc;
    bool support_GL_ARB_texture_compression;
    #ifdef GL_ARB_texture_compression
    PFNGLCOMPRESSEDTEXIMAGE3DARBPROC    procCompressedTexImage3DARB;
    PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    procCompressedTexImage2DARB;
    PFNGLCOMPRESSEDTEXIMAGE1DARBPROC    procCompressedTexImage1DARB;
    PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC procCompressedTexSubImage3DARB;
    PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC procCompressedTexSubImage2DARB;
    PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC procCompressedTexSubImage1DARB;
    PFNGLGETCOMPRESSEDTEXIMAGEARBPROC   procGetCompressedTexImageARB;
    #endif

    bool support_GL_ARB_texture_cube_map;

    bool support_GL_ARB_multitexture;
    #ifdef GL_ARB_multitexture
    PFNGLCLIENTACTIVETEXTUREARBPROC     procActiveTextureARB;
    PFNGLMULTITEXCOORD2FARBPROC         procMultiTexCoord2fARB;
    PFNGLACTIVETEXTUREARBPROC           procClientActiveTextureARB;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nGLExtensionServer*
nGLExtensionServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
#endif
