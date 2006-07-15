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
    @class nGLTexture
    @ingroup OpenGL

    OpenGL subclass for nTexture2.

    - 21-Jun-2003 cubejk  created
    - Dec-2003 Haron      + P-Buffer and render_texture support
    - Jan-2004 Haron      + compressed (DDS) texture support
*/
#include "gfx2/ntexture2.h"

#ifdef __MACOSX__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    //#include <Carbon/carbon.h>
    #include <AGL/agl.h>
#else
    #include <GL/gl.h>
#endif

#ifdef __LINUX__
    #include <GL/glx.h>
#endif

#include <GL/glext.h>

#ifdef __WIN32__
    #include <GL/wglext.h>
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

