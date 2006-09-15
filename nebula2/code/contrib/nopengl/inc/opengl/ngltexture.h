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

//#include "opengl/nglincludes.h"

struct DDSHeader;

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
    virtual bool Lock(LockType lockType, int level, LockInfo& lockInfo);
    /// unlock 2d texture
    virtual void Unlock(int level);
    /// lock a cube face
    virtual bool LockCubeFace(LockType lockType, CubeFace face, int level, LockInfo& lockInfo);
    /// unlock a cube face
    virtual void UnlockCubeFace(CubeFace face, int level);
    /// get an estimated byte size of the resource data (for memory statistics)
    virtual int GetByteSize();
    /// save texture to file
    //virtual bool SaveTextureToFile(const nString &filename);

    /// filters mipmap levels of a texture.
    virtual void GenerateMipMaps();

    int ApplyCoords(int stage, GLint size, GLsizei stride, GLvoid *pointer);
    int UnApplyCoords(int stage);

protected:
    /// load texture resource (create rendertarget if render target resource)
    virtual bool LoadResource();
    /// unload texture resource
    virtual void UnloadResource();
    /// called when contained resource may become lost 
    virtual void OnLost();
    /// called when contained resource may be restored
    virtual void OnRestored();

private:
    friend class nGLServer2;
    friend class nGLSLShader; //    friend class nCgFXShader;

    /// create a render target texture
    bool CreateRenderTarget();
    /// create an empty 2d or cube texture
    bool CreateEmptyTexture();
    /// load texture through D3DX
    //bool LoadD3DXFile(bool genMipMaps);
    /// load from a raw compound file
    bool LoadFromRawCompoundFile();
    /// load from dds compound file
    bool LoadFromDDSCompoundFile();
    /// get attributes from gl texture and update my own attributes from them
    //void QueryGLTextureAttributes();

    struct GLTexParams
    {
        GLint iformat;
        GLenum tformat;    // texture format
        GLenum ttype;
        int bytesPerPixel;
        bool compressed;
        bool usingAlpha;
        nArray<int> pfa;
        nArray<int> pba;
    };

    /// load from dds compound file
    bool ParseDDSHeader(DDSHeader* header, GLTexParams* params);

    /// get GL texture ID
    GLuint GetTexID() const;
    /// create a P-Buffer
    bool CreatePBuffer(const int *pfa, const int *pba);
    /// load a texture through DevIL
    bool LoadILFile();

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

