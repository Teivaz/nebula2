#ifndef N_D3D9TEXTURE_H
#define N_D3D9TEXTURE_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Texture
    @ingroup NebulaD3D9GraphicsSystem

    Direct3D9 subclass for nTexture2.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_TEXTURE2_H
#include "gfx2/ntexture2.h"
#endif

#ifndef N_D3D9SERVER_H
#include "gfx2/nd3d9server.h"
#endif

#undef N_DEFINES
#define N_DEFINES nTexture2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nD3D9Texture : public nTexture2
{
public:
    /// constructor
    nD3D9Texture();
    /// destructor
    virtual ~nD3D9Texture();
    /// load texture resource (create rendertarget if render target resource)
    virtual bool Load();
    /// unload texture resource
    virtual void Unload();

    static nKernelServer* kernelServer;

private:
    friend class nD3D9Server;
    friend class nD3D9Shader;

    /// get d3d8 texture
    IDirect3DBaseTexture9* GetTexture() const;
    /// get render target surface (returns 0 if no render target)
    IDirect3DSurface9* GetRenderTarget() const;
    /// get optional rendertarget depth stencil surface
    IDirect3DSurface9* GetDepthStencil() const;
    /// verify pixelformat of rendertarget
    bool CheckRenderTargetFormat(IDirect3D9* d3d9, IDirect3DDevice9* d3d9Device, DWORD usage, D3DFORMAT pixelFormat);

    /// create a render target texture
    bool CreateRenderTarget();
    /// load a ntx texture
    bool LoadNtxFile();
    /// load texture through D3DX
    bool LoadD3DXFile();
    /// load a texture through DevIL
    bool LoadILFile();
    /// get attributes from d3d texture and update my own attributes from them
    void QueryD3DTextureAttributes();

    nAutoRef<nD3D9Server> refGfxServer;
    
    IDirect3DBaseTexture9* baseTexture;
    IDirect3DTexture9* texture2D;
    IDirect3DCubeTexture9* textureCube;
    IDirect3DTexture9* depthStencil;
    IDirect3DSurface9* renderTargetSurface;
    IDirect3DSurface9* depthStencilSurface;
};

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DBaseTexture9*
nD3D9Texture::GetTexture() const
{
    return this->baseTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DSurface9*
nD3D9Texture::GetRenderTarget() const
{
    return this->renderTargetSurface;
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirect3DSurface9*
nD3D9Texture::GetDepthStencil() const
{
    return this->depthStencilSurface;
}

//------------------------------------------------------------------------------
#endif
 
