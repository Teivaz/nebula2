/*notes:
    GL_NUM_COMPRESSED_TEXTURE_FORMATS
    GL_COMPRESSED_TEXTURE_FORMATS

    GL_EXT_texture_compression


    X8R8G8B8,           =>RGB8 + X
    A8R8G8B8,           =>RGBA8
        case X8R8G8B8:
        case A8R8G8B8:
            depthFormat = D3DFMT_D24S8;
    R5G6B5,
    A1R5G5B5,           =>RGB5_A1
    A4R4G4B4,           =>RGBA4
    P8,                 =>GL_EXT_paletted_texture
    DXT1,DXT3, DXT5   => GL_EXT_texture_compression_s3tc
    DXT2,DXT4,        => No supported.

    TEXTURE_2D,
    TEXTURE_3D,     => GL_EXT_texture3D
    TEXTURE_CUBE,   => GL_EXT_cube_texture_map == GL_ARB_cube_texture_map

    ARB_depth_texture
*/


#ifndef N_GLTEXTURE_H
#define N_GLTEXTURE_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Texture
    @ingroup OpenGL

    OpenGL subclass for nTexture2.

    - 21-Jun-2003 cubejk  created
    - Dec-2003 Haron      + P-Buffer and render_texture support
    - Jan-2004 Haron      + compressed (DDS) texture support
*/
#include "gfx2/ntexture2.h"
#include "opengl/nglserver2.h"

#ifdef __WIN32__
//GL_ARB_texture_compression functions
#define glCompressedTexImage3DARB       this->refGfxServer->procCompressedTexImage3DARB
#define glCompressedTexImage2DARB       this->refGfxServer->procCompressedTexImage2DARB
#define glCompressedTexImage1DARB       this->refGfxServer->procCompressedTexImage1DARB
#define glCompressedTexSubImage3DARB    this->refGfxServer->procCompressedTexSubImage3DARB
#define glCompressedTexSubImage2DARB    this->refGfxServer->procCompressedTexSubImage2DARB
#define glCompressedTexSubImage1DARB    this->refGfxServer->procCompressedTexSubImage1DARB
#define glGetCompressedTexImageARB      this->refGfxServer->procGetCompressedTexImageARB
//WGL_ARB_render_texture functions
#define wglBindTexImageARB          this->refGfxServer->procBindTexImageARB
#define wglReleaseTexImageARB       this->refGfxServer->procReleaseTexImageARB
#define wglSetPbufferAttribARB      this->refGfxServer->procSetPbufferAttribARB
//WGL_ARB_pbuffer functions
#define wglCreatePbufferARB         this->refGfxServer->procCreatePbufferARB
#define wglGetPbufferDCARB          this->refGfxServer->procGetPbufferDCARB
#define wglReleasePbufferDCARB      this->refGfxServer->procReleasePbufferDCARB
#define wglDestroyPbufferARB        this->refGfxServer->procDestroyPbufferARB
#define wglQueryPbufferARB          this->refGfxServer->procQueryPbufferARB
//WGL_ARB_pixel_format functions
#define wglGetPixelFormatAttribivARB    this->refGfxServer->procGetPixelFormatAttribivARB
#define wglGetPixelFormatAttribfvARB    this->refGfxServer->procGetPixelFormatAttribfvARB
#define wglChoosePixelFormatARB         this->refGfxServer->procChoosePixelFormatARB
//WGL_ARB_make_current_read functions
#define wglMakeContextCurrentARB    this->refGfxServer->procMakeContextCurrentARB
#define wglGetCurrentReadDCARB      this->refGfxServer->procGetCurrentReadDCARB
#endif

#ifdef __MACOSX__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    //#include <Carbon/carbon.h>
    #include <AGL/agl.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#ifdef __LINUX__
    #include <GL/glx.h>
    //#include <X11/Xatom.h>
    //#include <X11/Xmu/StdCmap.h>
    //#include <X11/keysym.h>
    //#include <X11/extensions/xf86vmode.h>
#endif

#include "GL/glext.h"

#ifdef __WIN32__
    #include "GL/wglext.h"
#else
    #include "GL/glxext.h"
#endif

//------------------------------------------------------------------------------
class nGLTexture : public nTexture2
{
public:
    /// constructor
    nGLTexture();
    /// destructor
    virtual ~nGLTexture();
    /// supports async resource loading
    virtual bool CanLoadAsync() const;
    /// lock a 2D texture, returns pointer and pitch
    //virtual bool Lock(LockType lockType, int level, LockInfo& lockInfo);
    /// unlock 2d texture
    //virtual void Unlock(int level);
    /// lock a cube face
    //virtual bool LockCubeFace(LockType lockType, CubeFace face, int level, LockInfo& lockInfo);
    /// unlock a cube face
    //virtual void UnlockCubeFace(CubeFace face, int level);
    int ApplyCoords(int stage, GLint size, GLsizei stride, GLvoid *pointer);
    int UnApplyCoords(int stage);

protected:
    /// load texture resource (create rendertarget if render target resource)
    virtual bool LoadResource();
    /// unload texture resource
    virtual void UnloadResource();

private:
    friend class nGLServer2;
    friend class nCgFXShader;

    /// get GL texture ID
    GLuint GetTexID() const;
    /// create a render target texture
    bool CreateRenderTarget();
    /// create an empty 2d or cube texture
    bool CreateEmptyTexture();
    /// load texture through D3DX
    //bool LoadD3DXFile();
    /// load from a raw compound file
    bool LoadFromRawCompoundFile();
    /// load from dds compound file
    bool LoadFromDDSCompoundFile();
    /// load a texture through DevIL
    bool LoadILFile();
    /// get attributes from gl texture and update my own attributes from them
    //void QueryGLTextureAttributes();
    /// create a P-Buffer
    bool CreatePBuffer(int *pf_attr, int *pb_attr);

    nAutoRef<nGLServer2> refGfxServer;
    nAutoRef<nFileServer2> refFileServer;

    GLuint texID;
    GLenum target;
    HPBUFFERARB hPBuffer;
    HDC hPBufferDC;
    HGLRC hPBufferRC;
};

//------------------------------------------------------------------------------
/**
*/
inline
GLuint
nGLTexture::GetTexID() const
{
    return this->texID;
}

//------------------------------------------------------------------------------
#endif

