#ifndef N_TEXTURE2_H
#define N_TEXTURE2_H
//------------------------------------------------------------------------------
/**
    @class nTexture2
    @ingroup NebulaGraphicsSystem

    Contains image data used by the gfx api's texture samplers. Textures
    are normal named, shared resources which are usually loaded from disk.
    Textures can also be declared as render targets.

    (C) 2002 RadonLabs GmbH
*/
#include "resource/nresource.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
class nTexture2 : public nResource
{
public:
    /// texture type
    enum Type
    {
        TEXTURE_NOTYPE,
        TEXTURE_2D,                 // 2-dimensional
        TEXTURE_3D,                 // 3-dimensional
        TEXTURE_CUBE,               // cube
    };

    // pixel formats
    enum Format
    {
        NOFORMAT,
        X8R8G8B8,
        A8R8G8B8,
        R5G6B5,
        A1R5G5B5,
        A4R4G4B4,
        P8,
        DXT1,
        DXT2,
        DXT3,
        DXT4,
        DXT5,
        R16F,                       // 16 bit float, red only
        G16R16F,                    // 32 bit float, 16 bit red, 16 bit green
        A16B16G16R16F,              // 64 bit float, 16 bit rgba each
        R32F,                       // 32 bit float, red only
        G32R32F,                    // 64 bit float, 32 bit red, 32 bit green
        A32B32G32R32F,              // 128 bit float, 32 bit rgba each
    };

    // the sides of a cube map
    enum CubeFace
    {
        PosX = 0,
        NegX,
        PosY,
        NegY,
        PosZ,
        NegZ,
    };

    // usage flags
    enum Usage
    {
        CreateEmpty = (1<<0),               // don't load from disk, instead create empty texture
        CreateFromRawCompoundFile = (1<<1), // create from a compound file as raw ARGB pixel chunk
        CreateFromDDSCompoundFile = (1<<2), // create from dds file inside a compound file
        RenderTargetColor = (1<<3),         // is render target, has color buffer
        RenderTargetDepth = (1<<4),         // is render target, has depth buffer
        RenderTargetStencil = (1<<5),       // is render target, has stencil buffer
        Dynamic = (1<<6),                   // is a dynamic texture (for write access with CPU)
    };

    // lock types
    enum LockType
    {
        ReadOnly,       // cpu will only read from texture
        WriteOnly,      // cpu will only write to texture (an overwrite everything!)
    };

    // lock information
    struct LockInfo
    {
        void* surfPointer;
        int   surfPitch;
    };

    /// constructor
    nTexture2();
    /// destructor
    virtual ~nTexture2();
    /// set combination of usage flags
    void SetUsage(int useFlags);
    /// get usage flags combination
    int GetUsage() const;
    /// check usage flags if this is a render target
    bool IsRenderTarget() const;
    /// set compound file read data
    void SetCompoundFileData(nFile* file, int filePos, int byteSize);
    /// set texture type (render target only!)
    void SetType(Type t);
    /// get texture type
    Type GetType() const;
    /// set texture's pixel format (render target only!)
    void SetFormat(Format f);
    /// get texture's pixel format
    Format GetFormat() const;
    /// set width (render target only!)
    void SetWidth(int w);
    /// get width
    int GetWidth() const;
    /// set height (render target only!)
    void SetHeight(int h);
    /// get height
    int GetHeight() const;
    /// set depth (render target only! oops: 3d render targets?)
    void SetDepth(int d);
    /// get depth
    int GetDepth() const;
    /// get number of mipmaps
    int GetNumMipLevels() const;
    /// get bytes per pixel (computed from pixel format)
    int GetBytesPerPixel() const;
    /// lock a 2D texture, returns pointer and pitch
    virtual bool Lock(LockType lockType, int level, LockInfo& lockInfo);
    /// unlock 2D texture
    virtual void Unlock(int level);
    /// lock a cube face
    virtual bool LockCubeFace(LockType lockType, CubeFace face, int level, LockInfo& lockInfo);
    /// unlock a cube face
    virtual void UnlockCubeFace(CubeFace face, int level);

    static nKernelServer* kernelServer;

protected:
    /// set number of mipmaps
    void SetNumMipLevels(int num);

    Type type;
    Format format;
    ushort usage;
    ushort width;
    ushort height;
    ushort depth;
    ushort numMipMaps;
    nFile* compoundFile;
    int compoundFilePos;
    int compoundFileDataSize;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture2::SetCompoundFileData(nFile* file, int filePos, int byteSize)
{
    n_assert(file);
    n_assert(byteSize > 0);
    if (this->compoundFile)
    {
        this->compoundFile->Release();
        this->compoundFile = 0;
    }
    this->compoundFile = file;
    this->compoundFile->AddRef();
    this->compoundFilePos = filePos;
    this->compoundFileDataSize = byteSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture2::SetUsage(int useFlags)
{
    this->usage = useFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture2::GetUsage() const
{
    return this->usage;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTexture2::IsRenderTarget() const
{
    return (0 != (this->usage & (RenderTargetColor | RenderTargetDepth | RenderTargetStencil)));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture2::SetType(Type t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTexture2::Type
nTexture2::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture2::SetFormat(Format f)
{
    this->format = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTexture2::Format
nTexture2::GetFormat() const
{
    return this->format;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture2::SetWidth(int w)
{
    this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture2::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture2::SetHeight(int h)
{
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture2::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture2::SetDepth(int d)
{
    this->depth = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture2::GetDepth() const
{
    return this->depth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture2::SetNumMipLevels(int num)
{
    this->numMipMaps = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture2::GetNumMipLevels() const
{
    return this->numMipMaps;
}

//------------------------------------------------------------------------------
/**
    Returns the bytes per pixel for the current pixel format. May be
    incorrect for compressed textures!
*/
inline
int
nTexture2::GetBytesPerPixel() const
{
    switch (this->format)
    {
        case X8R8G8B8:  
        case A8R8G8B8:
            return 4;

        case R5G6B5:
        case A1R5G5B5:
        case A4R4G4B4:
            return 2;

        case P8:
            return 1;

        case DXT1:
        case DXT2:
        case DXT3:
        case DXT4:
        case DXT5:
            n_error("nTexture2::GetBytesPerPixel(): compressed pixel format!");
            return 1;

        case R16F:
            return 2;

        case G16R16F:
            return 4;

        case A16B16G16R16F:
            return 8;

        case R32F:
            return 4;

        case G32R32F:
            return 8;

        case A32B32G32R32F:
            return 16;
        
        default:
            n_error("nTexture2::GetBytesPerPixel(): invalid pixel format!");
            return 1;
    }
}

//------------------------------------------------------------------------------
#endif    

