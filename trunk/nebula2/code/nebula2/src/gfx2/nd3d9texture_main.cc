#define N_IMPLEMENTS nD3D9Texture
//------------------------------------------------------------------------------
//  nd3d9texture_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9texture.h"
#include "gfx2/nntxfile.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "il/il.h"
#include "il/ilu.h"

nNebulaClass(nD3D9Texture, "ntexture2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Texture::nD3D9Texture() :
    refGfxServer("/sys/servers/gfx"),
    baseTexture(0),
    texture2D(0),
    depthStencil(0),
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
*/
void
nD3D9Texture::Unload()
{
    if (this->valid)
    {
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
        for (curStage = 0; curStage < nGfxServer2::MAX_TEXTURESTAGES; curStage++)
        {
            if (gfxServer->GetTexture(curStage) == this)
            {
                gfxServer->SetTexture(curStage, 0);
            }
        }

        // release d3d resources
        if (this->renderTargetSurface)
        {
            this->renderTargetSurface->Release();
            this->renderTargetSurface = 0;
        }
        if (this->depthStencilSurface)
        {
            this->depthStencilSurface->Release();
            this->depthStencilSurface = 0;
        }
        if (this->baseTexture)
        {
            this->baseTexture->Release();
            this->baseTexture = 0;
        }
        if (this->texture2D)
        {
            this->texture2D->Release();
            this->texture2D = 0;
        }
        if (this->depthStencil)
        {
            this->depthStencil->Release();
            this->depthStencil = 0;
        }
        if (this->textureCube)
        {
            this->textureCube->Release();
            this->textureCube = 0;
        }

        this->valid = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9Texture::Load()
{
    n_assert(!this->valid);

    if (this->IsRenderTarget())
    {
        // create a render target
        this->valid = this->CreateRenderTarget();
    }
    else if (this->filename.CheckExtension("ntx"))
    {
        // load ntx file
        this->valid = this->LoadNtxFile();
    }
    else if (this->filename.CheckExtension("dds"))
    {
        // load file through D3DX
        this->valid = this->LoadD3DXFile();
    }
    else 
    {
        // load file through DevIL
        this->valid = this->LoadILFile();
    }

    // fail hard if loading failed
    if (!this->valid)
    {
        n_error("Could not open texture '%s'!\n", this->filename.Get());
    }

    return this->valid;
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
    n_assert(0 == this->depthStencil);
    n_assert(0 != this->renderTargetFlags);
    HRESULT hr;

    IDirect3D9* d3d9 = this->refGfxServer->d3d9;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9);
    n_assert(d3d9Dev);

    // create render target color surface
    if (this->renderTargetFlags & RENDERTARGET_HASCOLOR)
    {
        // get d3d compatible pixel format
        D3DFORMAT colorFormat;
        switch (this->format)
        {
            case X8R8G8B8:
                colorFormat = D3DFMT_X8R8G8B8;
                break;

            case A8R8G8B8:
                colorFormat = D3DFMT_A8R8G8B8;
                break;

            default:
                n_printf("nD3D9Texture: invalid render target pixel format!\n");
                return false;
        }

        // make sure the format is compatible on this device
        if (!this->CheckRenderTargetFormat(d3d9, d3d9Dev, D3DUSAGE_RENDERTARGET, colorFormat))
        {
            n_printf("nD3D9Texture: Could not create render target surface!\n");
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
    if (this->renderTargetFlags & RENDERTARGET_HASDEPTH)
    {
        D3DFORMAT depthFormat;
        switch (this->format)
        {
            case X8R8G8B8:
            case A8R8G8B8:
                depthFormat = D3DFMT_D24S8;
                break;

            default:
                n_printf("nD3D9Texture: invalid render target pixel format!\n");
                return false;
        }

        // make sure the format is compatible on this device
        if (!this->CheckRenderTargetFormat(d3d9, d3d9Dev, D3DUSAGE_DEPTHSTENCIL, depthFormat))
        {
            n_printf("nD3D9Texture: Could not create render target surface!\n");
            return false;
        }

        // create z-buffer surface
        hr = d3d9Dev->CreateDepthStencilSurface(
            this->width,
            this->height,
            depthFormat,
            D3DMULTISAMPLE_NONE,
            0,
            false,
            &(this->depthStencilSurface), NULL);

        n_assert(SUCCEEDED(hr));
        n_assert(this->depthStencilSurface);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Create a d3d texture from a ntx file.

     - 09-Sep-02   floh    support for cube map textures
*/
bool
nD3D9Texture::LoadNtxFile()
{
    n_assert(!this->filename.IsEmpty());
    n_assert(0 == this->baseTexture);
    n_assert(0 == this->texture2D);
    n_assert(0 == this->textureCube);

    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9Dev);

    void* imageData = 0;
    HRESULT hr;
    int numLevels;
    int dataSize;

    // create a nNtxFile object 
    nNtxFile ntxFile(kernelServer);

    // open ntx object for reading
    if (!ntxFile.OpenRead(this->filename.Get()))
    {
        n_printf("nD3D9Texture: failed to open ntx file '%s'!\n", this->filename.Get());
        return false;
    }

    // agree on a d3d9 format
    D3DFORMAT d3dSrcFormat;
    D3DFORMAT d3dDstFormat;
    Format texFormat;
    ntxFile.SetCurrentBlock(0);
    nNtxFile::Format ntxFormat = ntxFile.GetFormat();
    nNtxFile::Type ntxType = ntxFile.GetType();
    int bytesPerPixel = 0;
    switch (ntxFormat)
    {
        case nNtxFile::FORMAT_R8G8B8:
            texFormat = X8R8G8B8;
            d3dSrcFormat = D3DFMT_R8G8B8;
            d3dDstFormat = D3DFMT_X8R8G8B8;
            ntxFormat = nNtxFile::FORMAT_R8G8B8;
            bytesPerPixel = 4;
            break;

        case nNtxFile::FORMAT_R5G6B5:
            texFormat = R5G6B5;
            d3dSrcFormat = D3DFMT_R5G6B5;
            d3dDstFormat = D3DFMT_R5G6B5;
            ntxFormat = nNtxFile::FORMAT_R5G6B5;
            bytesPerPixel = 2;
            break;
            
        case nNtxFile::FORMAT_A8R8G8B8:
            texFormat = A8R8G8B8;
            d3dSrcFormat = D3DFMT_A8R8G8B8;
            d3dDstFormat = D3DFMT_A8R8G8B8;
            ntxFormat = nNtxFile::FORMAT_A8R8G8B8;
            bytesPerPixel = 4;
            break;

        case nNtxFile::FORMAT_A4R4G4B4:
            texFormat = A4R4G4B4;
            d3dSrcFormat = D3DFMT_A4R4G4B4;
            d3dDstFormat = D3DFMT_A4R4G4B4;
            ntxFormat = nNtxFile::FORMAT_A4R4G4B4;
            bytesPerPixel = 2;
            break;

    default:
            n_error("nD3D9Texture: Invalid pixel format in ntx file '%s'\n", this->filename.Get());
            return false;
    }

    switch (ntxType)
    {
        case nNtxFile::TYPE_TEXTURE2D:
            this->SetType(TEXTURE_2D);
            break;
        
        case nNtxFile::TYPE_TEXTURECUBE:
            this->SetType(TEXTURE_CUBE);
            break;

        default:
            n_error("nD3D9Texture: Unknown ntx image type in file '%s'!\n", this->filename.Get());
            break;
    }

    // set first mipmap level as current block and get texture size
    int firstBlock = ntxFile.FindBlock(ntxType, ntxFormat, 0);
    n_assert(firstBlock != -1);

    ntxFile.SetCurrentBlock(firstBlock);
    int curLevel;
    int curBlockIndex = 0;
    int imgWidth  = ntxFile.GetWidth();
    int imgHeight = ntxFile.GetHeight();

    // configure nTexture2 attributes
    this->SetFormat(texFormat);
    this->SetWidth(imgWidth);
    this->SetHeight(imgHeight);
    this->SetDepth(1);

    if (this->GetType() == TEXTURE_2D)
    {
        // create d3d texture
        hr = d3d9Dev->CreateTexture(
            imgWidth,                   // Width
            imgHeight,                  // Height
            0,                          // Levels
            0,                          // Usage
            d3dDstFormat,               // Format
            D3DPOOL_MANAGED,            // Pool
            &(this->texture2D),         // ppTexture
            NULL);                      // pHandle
        n_assert(SUCCEEDED(hr));

        // get base texture interface pointer
        hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_assert(SUCCEEDED(hr));
    }
    else
    {
        // create cube texture
        hr = d3d9Dev->CreateCubeTexture(
            imgWidth,                   // EdgeLength
            1,                          // Levels
            0,                          // Usage
            d3dDstFormat,               // Format
            D3DPOOL_MANAGED,            // Pool
            &(this->textureCube),       // ppCubeTexture
            NULL);                      // pHandle
        n_assert(SUCCEEDED(hr));

        // get base texture interface pointer
        hr = this->textureCube->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_assert(SUCCEEDED(hr));
    }

    // allocate a memory block for the image data
    dataSize = ntxFile.GetSize();
    imageData = n_malloc(dataSize);

    // read each block and write to d3d9 texture
    numLevels = this->baseTexture->GetLevelCount();
    this->SetNumMipLevels(numLevels);
    for (curLevel = 0; curLevel < numLevels; curLevel++)
    {
        // find the next block index which first into maxTextureSize
        int curBlock = ntxFile.FindBlock(ntxType, ntxFormat, curBlockIndex++);
        if (curBlock != -1)
        {            
            ntxFile.SetCurrentBlock(curBlock);
            int mipWidth  = ntxFile.GetWidth();
            int mipHeight = ntxFile.GetHeight();
            
            // skip blocks which don't fit into maxTextureSize
            while ((mipWidth > imgWidth) || (mipHeight > imgHeight))
            {
                curBlock = ntxFile.FindBlock(nNtxFile::TYPE_TEXTURE2D, ntxFormat, curBlockIndex++);
                ntxFile.SetCurrentBlock(curBlock);
                mipWidth  = ntxFile.GetWidth();
                mipHeight = ntxFile.GetHeight();
            }

            if (curBlock != -1)
            {
                // read block data
                ntxFile.ReadBlock(imageData, dataSize);

                // fill a RECT structure with width and height
                RECT srcRect;
                srcRect.left   = 0;
                srcRect.top    = 0;
                srcRect.right  = mipWidth;
                srcRect.bottom = mipHeight;

                if (this->GetType() == TEXTURE_CUBE)
                {
                    // read 6 cube map faces
                    int curFace;
                    int faceDataSize = mipWidth * mipHeight * ntxFile.GetBytesPerPixel();
                    for (curFace = 0; curFace < 6; curFace++)
                    {
                        IDirect3DSurface9* cubeSurface;
                        hr = this->textureCube->GetCubeMapSurface((D3DCUBEMAP_FACES)curFace, curLevel, &cubeSurface);
                        n_assert(SUCCEEDED(hr));

                        // compute pointer to face data
                        uchar* faceData = ((uchar*)imageData) + curFace * faceDataSize;

                        // transfer image data to surface
                        hr = D3DXLoadSurfaceFromMemory(
                            cubeSurface,            // pDestSurface
                            NULL,                   // pDestPalette (none)
                            NULL,                   // pDestRect (entire surface)
                            faceData,               // pSrcMemory
                            d3dSrcFormat,               // SrcFormat
                            ntxFile.GetBytesPerRow(),   // SrcPitch
                            NULL,                       // pSrcPalette
                            &srcRect,                   // pSrcRect
                            D3DX_FILTER_NONE,           // Filter (no filtering since no scaling takes place)
                            0);                         // ColorKey (none)
                        n_assert(SUCCEEDED(hr));

                        cubeSurface->Release();
                        cubeSurface = 0;
                    }
                }
                else
                {
                    // get current d3d mipmap surface
                    IDirect3DSurface9* mipSurface;
                    hr = this->texture2D->GetSurfaceLevel(curLevel, &mipSurface);
                    n_assert(SUCCEEDED(hr));

                    // transfer image data to d3d9 surface
                    hr = D3DXLoadSurfaceFromMemory(
                        mipSurface,                 // pDestSurface
                        NULL,                       // pDestPalette (none)
                        NULL,                       // pDestRect (entire surface)
                        imageData,                  // pSrcMemory
                        d3dSrcFormat,               // SrcFormat
                        ntxFile.GetBytesPerRow(),   // SrcPitch
                        NULL,                       // pSrcPalette
                        &srcRect,                   // pSrcRect
                        D3DX_FILTER_NONE,           // Filter (no filtering since no scaling takes place)
                        0);                         // ColorKey (none)
                    n_assert(SUCCEEDED(hr));

                    // release the mipSurface (as required by GetSurfaceLevel())
                    mipSurface->Release();
                    mipSurface = 0;
                }
            }
        }
    }

    // success
    n_free(imageData);
    ntxFile.CloseRead();
    ntxFile.FreeBlocks();
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
    n_assert(this->texture2D);
    HRESULT hr;

    // set texture attributes
    D3DSURFACE_DESC desc;
    hr = this->texture2D->GetLevelDesc(0, &desc);
    n_assert(SUCCEEDED(hr));

    switch (desc.Type)
    {
        case D3DRTYPE_TEXTURE: 
        case D3DRTYPE_SURFACE:          this->SetType(TEXTURE_2D); break;
        case D3DRTYPE_VOLUMETEXTURE:    this->SetType(TEXTURE_3D); break;
        case D3DRTYPE_CUBETEXTURE:      this->SetType(TEXTURE_CUBE); break;
    }
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
    }
    this->SetWidth(desc.Width);
    this->SetHeight(desc.Height);
    this->SetDepth(1);
    this->SetNumMipLevels(this->texture2D->GetLevelCount());
}

//------------------------------------------------------------------------------
/**
    Create texture from file via D3DX.
*/
bool
nD3D9Texture::LoadD3DXFile()
{
    n_assert(!this->filename.IsEmpty());
    n_assert(0 == this->baseTexture);
    n_assert(0 == this->texture2D);
    n_assert(0 == this->textureCube);

    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9Dev);
 
    // mangle pathname
    char mangledPath[N_MAXPATH];
    this->refFileServer->ManglePath(this->filename.Get(), mangledPath, sizeof(mangledPath));

    // let d3dx load the file
    hr = D3DXCreateTextureFromFileEx(
            d3d9Dev,                    // pDevice
            mangledPath,                // pSrcFile
            D3DX_DEFAULT,               // Width
            D3DX_DEFAULT,               // Height
            D3DX_DEFAULT,               // MipLevels
            0,                          // Usage
            D3DFMT_UNKNOWN,             // Format
            D3DPOOL_MANAGED,            // Pool
            D3DX_FILTER_NONE,           // Filter
            D3DX_DEFAULT,               // MipFilter
            0,                          // ColorKey
            0,                          // pSrcInfo
            0,                          // pPalette
            &(this->texture2D));        // 
    if (FAILED(hr))
    {
        n_printf("nD3D9Texture: D3DXCreateTextureFromFileEx() failed loading file '%s'\n", mangledPath);
        return false;
    }
    n_assert(this->texture2D);

    // get base texture interface pointer
    hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
    n_assert(SUCCEEDED(hr));

    // query texture attributes 
    this->QueryD3DTextureAttributes();

    return true;
}

//------------------------------------------------------------------------------
/**
    Load the file through DevIL. This method should only be used in
    tools and viewers, not in an actual game application, because a lot
    of conversion happens in here, and all loaded textures are converted
    to 32 bit BGRA, which is not very memory efficient.
*/
bool
nD3D9Texture::LoadILFile()
{
    n_assert(!this->filename.IsEmpty());
    n_assert(0 == this->baseTexture);
    n_assert(0 == this->texture2D);
    n_assert(0 == this->textureCube);

    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9Dev);
 
    // mangle pathname
    char mangledPath[N_MAXPATH];
    this->refFileServer->ManglePath(this->filename.Get(), mangledPath, sizeof(mangledPath));

    // create IL image and load
    ilEnable(IL_CONV_PAL);
    ILuint image = iluGenImage();
    ilBindImage(image);
    if (!ilLoadImage(mangledPath))
    {
        n_printf("nD3D9Texture: ilLoadImage() failed loading file '%s'\n", mangledPath);
        iluDeleteImage(image);
        return false;
    }

    // always convert the image to BGRA format
    ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);

    // get relevant image data and create an empty d3d9 texture
    int imageWidth = ilGetInteger(IL_IMAGE_WIDTH);
    int imageHeight = ilGetInteger(IL_IMAGE_HEIGHT);
    hr = D3DXCreateTexture(d3d9Dev,             // pDevice,
                           imageWidth,          // Width
                           imageHeight,         // Height
                           D3DX_DEFAULT,        // MipLevels (complete)
                           0,                   // Usage
                           D3DFMT_A8R8G8B8,     // Format
                           D3DPOOL_MANAGED,     // Pool
                           &(this->texture2D));
    n_assert(SUCCEEDED(hr));

    // get base texture interface pointer
    hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
    n_assert(SUCCEEDED(hr));

    // copy the image data into the toplevel surface
    IDirect3DSurface9* surf;
    hr = this->texture2D->GetSurfaceLevel(0, &surf);
    n_assert(SUCCEEDED(hr));
    RECT srcRect = { 0, 0, imageWidth, imageHeight };
    hr = D3DXLoadSurfaceFromMemory(surf,                // pDestSurface
                                   NULL,                // pDestPalette
                                   NULL,                // pDestRect (entire surface)
                                   ilGetData(),         // pSrcMemory
                                   D3DFMT_A8R8G8B8,     // SrcFormat
                                   imageWidth * 4,      // SrcPitch
                                   NULL,                // pSrcPalette
                                   &srcRect,            // pSrcRect
                                   D3DX_FILTER_NONE,    // Filter
                                   0),                  // ColorKey (disabled)
    surf->Release();
    n_assert(SUCCEEDED(hr));

    // generate mipmaps
    hr = D3DXFilterTexture(this->texture2D,     // pTexture
                           NULL,                // pPalette
                           D3DX_DEFAULT,        // SrcLevel (0)
                           D3DX_DEFAULT);       // MipFilter (D3DX_FILTER_BOX)
    n_assert(SUCCEEDED(hr));

    // query texture attributes 
    this->QueryD3DTextureAttributes();

    // cleanup
    iluDeleteImage(image);
    return true;
}
