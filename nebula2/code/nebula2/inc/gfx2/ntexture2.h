#ifndef N_TEXTURE2_H
#define N_TEXTURE2_H
//------------------------------------------------------------------------------
/**
    @class nTexture2

    Contains image data used by the gfx api's texture samplers. Textures
    are normal named, shared resources which are usually loaded from disk.
    Textures can also be declared as render targets.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_RESOURCE_H
#include "resource/nresource.h"
#endif

#undef N_DEFINES
#define N_DEFINES nTexture2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nTexture2 : public nResource
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
    };

    /// constructor
    nTexture2();
    /// destructor
    virtual ~nTexture2();
    /// initialize as render target
    void SetRenderTarget(bool hasColor, bool hasDepth, bool hasStencil);
    /// is a render target?
    bool IsRenderTarget() const;
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

    static nKernelServer* kernelServer;

protected:
    /// set number of mipmaps
    void SetNumMipLevels(int num);

    enum
    {
        RENDERTARGET_HASCOLOR = (1<<0),
        RENDERTARGET_HASDEPTH = (1<<1),
        RENDERTARGET_HASSTENCIL = (1<<2),
    };

    Type type;
    Format format;
    ushort width;
    ushort height;
    ushort depth;
    ushort numMipMaps;
    ushort renderTargetFlags;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture2::SetRenderTarget(bool hasColor, bool hasDepth, bool hasStencil)
{
    this->renderTargetFlags = 0;
    if (hasColor)
    {
        this->renderTargetFlags |= RENDERTARGET_HASCOLOR;
    }
    if (hasDepth)
    {
        this->renderTargetFlags |= RENDERTARGET_HASDEPTH;
    }
    if (hasStencil)
    {
        this->renderTargetFlags |= RENDERTARGET_HASSTENCIL;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTexture2::IsRenderTarget() const
{
    return (this->renderTargetFlags != 0);
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
#endif    

