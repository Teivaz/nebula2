//------------------------------------------------------------------------------
//  nd3d9texture_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include <d3d9.h>
#include "gfx2/nsurface.h"
#include "gfx2/nd3d9surface.h"
#include "gfx2/nd3d9texture.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

nNebulaClass(nD3D9Texture, "ntexture2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Texture::nD3D9Texture() :
    refGfxServer("/sys/servers/gfx"),
    baseTexture(0),
    texture2D(0),
    textureCube(0),
    renderTargetSurface(0),
    depthStencilSurface(0)
{   
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nD3D9Texture::~nD3D9Texture()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    nD3D9Texture support asynchronous resource loading.
*/
bool
nD3D9Texture::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Texture::UnloadResource()
{
    n_assert(this->IsValid());

    nD3D9Server* gfxServer = this->refGfxServer.get();
    n_assert(gfxServer->d3d9Device);

    // check if I am the current render target in the gfx server...
    if (this->IsRenderTarget())
    {
        if (gfxServer->GetRenderTarget() == this)
        {
            gfxServer->SetRenderTarget(0);
        }
    }

    // check if I am one of the currently active textures
    int curStage;
    for (curStage = 0; curStage < nGfxServer2::MaxTextureStages; curStage++)
    {
        if (gfxServer->GetTexture(curStage) == this)
        {
            gfxServer->SetTexture(curStage, 0);
        }
    }

    // release d3d resources
    if (this->renderTargetSurface)
    {
        int ref = this->renderTargetSurface->Release();
        this->renderTargetSurface = 0;
    }
    if (this->depthStencilSurface)
    {
        int ref = this->depthStencilSurface->Release();
        this->depthStencilSurface = 0;
    }
    if (this->baseTexture)
    {
        int ref = this->baseTexture->Release();
        this->baseTexture = 0;
    }
    if (this->texture2D)
    {
        int ref = this->texture2D->Release();
        this->texture2D = 0;
    }
    if (this->textureCube)
    {
        int ref = this->textureCube->Release();
        this->textureCube = 0;
    }

    this->SetValid(false);
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9Texture::LoadResource()
{
    n_assert(!this->IsValid());

    bool success = false;
    nPathString filename = this->GetFilename().Get();

    if (this->IsRenderTarget())
    {
        // create a render target
        success = this->CreateRenderTarget();
    }
    else if (this->GetUsage() & CreateFromRawCompoundFile)
    {
        success = this->LoadFromRawCompoundFile();
    }
    else if (this->GetUsage() & CreateFromDDSCompoundFile)
    {
        success = this->LoadFromDDSCompoundFile();
    }
    else if (this->GetUsage() & CreateEmpty)
    {
        // create an empty texture
        success = this->CreateEmptyTexture();
    }
    else if (filename.CheckExtension("dds"))
    {
        // load file through D3DX, assume file has mip maps 
        success = this->LoadD3DXFile(false);
    }
    else
    {
        // load file through D3DX and generate mip maps
        success = this->LoadD3DXFile(true);
    }
    this->SetValid(success);
    return success;
}

//------------------------------------------------------------------------------
/**
    Check a pixel or depth stencil format for compatibility on the current
    device.

    @param  d3d9            pointer to Direct3D9 inteface
    @param  d3d9Device      pointer to Direct3D9 device
    @param  usage           D3DUSAGE_DEPTHSTENCIL or D3DUSAGE_RENDERTARGET 
    @param  pixelFormat     D3DFORMAT member
*/
bool
nD3D9Texture::CheckRenderTargetFormat(IDirect3D9* d3d9, 
                                      IDirect3DDevice9* d3d9Device,
                                      DWORD usage, 
                                      D3DFORMAT pixelFormat)
{
    n_assert(d3d9);
    n_assert(d3d9Device);
    HRESULT hr;

    // get current display mode
    D3DDISPLAYMODE dispMode;
    hr = d3d9Device->GetDisplayMode(0, &dispMode);
    n_assert(SUCCEEDED(hr));

    // check format
    hr = d3d9->CheckDeviceFormat(D3DADAPTER_DEFAULT,
                                 D3DDEVTYPE_HAL,
                                 dispMode.Format,
                                 usage,
                                 D3DRTYPE_SURFACE,
                                 pixelFormat);
    return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
/**
    Create d3d render target texture, and optionally a depthStencil surface

    FIXME: choosing the depthStencil pixel format is not as flexible as
    the code in nD3D9Server
*/
bool
nD3D9Texture::CreateRenderTarget()
{
    n_assert(this->width > 0);
    n_assert(this->height > 0);
    n_assert(0 == this->texture2D);
    n_assert(0 == this->depthStencilSurface);
    n_assert(this->IsRenderTarget());
    HRESULT hr;

    IDirect3D9* d3d9 = this->refGfxServer->d3d9;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9);
    n_assert(d3d9Dev);

    // create render target color surface
    if (this->usage & RenderTargetColor)
    {
        // get d3d compatible pixel format
        D3DFORMAT colorFormat;
        switch (this->format)
        {
            case X8R8G8B8:      colorFormat = D3DFMT_X8R8G8B8;      break;
            case A8R8G8B8:      colorFormat = D3DFMT_A8R8G8B8;      break;
            case R16F:          colorFormat = D3DFMT_R16F;          break;
            case G16R16F:       colorFormat = D3DFMT_G16R16F;       break;
            case A16B16G16R16F: colorFormat = D3DFMT_A16B16G16R16F; break;
            case R32F:          colorFormat = D3DFMT_R32F;          break;
            case G32R32F:       colorFormat = D3DFMT_G32R32F;       break;
            case A32B32G32R32F: colorFormat = D3DFMT_A32B32G32R32F; break;
            default:
                n_error("nD3D9Texture: invalid render target pixel format!\n");
                return false;
        }

        // make sure the format is valid as render target
        if (!this->CheckRenderTargetFormat(d3d9, d3d9Dev, D3DUSAGE_RENDERTARGET, colorFormat))
        {
            n_error("nD3D9Texture: Could not create render target surface!\n");
            return false;
        }

        // create render target surface
        hr = d3d9Dev->CreateTexture(
            this->width,                // Width
            this->height,               // Height
            1,                          // Levels
            D3DUSAGE_RENDERTARGET,      // Usage
            colorFormat,                // Format
            D3DPOOL_DEFAULT,            // Pool (must be default)
            &(this->texture2D),
            NULL);
        n_assert(SUCCEEDED(hr));
        n_assert(this->texture2D);

        // get base texture interface pointer
        hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_assert(SUCCEEDED(hr));

        // get pointer to highest mipmap surface
        hr = this->texture2D->GetSurfaceLevel(0, &(this->renderTargetSurface));
        n_assert(SUCCEEDED(hr));
    }

    // create optional render target depthStencil surface
    if (this->usage & RenderTargetDepth)
    {
        // always create a 24 bit depth buffer
        D3DFORMAT depthFormat = D3DFMT_D24X8;

        // make sure the format is compatible on this device
        if (!this->CheckRenderTargetFormat(d3d9, d3d9Dev, D3DUSAGE_DEPTHSTENCIL, depthFormat))
        {
            n_error("nD3D9Texture: Could not create render target surface!\n");
            return false;
        }

        // create render target surface
        hr = d3d9Dev->CreateDepthStencilSurface(
            this->width,                // Width
            this->height,               // Height
            depthFormat,                // Format
            D3DMULTISAMPLE_NONE,        // MultiSampleType
            0,                          // MultiSampleQuality
            TRUE,                       // Discard
            &(this->depthStencilSurface),
            NULL);
        n_assert(SUCCEEDED(hr));
        n_assert(this->depthStencilSurface);
    }

    return true;
}


//------------------------------------------------------------------------------
/**
    Query the texture attributes from the D3D texture object and
    update my own attributes. This method is called by LoadD3DXFile() and
    LoadILFile().
*/
void
nD3D9Texture::QueryD3DTextureAttributes()
{
    n_assert(this->GetType() != TEXTURE_NOTYPE);

    if (this->GetType() == TEXTURE_2D)
    {
        n_assert(this->texture2D);
        HRESULT hr;

        // set texture attributes
        D3DSURFACE_DESC desc;
        hr = this->texture2D->GetLevelDesc(0, &desc);
        n_assert(SUCCEEDED(hr));

        switch(desc.Format)
        {
            case D3DFMT_R8G8B8:
            case D3DFMT_X8R8G8B8:       this->SetFormat(X8R8G8B8); break;
            case D3DFMT_A8R8G8B8:       this->SetFormat(A8R8G8B8); break;
            case D3DFMT_R5G6B5:
            case D3DFMT_X1R5G5B5:       this->SetFormat(R5G6B5); break;
            case D3DFMT_A1R5G5B5:       this->SetFormat(A1R5G5B5); break;
            case D3DFMT_A4R4G4B4:       this->SetFormat(A4R4G4B4); break;
            case D3DFMT_DXT1:           this->SetFormat(DXT1); break;
            case D3DFMT_DXT2:           this->SetFormat(DXT2); break;
            case D3DFMT_DXT3:           this->SetFormat(DXT3); break;
            case D3DFMT_DXT4:           this->SetFormat(DXT4); break;
            case D3DFMT_DXT5:           this->SetFormat(DXT5); break;
            case D3DFMT_R16F:           this->SetFormat(R16F); break;
            case D3DFMT_G16R16F:        this->SetFormat(G16R16F); break;
            case D3DFMT_A16B16G16R16F:  this->SetFormat(A16B16G16R16F); break;
            case D3DFMT_R32F:           this->SetFormat(R32F); break;
            case D3DFMT_G32R32F:        this->SetFormat(G32R32F); break;
            case D3DFMT_A32B32G32R32F:  this->SetFormat(A32B32G32R32F); break;
        }
        this->SetWidth(desc.Width);
        this->SetHeight(desc.Height);
        this->SetDepth(1);
        this->SetNumMipLevels(this->texture2D->GetLevelCount());
    }
    else if (this->GetType() == TEXTURE_3D)
    {
        n_error("3D textures not implemented yet.\n");
    }
    else if (this->GetType() == TEXTURE_CUBE)
    {
        n_assert(this->textureCube);
        HRESULT hr;

        // set texture attributes
        D3DSURFACE_DESC desc;
        hr = this->textureCube->GetLevelDesc(0, &desc);
        n_assert(SUCCEEDED(hr));

        switch(desc.Format)
        {
            case D3DFMT_R8G8B8:
            case D3DFMT_X8R8G8B8:    this->SetFormat(X8R8G8B8); break;
            case D3DFMT_A8R8G8B8:    this->SetFormat(A8R8G8B8); break;
            case D3DFMT_R5G6B5:
            case D3DFMT_X1R5G5B5:    this->SetFormat(R5G6B5); break;
            case D3DFMT_A1R5G5B5:    this->SetFormat(A1R5G5B5); break;
            case D3DFMT_A4R4G4B4:    this->SetFormat(A4R4G4B4); break;
            case D3DFMT_DXT1:        this->SetFormat(DXT1); break;
            case D3DFMT_DXT2:        this->SetFormat(DXT2); break;
            case D3DFMT_DXT3:        this->SetFormat(DXT3); break;
            case D3DFMT_DXT4:        this->SetFormat(DXT4); break;
            case D3DFMT_DXT5:        this->SetFormat(DXT5); break;
            case D3DFMT_R16F:           this->SetFormat(R16F); break;
            case D3DFMT_G16R16F:        this->SetFormat(G16R16F); break;
            case D3DFMT_A16B16G16R16F:  this->SetFormat(A16B16G16R16F); break;
            case D3DFMT_R32F:           this->SetFormat(R32F); break;
            case D3DFMT_G32R32F:        this->SetFormat(G32R32F); break;
            case D3DFMT_A32B32G32R32F:  this->SetFormat(A32B32G32R32F); break;
        }
        this->SetWidth(desc.Width);
        this->SetHeight(desc.Height);
        this->SetDepth(1);
        this->SetNumMipLevels(this->textureCube->GetLevelCount());
    }
    else
    {
        n_assert("nD3D9Texture: unknown texture type in QueryD3DTextureAttributes()");
    }
}

//------------------------------------------------------------------------------
/**
    Create texture from file via D3DX.
*/
bool
nD3D9Texture::LoadD3DXFile(bool genMipMaps)
{
    n_assert(0 == this->baseTexture);
    n_assert(0 == this->texture2D);
    n_assert(0 == this->textureCube);

    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9Dev);
 
    // mangle pathname
    char mangledPath[N_MAXPATH];
    this->refFileServer->ManglePath(this->GetFilename().Get(), mangledPath, sizeof(mangledPath));

    // check whether this is a 2d texture or a cube texture
    D3DXIMAGE_INFO imgInfo = { 0 };
    hr = D3DXGetImageInfoFromFile(mangledPath, &imgInfo);
    if (FAILED(hr))
    {
        n_error("nD3D9Texture::LoadD3DXFile(): Failed to open texture file '%s'\n", mangledPath);
        return false;
    }

    // Generate mipmaps?
    DWORD mipmapFilter = D3DX_FILTER_NONE;
    if (genMipMaps)
    {
        mipmapFilter = D3DX_DEFAULT;
    }

    // D3D usage flags
    DWORD d3dUsage = 0;
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    if (this->usage & Dynamic)
    {
        d3dUsage = D3DUSAGE_DYNAMIC;
        d3dPool  = D3DPOOL_DEFAULT;
    }

    if (D3DRTYPE_TEXTURE == imgInfo.ResourceType)
    {
        // load 2D texture
        hr = D3DXCreateTextureFromFileEx(
                d3d9Dev,                    // pDevice
                mangledPath,                // pSrcFile
                D3DX_DEFAULT,               // Width
                D3DX_DEFAULT,               // Height
                D3DX_DEFAULT,               // MipLevels
                d3dUsage,                   // Usage
                D3DFMT_UNKNOWN,             // Format
                d3dPool,                    // Pool
                D3DX_FILTER_NONE,           // Filter
                mipmapFilter,               // MipFilter
                0,                          // ColorKey
                0,                          // pSrcInfo
                0,                          // pPalette
                &(this->texture2D));
        if (FAILED(hr))
        {
            n_error("nD3D9Texture::LoadD3DXFile(): Failed to load 2D texture '%s'\n", mangledPath);
            return false;
        }
        this->SetType(TEXTURE_2D);
        hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_assert(SUCCEEDED(hr));
    }
    else if (D3DRTYPE_CUBETEXTURE == imgInfo.ResourceType)
    {
        // load cube texture
        hr = D3DXCreateCubeTextureFromFileEx(
                d3d9Dev,                    // pDevice
                mangledPath,                // pSrcFile
                D3DX_DEFAULT,               // Size
                D3DX_DEFAULT,               // MipLevels
                d3dUsage,                   // Usage
                D3DFMT_UNKNOWN,             // Format
                d3dPool,                    // Pool
                D3DX_FILTER_NONE,           // Filter
                D3DX_FILTER_NONE,           // MipFilter
                0,                          // ColorKey
                0,                          // pSrcInfo
                0,                          // pPalette
                &(this->textureCube));
        if (FAILED(hr))
        {
            n_error("nD3D9Texture::LoadD3DXFile(): Failed to load cube texture '%s'\n", mangledPath);
            return false;
        }
        this->SetType(TEXTURE_CUBE);
        hr = this->textureCube->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_assert(SUCCEEDED(hr));
    }
    else
    {
        // unsupported texture type
        n_error("nD3D9Texture::LoadD3DXFile(): Unsupported texture type (cube texture?) in file '%s'\n", mangledPath);
        return false;
    }
    this->baseTexture->PreLoad();

    // query texture attributes 
    this->QueryD3DTextureAttributes();
    return true;
}

//------------------------------------------------------------------------------
/**
    Create an empty 2D or cube texture (without mipmaps!).
*/
bool
nD3D9Texture::CreateEmptyTexture()
{
    n_assert(this->GetWidth() > 0);
    n_assert(this->GetHeight() > 0);
    n_assert(this->GetType() != TEXTURE_NOTYPE);
    n_assert(this->GetFormat() != NOFORMAT);
    n_assert(0 == this->texture2D);
    n_assert(0 == this->textureCube);

    HRESULT hr;
    IDirect3D9* d3d9 = this->refGfxServer->d3d9;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9);
    n_assert(d3d9Dev);

    DWORD d3dUsage = 0;
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    if (this->usage & Dynamic)
    {
        d3dUsage = D3DUSAGE_DYNAMIC;
        d3dPool  = D3DPOOL_DEFAULT;
    }
    D3DFORMAT d3dFormat;
    switch (this->format)
    {
        case X8R8G8B8:          d3dFormat = D3DFMT_X8R8G8B8; break;
        case A8R8G8B8:          d3dFormat = D3DFMT_A8R8G8B8; break;
        case R5G6B5:            d3dFormat = D3DFMT_R5G6B5; break;
        case A1R5G5B5:          d3dFormat = D3DFMT_A1R5G5B5; break;
        case A4R4G4B4:          d3dFormat = D3DFMT_A4R4G4B4; break;
        case P8:                d3dFormat = D3DFMT_P8; break;
        case DXT1:              d3dFormat = D3DFMT_DXT1; break;
        case DXT2:              d3dFormat = D3DFMT_DXT2; break;
        case DXT3:              d3dFormat = D3DFMT_DXT3; break;
        case DXT4:              d3dFormat = D3DFMT_DXT4; break;
        case DXT5:              d3dFormat = D3DFMT_DXT5; break;
        case R16F:              d3dFormat = D3DFMT_R16F; break;
        case G16R16F:           d3dFormat = D3DFMT_G16R16F; break;
        case A16B16G16R16F:     d3dFormat = D3DFMT_A16B16G16R16F; break;
        case R32F:              d3dFormat = D3DFMT_R32F; break;
        case G32R32F:           d3dFormat = D3DFMT_G32R32F; break;
        case A32B32G32R32F:     d3dFormat = D3DFMT_A32B32G32R32F; break;
        case A8:                d3dFormat = D3DFMT_A8; break;
        default:            
            // can't happen
            n_assert(false);
    }

    if (this->GetType() == TEXTURE_2D)
    {
        hr = D3DXCreateTexture(d3d9Dev,             // pDevice
                               this->GetWidth(),    // Width
                               this->GetHeight(),   // Height
                               1,                   // MipLevels
                               d3dUsage,            // Usage
                               d3dFormat,           // Format
                               d3dPool,             // Pool
                               &(this->texture2D));
        if (FAILED(hr))
        {
            n_error("nD3D9Texture::CreateEmptyTexture(): Failed to create 2D texture!\n");
            return false;
        }
        hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_assert(SUCCEEDED(hr));
    }
    else if (this->GetType() == TEXTURE_CUBE)
    {
        n_assert(this->GetWidth() == this->GetHeight());
        hr = D3DXCreateCubeTexture(d3d9Dev,             // pDevice
                                   this->GetWidth(),    // Size
                                   1,                   // MipLevels
                                   d3dUsage,            // Usage
                                   d3dFormat,           // Format
                                   d3dPool,             // Pool
                                   &(this->textureCube));
        if (FAILED(hr))
        {
            n_error("nD3D9Texture::CreateEmptyTexture(): Failed to create cube texture!\n");
            return false;
        }
        hr = this->textureCube->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_assert(SUCCEEDED(hr));
    }
    else
    {
        // unsupported texture type
        n_error("nD3D9Texture::CreateEmptyTexture(): Unsupported texture type (cube texture?!\n");
        return false;
    }

    // query texture attributes 
    this->QueryD3DTextureAttributes();
    return true;
}

//------------------------------------------------------------------------------
/**
    Create texture and load contents from as "raw" pixel chunk from
    inside a compound file.
*/
bool
nD3D9Texture::LoadFromRawCompoundFile()
{
    // create texture...
    if (this->CreateEmptyTexture())
    {
        // read data into texture
        nTexture2::LockInfo lockInfo;
        if (this->Lock(nTexture2::WriteOnly, 0, lockInfo))
        {
            const int bytesToRead = this->GetWidth() * this->GetHeight() * this->GetBytesPerPixel();
            n_assert(lockInfo.surfPitch == (this->GetWidth() * this->GetBytesPerPixel()));

            // get seek position from Toc
            n_assert(this->compoundFile);
            this->compoundFile->Seek(this->compoundFilePos, nFile::START);
            int numBytesRead = this->compoundFile->Read(lockInfo.surfPointer, bytesToRead);
            n_assert(numBytesRead == bytesToRead);

            this->Unlock(0);
            return true;
        }        
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Load texture as dds file from inside a compound file.
    FIXME: Add cube texture support?
*/
bool
nD3D9Texture::LoadFromDDSCompoundFile()
{
    n_assert(this->compoundFile);
    HRESULT hr;

    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9Dev);

    // allocate temp buffer of requested size
    void* buffer = n_malloc(this->compoundFileDataSize);
    n_assert(buffer);

    // seek to start of data
    this->compoundFile->Seek(this->compoundFilePos, nFile::START);
    int bytesRead = this->compoundFile->Read(buffer, this->compoundFileDataSize);
    n_assert(bytesRead == this->compoundFileDataSize);

    // D3D usage flags
    DWORD d3dUsage = 0;
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    if (this->usage & Dynamic)
    {
        d3dUsage = D3DUSAGE_DYNAMIC;
        d3dPool  = D3DPOOL_DEFAULT;
    }

    // create texture from memory buffer
    hr = D3DXCreateTextureFromFileInMemoryEx(d3d9Dev,                       // pDevice
                                             buffer,                        // pSrcData
                                             this->compoundFileDataSize,    // SrcDataSize
                                             D3DX_DEFAULT,                  // Width
                                             D3DX_DEFAULT,                  // Height
                                             D3DX_DEFAULT,                  // MipLevels
                                             d3dUsage,                      // Usage
                                             D3DFMT_UNKNOWN,                // Format
                                             d3dPool,                       // Pool
                                             D3DX_FILTER_NONE,              // Filter
                                             D3DX_FILTER_NONE,              // MipFilter
                                             0,                             // ColorKey
                                             0,                             // pSrcInfo
                                             NULL,                          // pPalette
                                             &(this->texture2D));
    if (FAILED(hr))
    {
        n_error("nD3D9Texture::LoadFromDDSCompoundFile(): Failed to load 2D texture!\n");
        return false;
    }
    this->SetType(TEXTURE_2D);
    hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
    n_assert(SUCCEEDED(hr));

    // free temp buffer
    n_free(buffer);
    buffer = 0;

    // query texture attributes 
    this->QueryD3DTextureAttributes();

    return true;
}

//------------------------------------------------------------------------------
/**
    Locks the 2D texture surface and returns a pointer to the
    image data and the pitch of the surface (refer to the DX9 docs
    for details). Call Unlock() to unlock the texture after accessing it.

    @param  lockType    defines the intended access to the surface (Read, Write)
    @param  level       the mip level
    @param  lockInfo    will be filled with surface pointer and pitch
    @return             true if surface has been locked successfully
*/
bool
nD3D9Texture::Lock(LockType lockType, int level, LockInfo& lockInfo)
{
    this->LockMutex();
    n_assert(this->GetType() == TEXTURE_2D);
    n_assert(this->texture2D);

    DWORD d3dLockFlags = 0;
    switch (lockType)
    {
        case ReadOnly:
            d3dLockFlags = D3DLOCK_READONLY;
            break;

        case WriteOnly:
            d3dLockFlags = D3DLOCK_NO_DIRTY_UPDATE;
            break;
    }

    bool retval = false;
    D3DLOCKED_RECT d3dLockedRect = { 0 };
    HRESULT hr = this->texture2D->LockRect(level, &d3dLockedRect, NULL, d3dLockFlags);
    if (SUCCEEDED(hr))
    {
        lockInfo.surfPointer = d3dLockedRect.pBits;
        lockInfo.surfPitch   = d3dLockedRect.Pitch;
        retval = true;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Locks all cube texture surfaces and returns pointers to the image data
    and surface pitches. Call Unlock() to unlock the texture after accessing it.

    @param  lockType    defines the intended access to the surface (Read, Write)
    @param  level       the mip level
    @param  lockInfo    array of size 6 which will be filled with surface pointers and pitches
    @return             true if surface has been locked successfully
*/
bool
nD3D9Texture::LockCubeFace(LockType lockType, CubeFace face, int level, LockInfo& lockInfo)
{
    this->LockMutex();
    n_assert(this->GetType() == TEXTURE_CUBE);
    n_assert(this->textureCube);

    DWORD d3dLockFlags = 0;
    switch (lockType)
    {
        case ReadOnly:
            d3dLockFlags = D3DLOCK_READONLY;
            break;

        case WriteOnly:
            d3dLockFlags = 0;
            break;
    }

    bool retval = false;
    D3DLOCKED_RECT d3dLockedRect = { 0 };
    HRESULT hr = this->textureCube->LockRect((D3DCUBEMAP_FACES) face, level, &d3dLockedRect, NULL, d3dLockFlags);
    if (SUCCEEDED(hr))
    {
        lockInfo.surfPointer = d3dLockedRect.pBits;
        lockInfo.surfPitch   = d3dLockedRect.Pitch;
        retval = true;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock the 2D texture.
*/
void
nD3D9Texture::Unlock(int level)
{
    this->LockMutex();
    n_assert(this->GetType() == TEXTURE_2D);
    n_assert(this->texture2D);
    HRESULT hr = this->texture2D->UnlockRect(level);
    n_assert(SUCCEEDED(hr));
    this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Unlock a cube texture face.
*/
void
nD3D9Texture::UnlockCubeFace(CubeFace face, int level)
{
    this->LockMutex();
    n_assert(this->GetType() == TEXTURE_CUBE);
    n_assert(this->textureCube);
    HRESULT hr = this->textureCube->UnlockRect((D3DCUBEMAP_FACES) face, level);
    n_assert(SUCCEEDED(hr));
    this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**

    @param objName  name of nSurafce which to be created.
    @param level    mipmap level
    @param surface

    - Feb-04 Kim, H.W. added to support ngameswf.
*/
void nD3D9Texture::GetSurfaceLevel(const char* objName, int level, nSurface** surface)
{
    n_assert(this->texture2D);

    HRESULT hr;

    *surface = (nSurface*)this->kernelServer->New ("nd3d9surface", objName);
    n_assert(*surface != NULL);

    hr = this->texture2D->GetSurfaceLevel(0, &((nD3D9Surface*)*surface)->baseSurface);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
    Generante mip maps for surface data.

    - Feb-04 Kim, H.W. added to support ngameswf.
*/
void nD3D9Texture::GenerateMipMaps()
{
    n_assert(this->texture2D);

    HRESULT hr;

    hr = D3DXFilterTexture(this->texture2D,    // pTexture
                           NULL,               // pPalette
                           D3DX_DEFAULT,       // SrcLevel(0)
                           D3DX_FILTER_LINEAR);// MipFilter

    n_assert (SUCCEEDED(hr));
}
                
//------------------------------------------------------------------------------
/**             
    Compute the byte size of the texture data.
*/          
int         
nD3D9Texture::GetByteSize()
{               
    if (this->IsValid())
    {
        // compute number of pixels
        int numPixels = this->GetWidth() * this->GetHeight();
                
        // 3d or cube texture?
        switch (this->GetType())
        {
            case TEXTURE_3D:   numPixels *= this->GetDepth(); break;
            case TEXTURE_CUBE: numPixels *= 6; break;
            default: break;
        }
    
        // mipmaps ?
        if (this->GetNumMipLevels() > 1)
        {
            switch (this->GetType())
            {
                case TEXTURE_2D:
                case TEXTURE_CUBE:
                    numPixels += numPixels / 3;
                    break;

                default:
                    /// 3d texture
                    numPixels += numPixels / 7;
                    break;
            }
        }

        // size per pixel
        int size = 0;
        switch (this->GetFormat())
        {
            case DXT1:
                // 4 bits per pixel
                size = numPixels / 2;
                break;

            case DXT2:
            case DXT3:
            case DXT4:
            case DXT5:
            case P8:
                // 8 bits per pixel
                size = numPixels;
                break;

            case R5G6B5:
            case A1R5G5B5:
            case A4R4G4B4:
            case R16F:
                // 16 bits per pixel
                size = numPixels * 2;
                break;

            case X8R8G8B8:
            case A8R8G8B8:
            case R32F:
            case G16R16F:
                // 32 bits per pixel
                size = numPixels * 4;
                break;

            case A16B16G16R16F:
            case G32R32F:
                // 64 bits per pixel
                size = numPixels * 8;
                break;

            case A32B32G32R32F:
                // 128 bits per pixel
                size = numPixels * 16;
                break;
        }
        return size;
    }
    else
    {
        return 0;
    }
}

