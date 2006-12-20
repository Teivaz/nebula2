//------------------------------------------------------------------------------
//  ngltexture_main.cc
//  2003 - Haron
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "opengl/ngltexture.h"
#include "opengl/nglextensionserver.h"

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

#if USE_DEVIL
#include "il/il.h"
#include "il/ilu.h"
#endif

nNebulaClass(nGLTexture, "ntexture2");

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


//------------------------------------------------------------------------------
/**
*/
nGLTexture::nGLTexture() :
    texID(-1),
    target(0),
    hPBuffer(0),
    hPBufferDC(0),
    hPBufferRC(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGLTexture::~nGLTexture()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    nGLTexture support asynchronous resource loading.
*/
bool
nGLTexture::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    apply texture to mesh and bind texture's buffers
*/
int
nGLTexture::ApplyCoords(int stage, GLint size, GLsizei stride, GLvoid* pointer)
{
    //n_printf("Tex[%d][%s]\n", stage, tex->GetFilename());
    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_multitexture))
        glClientActiveTextureARB(GL_TEXTURE0 + stage);//glActiveTextureARB - ??

    //glBindTexture(tex->target,tex->texID);
    if (N_GL_EXTENSION_SUPPORTED(WGL_ARB_render_texture))
        if (!wglBindTexImageARB(this->hPBuffer, WGL_FRONT_LEFT_ARB))
        {
            n_printf("nGLTexture::ApplyCoords() warning: Could not bind p-buffer to render texture!\n");
            n_gltrace("nGLTexture::ApplyCoords().");
            return -1;
        }

    glTexCoordPointer(size, GL_FLOAT, stride, pointer);

    //multitexturing not supported
    if (!N_GL_EXTENSION_SUPPORTED(GL_ARB_multitexture)) return 0;

    glClientActiveTextureARB(GL_TEXTURE0);

    n_gltrace("nGLTexture::ApplyCoords().");
    return 1;
}

//------------------------------------------------------------------------------
/**
    release texture buffers
*/
int
nGLTexture::UnApplyCoords(int stage)
{
    if (N_GL_EXTENSION_SUPPORTED(WGL_ARB_render_texture))
        if (!wglReleaseTexImageARB(this->hPBuffer, WGL_FRONT_LEFT_ARB))
        {
            n_printf("nGLTexture::UnApplyCoords() warning: Could not release p-buffer from render texture!");
            return -1;
        }

    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_multitexture))
    {
        glClientActiveTextureARB(GL_TEXTURE0 + stage);//glActiveTextureARB - ??
    }

    //multitexturing not supported
    if (!N_GL_EXTENSION_SUPPORTED(GL_ARB_multitexture)) return 0;
    n_gltrace("nGLTexture::UnApplyCoords().");
    return 1;
}

//------------------------------------------------------------------------------
/**
*/
void
nGLTexture::UnloadResource()
{
    n_assert(this->IsLoaded());

    nGLServer2* gfxServer = (nGLServer2*)nGfxServer2::Instance();
    n_assert(gfxServer->hDC);

    //n_printf("nGLTexture::UnloadResource(): %s\n", this->GetName());

    // check if I am the current render target in the gfx server...
    if (this->IsRenderTarget())
    {
        int i;
        for (i = 0; i < nGfxServer2::MaxRenderTargets; i++)
        {
            if (gfxServer->GetRenderTarget(i) == this)
            {
                gfxServer->SetRenderTarget(i, 0);
            }
        }
    }

    // check if I am one of the currently active textures
    //int curStage;
    //for (curStage = 0; curStage < nGfxServer2::MaxTextureStages; curStage++)
    //{
    //    if (gfxServer->GetTexture(curStage) == this)
    //    {
    //        gfxServer->SetTexture(curStage, 0);
    //    }
    //}

    glDeleteTextures(1, &this->texID);
    this->texID = 0;
    this->target = 0;

    if (hPBufferRC != NULL)
    {
        wglMakeCurrent(hPBufferDC, hPBufferRC);
        wglDeleteContext(hPBufferRC);
        wglReleasePbufferDCARB(hPBuffer, hPBufferDC);
        wglDestroyPbufferARB(hPBuffer);
        hPBufferRC = NULL;
    }
/*
    if (hPBufferDC != NULL )
    {
        ReleaseDC(gfxServer->hWnd,hPBufferDC);
        hPBufferDC = NULL;
    }
*/
    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
*/
bool
nGLTexture::LoadResource()
{
    n_assert(!this->IsLoaded());

    //n_printf("nGLTexture::LoadResource(): %s\n", this->GetName());

    bool success = false;
    nString filename = this->GetFilename().Get();

    if (this->IsRenderTarget())
    {
        // create a render target
        success = this->CreateRenderTarget();
    }
    else if (this->GetUsage() & CreateFromRawCompoundFile)
    {
        //success = this->LoadFromRawCompoundFile();
        success = false;
    }
    else if (this->GetUsage() & CreateFromDDSCompoundFile || filename.CheckExtension("dds"))
    {
        success = this->LoadFromDDSCompoundFile();
    }
    else if (this->GetUsage() & CreateEmpty)
    {
        // create an empty texture
        success = this->CreateEmptyTexture();
        if (success)
        {
            this->SetState(Empty);
        }
        //return true;
    }
    //else if (filename.CheckExtension("dds"))
    //{
    //    // load file through D3DX, assume file has mip maps 
    //    success = this->LoadD3DXFile(false);
    //}
#if USE_DEVIL
    else
    {
        // load file through DevIL
        success = this->LoadILFile();
    }
#else
    else
    {
        n_error("nGLTexture::LoadResource(): Unknown texture type!");
    }
#endif

    if (success)
    {
        this->SetState(Valid);
    }

    //// register texture with GLServer
    //nGLServer2* gfxServer = (nGLServer2*)nGfxServer2::Instance();
    //n_assert(gfxServer->hDC);
    //int curStage;

    //success = false;
    //for (curStage = 0; curStage < nGfxServer2::MaxTextureStages; curStage++)
    //{
    //    if (gfxServer->GetTexture(curStage) == 0)
    //    {
    //        gfxServer->SetTexture(curStage, this);
    //        success = true;
    //        n_printf("TexStage = %d\n", curStage);
    //        break;
    //    }
    //}
    
    n_gltrace("nGLTexture::LoadResource().");
    return success;
}

//------------------------------------------------------------------------------
/**
    This method is called when the gl device is lost.
*/
void
nGLTexture::OnLost()
{
    if (this->IsRenderTarget() || (this->usage & Dynamic))
    {
        this->UnloadResource();
        this->SetState(Lost);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the gl device has been restored.
*/
void
nGLTexture::OnRestored()
{
    if (this->IsRenderTarget() || (this->usage & Dynamic))
    {
        this->SetState(Unloaded);
        this->LoadResource();
        if (this->usage & CreateEmpty)
        {
            this->SetState(Empty);
        }
        else
        {
            this->SetState(Valid);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Create gl render target texture, and optionally a depthStencil surface
*/
bool
nGLTexture::CreateRenderTarget()
{
    n_assert(this->width > 0);
    n_assert(this->height > 0);
    n_assert(-1 == this->texID);
    n_assert(this->IsRenderTarget());

    if (!N_GL_EXTENSION_SUPPORTED(WGL_ARB_render_texture))
        return false;

    // Set some p-buffer attributes so that we can use this p-buffer as a
    // render target.
    int pba[] =
    {
        WGL_TEXTURE_FORMAT_ARB, WGL_NO_TEXTURE_ARB, // render target - not texture
        WGL_TEXTURE_TARGET_ARB, WGL_NO_TEXTURE_ARB,
        0
    };

    // Define the pixel format requirements we will need for our
    // p-buffer. A p-buffer is just like a frame buffer, it can have a depth
    // buffer associated with it and it can be double buffered.
    int pfa[] =
    {
        WGL_RED_BITS_ARB, 8,                // At least 8 bits for RED channel
        WGL_GREEN_BITS_ARB, 8,              // At least 8 bits for GREEN channel
        WGL_BLUE_BITS_ARB, 8,               // At least 8 bits for BLUE channel
        WGL_ALPHA_BITS_ARB, 8,              // At least 8 bits for ALPHA channel
        WGL_DEPTH_BITS_ARB, 0,                // depth buffer
        //WGL_STENCIL_BITS_ARB, 0,            // stencil buffer
        WGL_SUPPORT_OPENGL_ARB, TRUE,       // P-buffer will be used with OpenGL
        WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // Enable render to p-buffer
        //WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE, // P-buffer will be used as a texture
        //WGL_DOUBLE_BUFFER_ARB, FALSE,       // We don't require double buffering
        0                                   // Zero terminates the list
    };

    nGLServer2* gfxServer = (nGLServer2*)nGfxServer2::Instance();
    HDC hDC = gfxServer->hDC;
    HGLRC hRC = gfxServer->context;
    n_assert(hDC);
    n_assert(hRC);
    //TODO: check hDC ???

    // create render target color surface
    if (this->usage & RenderTargetColor)
    {
        switch (this->format)
        {
            case X8R8G8B8:
            case A8R8G8B8:
                //colorFormat = DDS_A8R8G8B8;
                pfa[1]    = 8;    // At least 8 bits for RED channel
                pfa[3]    = 8;    // At least 8 bits for GREEN channel
                pfa[5]    = 8;    // At least 8 bits for BLUE channel
                pfa[7]    = 8;    // At least 8 bits for ALPHA channel
                break;
            //Float formats with GL_NV_float_buffer support

            default:
                n_printf("nGLTexture: invalid render target pixel format!\n");
                return false;
        }
    }

    // create optional render target depth surface
    if (this->usage & RenderTargetDepth)
    {
        pfa[9] = 32;    // At least 32 bits for depth buffer
    }
/*
    // create optional render target stencil surface
    if (this->renderTargetFlags & RenderTargetStencil)
    {
        pfa[11] = 32;    // At least 32 bits for stencil buffer
    }
*/

    CreatePBuffer(pfa, pba);

    // We were successful in creating a p-buffer. We can now make its context
    // current and set it up just like we would a regular context
    // attached to a window.
    if (!wglMakeCurrent(hPBufferDC,hPBufferRC))
    {
        n_printf("nGLTexture: Could not make the p-buffer's context current for <%s>",this->GetName());
        return false;
    }

    this->target = GL_TEXTURE_2D;
    n_gltrace("nGLTexture::CreateRenderTarget().");
    return true;
}

//------------------------------------------------------------------------------
/**
    Query the texture attributes from the gl texture object and
    update my own attributes.
*/
/*
void
nGLTexture::QueryGLTextureAttributes()
{
    n_assert(this->GetType() != TEXTURE_NOTYPE);

    if (this->GetType() == TEXTURE_2D)
    {
        n_assert(this->texture2D);
        HRESULT hr;

        // set texture attributes
        D3DSURFACE_DESC desc;
        hr = this->texture2D->GetLevelDesc(0, &desc);
        n_dxtrace(hr, "QueryD3DTextureAttributes() failed");

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
        n_dxtrace(hr, "GetLevelDesc() failed");

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
*/

//------------------------------------------------------------------------------
/**
    Create an empty 2D or cube texture (without mipmaps!).
*/
bool
nGLTexture::CreateEmptyTexture()
{
    n_assert(this->GetWidth() > 0);
    n_assert(this->GetHeight() > 0);
    n_assert(this->GetType() != TEXTURE_NOTYPE);
    n_assert(this->GetFormat() != NOFORMAT);
    //n_assert(0 == this->texture2D);
    //n_assert(0 == this->textureCube);

    //n_assert(N_GL_EXTENSION_SUPPORTED(WGL_ARB_render_texture));

    int pfa[30]; // pixel format attributes
    int pba[10]; // pixel buffer attributes

    memset(pfa, 0, sizeof(int)*30);
    memset(pba, 0, sizeof(int)*10);

    // Set some p-buffer attributes so that we can use this p-buffer as a
    // RGBA texture target.
    pba[0] = WGL_TEXTURE_FORMAT_ARB;    pba[1] = WGL_TEXTURE_RGBA_ARB;
    pba[2] = WGL_TEXTURE_TARGET_ARB;

    // Define the pixel format requirements we will need for our
    // p-buffer. A p-buffer is just like a frame buffer, it can have a depth
    // buffer associated with it and it can be double buffered.
    pfa[0] = WGL_RED_BITS_ARB;
    pfa[2] = WGL_GREEN_BITS_ARB;
    pfa[4] = WGL_BLUE_BITS_ARB;
    pfa[6] = WGL_ALPHA_BITS_ARB;

    pfa[8] = WGL_SUPPORT_OPENGL_ARB;        pfa[9] = GL_TRUE;    // P-buffer will be used with OpenGL
    pfa[10] = WGL_DRAW_TO_PBUFFER_ARB;        pfa[11] = GL_TRUE;    // Enable render to p-buffer
    pfa[12] = WGL_BIND_TO_TEXTURE_RGBA_ARB;    pfa[13] = GL_TRUE;    // P-buffer will be used as a texture
    //pfa[14] = WGL_DOUBLE_BUFFER_ARB;        pfa[15] = GL_FALSE;    // We don't require double buffering

    GLenum cformat;    // compressed texture format
    GLint iformat;
    GLenum tformat;
    GLenum ttype;
    int blockSize = 0; // for calculating compressed textures size

    nGLServer2* gfxServer = (nGLServer2*)nGfxServer2::Instance();
    HDC hDC = gfxServer->hDC;
    HGLRC hRC = gfxServer->context;
    n_assert(hDC);
    n_assert(hRC);
    //TODO: check hDC ???

    // checking for 'Dinamic'
    bool compressedTex = false;
    switch (this->format)
    {
        case X8R8G8B8:
        case A8R8G8B8:
            pfa[1]    = 8;    // At least 8 bits for RED channel
            pfa[3]    = 8;    // At least 8 bits for GREEN channel
            pfa[5]    = 8;    // At least 8 bits for BLUE channel
            pfa[7]    = 8;    // At least 8 bits for ALPHA channel
            iformat = GL_RGBA8;
            tformat = GL_RGBA;
            ttype = GL_UNSIGNED_INT_8_8_8_8;
            break;
        case R5G6B5:
            pfa[1]    = 5;    // At least 5 bits for RED channel
            pfa[3]    = 6;    // At least 6 bits for GREEN channel
            pfa[5]    = 5;    // At least 5 bits for BLUE channel
            pfa[7]    = 0;    // At least 0 bits for ALPHA channel
            pba[1] = WGL_TEXTURE_RGB_ARB;
            iformat = GL_RGB5_A1;
            tformat = GL_RGB;
            ttype = GL_UNSIGNED_SHORT_5_6_5;
            break;
        case A1R5G5B5:
            pfa[1]    = 5;    // At least 5 bits for RED channel
            pfa[3]    = 5;    // At least 5 bits for GREEN channel
            pfa[5]    = 5;    // At least 5 bits for BLUE channel
            pfa[7]    = 1;    // At least 1 bits for ALPHA channel
            iformat = GL_RGB5_A1;
            tformat = GL_RGBA;
            ttype = GL_UNSIGNED_SHORT_5_5_5_1;
            break;
        case A4R4G4B4:
            pfa[1]    = 4;    // At least 4 bits for RED channel
            pfa[3]    = 4;    // At least 4 bits for GREEN channel
            pfa[5]    = 4;    // At least 4 bits for BLUE channel
            pfa[7]    = 4;    // At least 4 bits for ALPHA channel
            iformat = GL_RGBA4;
            tformat = GL_RGBA;
            ttype = GL_UNSIGNED_SHORT_4_4_4_4;
            break;
        //case P8:                d3dFormat = DDS_P8; break;
        case DXT1:
            if (N_GL_EXTENSION_SUPPORTED(GL_EXT_texture_compression_s3tc))
            {
                compressedTex = true;
                cformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                blockSize = 8;
            }
            else
            {
                n_printf("nGLTexture::CreateEmptyTexture - format DXT1: texture compression not supported!\n");
                return false;
            }
            break;
        //case DXT2:break;
        case DXT3:
            if (N_GL_EXTENSION_SUPPORTED(GL_EXT_texture_compression_s3tc))
            {
                compressedTex = true;
                cformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                blockSize = 16;
            }
            else
            {
                n_printf("nGLTexture::CreateEmptyTexture - format DXT3: texture compression not supported!\n");
                return false;
            }
            break;
        //case DXT4:break;
        case DXT5:
            if (N_GL_EXTENSION_SUPPORTED(GL_EXT_texture_compression_s3tc))
            {
                compressedTex = true;
                cformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                blockSize = 16;
            }
            else
            {
                n_printf("nGLTexture::CreateEmptyTexture - format DXT5: texture compression not supported!\n");
                return false;
            }
            break;
        //case R16F:              d3dFormat = DDS_R16F; break;
        //case G16R16F:           d3dFormat = DDS_G16R16F; break;
        //case A16B16G16R16F:     d3dFormat = DDS_A16B16G16R16F; break;
        //case R32F:              d3dFormat = DDS_R32F; break;
        //case G32R32F:           d3dFormat = DDS_G32R32F; break;
        //case A32B32G32R32F:     d3dFormat = DDS_A32B32G32R32F; break;
        //Float formats with GL_NV_float_buffer support

        default:
            n_printf("nGLTexture: invalid render target pixel format!\n");
            return false;
    }

    if (this->GetType() == TEXTURE_2D)
    {
        this->target = GL_TEXTURE_2D;
        pba[3] = WGL_TEXTURE_2D_ARB;

        if (!CreatePBuffer(pfa,pba)) return false;

        // We were successful in creating a p-buffer. We can now make its
        // context current and set it up just like we would a regular context
        // attached to a window.
        if (!wglMakeCurrent(hPBufferDC,hPBufferRC))
        {
            n_printf("nGLTexture: Could not make the p-buffer's context current for <%s>",this->GetName());
            return false;
        }

        //generating texture
        glGenTextures(1, &this->texID);
        n_assert(-1 != this->texID);
        glBindTexture(GL_TEXTURE_2D, this->texID);

        if (compressedTex)
        {
            int imageSize = ((this->width+3)/4) * ((this->height+3)/4) * blockSize;
            glCompressedTexImage2DARB(
                GL_TEXTURE_2D,        //target
                0,                    //level
                cformat,            //internal format
                this->width,
                this->height,
                0,                    //border
                imageSize,            //compressed image size
                NULL);
        }
        else
        {
            // transfer image data to surface
            glTexImage2D(
                GL_TEXTURE_2D,        //target
                0,                    //level
                iformat,            //internalformat ?(GL_COMPRESSED_RGBA)
                this->width,        //width
                this->height,        //height
                0,                    //border
                tformat,            //format
                ttype,                //type
                NULL);                //pixels
        }
    }
    else  if (this->GetType() == TEXTURE_CUBE)
    {
        this->target = GL_TEXTURE_CUBE_MAP;
        pba[3] = WGL_TEXTURE_CUBE_MAP_ARB;

        if (!CreatePBuffer(pfa,pba)) return false;

        // We were successful in creating a p-buffer. We can now make its
        // context current and set it up just like we would a regular context
        // attached to a window.
        if (!wglMakeCurrent(hPBufferDC, hPBufferRC))
        {
            n_printf("nGLTexture: Could not make the p-buffer's context current for <%s>", this->GetName());
            return false;
        }

        //generating texture
        glGenTextures(1, &this->texID);
        n_assert(-1 != this->texID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->texID);

        int i;
        int cube_attrib[] = {WGL_CUBE_MAP_FACE_ARB, WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 0};
        if (compressedTex)
        {
            int imageSize = ((this->width+3)/4) * ((this->height+3)/4) * blockSize;
            for (i = 0; i < 6; i++)
            {
                wglSetPbufferAttribARB(hPBuffer,(const int*)cube_attrib);
                glCompressedTexImage2DARB(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,    //target
                    0,                                    //level
                    cformat,                            //internal format
                    this->width,
                    this->height,
                    0,                                    //border
                    imageSize,                            //compressed image size
                    NULL);
                cube_attrib[1]++;
            }
        }
        else
        {
            // transfer image data to surface
            for (i = 0; i < 6; i++)
            {
                wglSetPbufferAttribARB(hPBuffer,(const int*)cube_attrib);
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,    //target
                    0,                                    //level
                    iformat,                            //internalformat ?(GL_COMPRESSED_RGBA)
                    this->width,                        //width
                    this->height,                        //height
                    0,                                    //border
                    tformat,                            //format
                    ttype,                                //type
                    NULL);                                //pixels
                cube_attrib[1]++;
            }
        }
    }
    else
    {
        // unsupported texture type
        n_error("nGLTexture::CreateEmptyTexture(): Unsupported texture type!\n");
        return false;
    }

    // query texture attributes
    //this->QueryGLTextureAttributes();
    n_gltrace("nGLTexture::CreateEmptyTexture().");
    return true;
}

//------------------------------------------------------------------------------
/**
    Create texture and load contents from as "raw" pixel chunk from
    inside a compound file.
*/
/*
bool
nGLTexture::LoadFromRawCompoundFile()
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
*/

// DDS header structures
/**
    DDSPixelFormat
*/
struct DDSPixelFormat
{
    ulong Size;            // size of structure
    ulong Flags;           // pixel format flags
    ulong FourCC;          // (FOURCC code)
    ulong RGBBitCount;     // how many bits per pixel
    ulong RBitMask;        // mask for red bit
    ulong GBitMask;        // mask for green bits
    ulong BBitMask;        // mask for blue bits
    ulong ABitMask;        // mask for alpha channel
};

/**
    DDSHeader
*/
struct DDSHeader
{
    ulong Size;                 // size of the DDSHeader structure
    ulong Flags;                // determines what fields are valid
    ulong Height;               // height of surface to be created
    ulong Width;                // width of input surface
    ulong PitchOrLinearSize;    // distance to start of next line (return value only) or Formless late-allocated optimized surface size
    ulong Depth;                // the depth if this is a volume texture
    ulong MipMapCount;          // number of mip-map levels requested
    ulong Reserved1[11];        // reserved
    DDSPixelFormat PixelFormat; // pixel format description of the surface
    ulong Caps1;
    ulong Caps2;
    ulong Reserved2[2];
    ulong TextureStage;         // stage in multitexture cascade
};

#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
    ((ulong)(uchar)(ch0) |\
    ((ulong)(uchar)(ch1) << 8 ) |\
    ((ulong)(uchar)(ch2) << 16) |\
    ((ulong)(uchar)(ch3) << 24))

/**
    texture pixel formats
*/
enum
{
    DDS_UNKNOWN              =  0,

    DDS_R8G8B8               = 20,
    DDS_A8R8G8B8             = 21,
    DDS_X8R8G8B8             = 22,
    DDS_R5G6B5               = 23,
    DDS_X1R5G5B5             = 24,
    DDS_A1R5G5B5             = 25,
    DDS_A4R4G4B4             = 26,
    DDS_R3G3B2               = 27,
    DDS_A8                   = 28,
    DDS_A8R3G3B2             = 29,
    DDS_X4R4G4B4             = 30,
    DDS_A2B10G10R10          = 31,
    DDS_A8B8G8R8             = 32,
    DDS_X8B8G8R8             = 33,
    DDS_G16R16               = 34,
    DDS_A2R10G10B10          = 35,
    DDS_A16B16G16R16         = 36,

    DDS_A8P8                 = 40,
    DDS_P8                   = 41,

    DDS_L8                   = 50,
    DDS_A8L8                 = 51,
    DDS_A4L4                 = 52,

    DDS_V8U8                 = 60,
    DDS_L6V5U5               = 61,
    DDS_X8L8V8U8             = 62,
    DDS_Q8W8V8U8             = 63,
    DDS_V16U16               = 64,
    DDS_A2W10V10U10          = 67,

    DDS_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    DDS_R8G8_B8G8            = MAKEFOURCC('R', 'G', 'B', 'G'),
    DDS_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
    DDS_G8R8_G8B8            = MAKEFOURCC('G', 'R', 'G', 'B'),
    DDS_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
    DDS_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
    DDS_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
    DDS_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
    DDS_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

    DDS_D16_LOCKABLE         = 70,
    DDS_D32                  = 71,
    DDS_D15S1                = 73,
    DDS_D24S8                = 75,
    DDS_D24X8                = 77,
    DDS_D24X4S4              = 79,
    DDS_D16                  = 80,

    DDS_D32F_LOCKABLE        = 82,
    DDS_D24FS8               = 83,


    DDS_L16                  = 81,

    DDS_VERTEXDATA           =100,
    DDS_INDEX16              =101,
    DDS_INDEX32              =102,

    DDS_Q16W16V16U16         =110,

    DDS_MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    DDS_R16F                 = 111,
    DDS_G16R16F              = 112,
    DDS_A16B16G16R16F        = 113,

    // IEEE s23e8 formats (32-bits per channel)
    DDS_R32F                 = 114,
    DDS_G32R32F              = 115,
    DDS_A32B32G32R32F        = 116,

    DDS_CxV8U8               = 117,


    DDS_FORCE_DWORD          =0x7fffffff
};

// Flags members of the DDSHeader
enum
{
    // bit flags for header
	DDS_CAPS	    = 0x00000001,
	DDS_HEIGHT	    = 0x00000002,
	DDS_WIDTH	    = 0x00000004,
	DDS_PITCH	    = 0x00000008,
	DDS_PIXELFORMAT = 0x00001000,
	DDS_MIPMAPCOUNT = 0x00020000,
	DDS_LINEARSIZE  = 0x00080000,
	DDS_DEPTH	    = 0x00800000,

    // flags for pixel formats
	DDS_ALPHA_PIXELS = 0x00000001,
	DDS_ALPHA        = 0x00000002,
	DDS_FOURCC	     = 0x00000004,
	DDS_RGB	         = 0x00000040,
    DDS_RGBA         = 0x00000041,

    // flags for complex caps
	DDS_COMPLEX	   = 0x00000008,
	DDS_TEXTURE	   = 0x00001000,
	DDS_MIPMAP	   = 0x00400000,

    // flags for cubemaps
	DDS_CUBEMAP	          = 0x00000200,
	DDS_CUBEMAP_POSITIVEX = 0x00000400,
	DDS_CUBEMAP_NEGATIVEX = 0x00000800,
	DDS_CUBEMAP_POSITIVEY = 0x00001000,
	DDS_CUBEMAP_NEGATIVEY = 0x00002000,
	DDS_CUBEMAP_POSITIVEZ = 0x00004000,
	DDS_CUBEMAP_NEGATIVEZ = 0x00008000,
	DDS_VOLUME		      = 0x00200000
};

const ulong dds_cubemap_face[] = {
    DDS_CUBEMAP_POSITIVEX,
    DDS_CUBEMAP_NEGATIVEX,
    DDS_CUBEMAP_POSITIVEY,
    DDS_CUBEMAP_NEGATIVEY,
    DDS_CUBEMAP_POSITIVEZ,
    DDS_CUBEMAP_NEGATIVEZ
};

#ifdef __MAC__
//------------------------------------------------------------------------------
/**
    Swap the bytes in a 32 bit value
*/
inline
void swap_endian(void *val)
{
    unsigned int *ival = (unsigned int *)val;

    *ival = ((*ival >> 24) & 0x000000ff) |
            ((*ival >>  8) & 0x0000ff00) |
            ((*ival <<  8) & 0x00ff0000) |
            ((*ival << 24) & 0xff000000);
}
#endif

//------------------------------------------------------------------------------
/**
    clamps input size to [1-size]
*/
inline
int clamp_size(int size)
{
    if (size <= 0)
        size = 1;

    return size;
}

//------------------------------------------------------------------------------
/**
*/
inline
int bitsCount(uint n)
{
    int i, res;
    res = 0;

    for(i = 0; i < 32; i++)
    {
        if (n & (1 << i))
            res++;
    }

    return res;
}

//------------------------------------------------------------------------------
/**
*/
inline
int firstSetBitPos(uint n)
{
    int i, res;
    res = 31; // there is no setted bits

    for(i = 0; i < 32; i++)
    {
        if (n & (1 << i))
        {
            res = i;
            break;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
/**
    Load texture as DDS file from inside of a compound file.
*/
bool
nGLTexture::LoadFromDDSCompoundFile()
{
    n_assert(this->compoundFile || !this->GetFilename().IsEmpty());
    n_assert(0 != this->texID);

    nFile *ddsfile;

    if (this->compoundFile) ddsfile = this->compoundFile;
    else
    {
        ddsfile = nFileServer2::Instance()->NewFileObject();
            // open the file
        if (!ddsfile->Open(this->GetFilename().Get(), "r"))
        {
            n_error("nGLTexture::LoadFromDDSCompoundFile(): Could not open file %s\n", this->GetFilename().Get());
            ddsfile->Release();
            return false;
        }
    }

    //n_assert(N_GL_EXTENSION_SUPPORTED(WGL_ARB_render_texture));

    DDSHeader ddsh;
    uchar *_buffer;
    char filecode[5];
    uint numBytesRead;

    GLTexParams gltexp;

    nGLServer2* gfxServer = (nGLServer2*)nGfxServer2::Instance();
    HDC hDC = gfxServer->hDC;
    HGLRC hRC = gfxServer->context;
    n_assert(hDC);
    n_assert(hRC);

    // Verify the file is a true .dds file
    numBytesRead = ddsfile->Read(filecode, 4);
    n_assert(numBytesRead == 4);
    filecode[4] = 0;

    if (n_stricmp(filecode, "DDS ") != 0)
    {
        n_printf("DDS file loader: The file doesn't appear to be a valid .dds file! Filecode: <%s>\n", filecode);
        return false;
    }

    // Get the header
    numBytesRead = ddsfile->Read(&ddsh, sizeof(ddsh));
    n_printf("Header size: %u. Width: %u. Height: %u.\n",numBytesRead,ddsh.Width,ddsh.Height);
    n_assert(numBytesRead == sizeof(ddsh));

#ifdef __MAC__
    swap_endian(&ddsh.Size);
    swap_endian(&ddsh.Flags);
    swap_endian(&ddsh.Height);
    swap_endian(&ddsh.Width);
    swap_endian(&ddsh.PitchOrLinearSize);
    swap_endian(&ddsh.MipMapCount);
    swap_endian(&ddsh.PixelFormat.Size);
    swap_endian(&ddsh.PixelFormat.Flags);
    swap_endian(&ddsh.PixelFormat.FourCC);
    swap_endian(&ddsh.PixelFormat.RGBBitCount);
    swap_endian(&ddsh.Caps1);
    swap_endian(&ddsh.Caps2);
#endif

    // number of mipmaps in file includes main surface so decrease count by one
    this->SetNumMipLevels(1);
    if (ddsh.Caps1 & DDS_MIPMAP)
    {
        this->SetNumMipLevels(ddsh.MipMapCount);
    }
    //else
    //{
    //    n_printf("DDS file loader: There is no mipmaps.");
    //    if (!this->compoundFile) ddsfile->Close();
    //    return false;
    //}

    //
    // How big will the buffer need to be to load all of the pixel data
    // including mip-maps?
    //
    //if (ddsh.PitchOrLinearSize == 0)
    //{
    //    n_printf("DDS file loader: LinearSize is 0!");
    //    if (!this->compoundFile)
    //    {
    //        ddsfile->Close();
    //    }
    //    return false;
    //}

    // figure out what the image format is
    if (!ParseDDSHeader(&ddsh, &gltexp))
    {
        if (!this->compoundFile)
        {
            ddsfile->Close();
        }
        return false;
    }

    if (N_GL_EXTENSION_SUPPORTED(WGL_ARB_render_texture))
    {
        if (CreatePBuffer((const int*)gltexp.pfa.Begin(), (const int*)gltexp.pba.Begin()))
        {
            // We were successful in creating a p-buffer. We can now make its
            // context current and set it up just like we would a regular
            // context attached to a window.
            if (!wglMakeCurrent(hPBufferDC, hPBufferRC))
            {
                n_printf("nGLTexture: Could not make the p-buffer's context current for <%s>", this->GetName());
                if (!this->compoundFile)
                {
                    ddsfile->Release();
                }
                return false;
            }
        }
    }

    // generate texture id
    glGenTextures(1, &this->texID);
    n_gltrace("nGLTexture::LoadFromDDSCompoundFile(): Can't generate texture id.");
    //n_assert(-1 != this->texID);

    // bind texture to the target
    glBindTexture(target, this->texID);
    n_gltrace("nGLTexture::LoadFromDDSCompoundFile(): Can't bind texture.");

    int i, n;
    uint _width, _height, _depth, _size;
    int cube_attrib[] = {WGL_CUBE_MAP_FACE_ARB, WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 0};
    GLenum ctarget; // = this->GetType() == TEXTURE_2D ? GL_TEXTURE_2D : this->GetType() == TEXTURE_CUBE ? GL_TEXTURE_CUBE_MAP_POSITIVE_X : GL_TEXTURE_2D;
    int cfaces = 1; //this->GetType() == TEXTURE_CUBE ? 6 : 1;

    switch (this->GetType())
    {
    case TEXTURE_2D:
        ctarget = GL_TEXTURE_2D;
        break;
    case TEXTURE_CUBE:
        ctarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        cfaces = 6;
        break;
    default:
        ctarget = GL_TEXTURE_2D;
    }

    n_printf("DDS read (texID=%u, LinearSize=%u, MipNum=%d, CubeFaceNum=%d, compressed=%s, width=%u, height=%u)...\n",
        this->texID, ddsh.PitchOrLinearSize, this->GetNumMipLevels(), cfaces,
        gltexp.compressed ? "true" : "false", this->GetWidth(), this->GetHeight());

    // load all surfaces for the image (6 surfaces for cubemaps)
    for (n = 0; n < cfaces; n++)
    {
        _width  = this->GetWidth();
        _height = this->GetHeight();
        _depth  = this->GetDepth();

        if (this->GetType() == TEXTURE_CUBE && N_GL_EXTENSION_SUPPORTED(WGL_ARB_render_texture))
        {
            wglSetPbufferAttribARB(hPBuffer, (const int*)cube_attrib);
        }

        // load all mipmaps for current surface
        for (i = 0; i < this->GetNumMipLevels() && (_width || _height); i++)
        {
            // calculate surface size
            if (gltexp.compressed)
            {
                _size = ((_width + 3) / 4) * ((_height + 3) / 4) *
                    (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16);
            }
            else
            {
                _size = _width * _height * gltexp.bytesPerPixel;
            }
            n_printf("DDS (cubeface=%d, mip=%u, _size=%u, ", n, i, _size);

            _buffer = (uchar*)n_malloc(_size * sizeof(uchar));

            numBytesRead = ddsfile->Read((void *)_buffer, _size);
            n_printf("numBytesRead=%u)\n", numBytesRead);
            n_assert(numBytesRead == _size);

            // transfer image data to surface
            if (gltexp.compressed)
            {
                glCompressedTexImage2DARB(ctarget,
                                          i,
                                          gltexp.tformat,
                                          _width, _height,
                                          0,
                                          _size,
                                          (const GLvoid *)_buffer
                                         );
                n_gltrace("nGLTexture::LoadFromDDSCompoundFile(): Can't load compressed texture.");
            }
            else
            {
                glTexImage2D(ctarget,                // GL_TEXTURE2D or GL_TEXTURE_CUBE_MAP_...
                             i,                      // mipmap level
                             gltexp.iformat,         // GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
                             _width, _height,        // width and height of the texture
                             0,                      // border size
                             gltexp.tformat,         // GL_RGB, GL_RGBA, GL_BGR, GL_BGRA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RED, ...
                             gltexp.ttype,           // GL_UNSIGNED_BYTE, ... GL_INT, ... GL_UNSIGNED SHORT_5_6_5, ...
                             (const GLvoid *)_buffer
                            );
                n_gltrace("nGLTexture::LoadFromDDSCompoundFile(): Can't load texture.");
            }

            // shrink to next power of 2
            _width  = clamp_size(_width >> 1);
            _height = clamp_size(_height >> 1);
            _depth  = clamp_size(_depth >> 1);

            n_free(_buffer);
            _buffer = NULL;
        }

        if (this->GetType() == TEXTURE_CUBE)
        {
            ctarget++;
            cube_attrib[1]++;    // next cube map face
        }
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // query texture attributes
    //this->QueryGLTextureAttributes();
    if (!this->compoundFile)
    {
        ddsfile->Close();
    }

    n_printf("Texture loaded.\n");
    n_gltrace("nGLTexture::LoadFromDDSCompoundFile(): Some error occured during DDS texture loading.");
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
nGLTexture::Lock(LockType lockType, int level, LockInfo& lockInfo)
{
    //this->LockMutex();
    //n_assert(this->GetType() == TEXTURE_2D);
    //n_assert(this->texture2D);

    //DWORD d3dLockFlags = 0;
    //switch (lockType)
    //{
    //    case ReadOnly:
    //        d3dLockFlags = D3DLOCK_READONLY;
    //        break;

    //    case WriteOnly:
    //        d3dLockFlags = D3DLOCK_NO_DIRTY_UPDATE;
    //        break;
    //}

    bool retval = false;
    //D3DLOCKED_RECT d3dLockedRect = { 0 };
    //HRESULT hr = this->texture2D->LockRect(level, &d3dLockedRect, NULL, d3dLockFlags);
    //if (SUCCEEDED(hr))
    //{
    //    lockInfo.surfPointer = d3dLockedRect.pBits;
    //    lockInfo.surfPitch   = d3dLockedRect.Pitch;
    //    retval = true;
    //}
    //this->UnlockMutex();
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
nGLTexture::LockCubeFace(LockType lockType, CubeFace face, int level, LockInfo& lockInfo)
{
    //this->LockMutex();
    //n_assert(this->GetType() == TEXTURE_CUBE);
    //n_assert(this->textureCube);

    //DWORD d3dLockFlags = 0;
    //switch (lockType)
    //{
    //    case ReadOnly:
    //        d3dLockFlags = D3DLOCK_READONLY;
    //        break;

    //    case WriteOnly:
    //        d3dLockFlags = 0;
    //        break;
    //}

    bool retval = false;
    //D3DLOCKED_RECT d3dLockedRect = { 0 };
    //HRESULT hr = this->textureCube->LockRect((D3DCUBEMAP_FACES) face, level, &d3dLockedRect, NULL, d3dLockFlags);
    //if (SUCCEEDED(hr))
    //{
    //    lockInfo.surfPointer = d3dLockedRect.pBits;
    //    lockInfo.surfPitch   = d3dLockedRect.Pitch;
    //    retval = true;
    //}
    //this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock the 2D texture.
*/
void
nGLTexture::Unlock(int level)
{
    //this->LockMutex();
    //n_assert(this->GetType() == TEXTURE_2D);
    //n_assert(this->texture2D);
    //HRESULT hr = this->texture2D->UnlockRect(level);
    //n_dxtrace(hr, "UnlockRect() on 2d texture failed");
    //this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Unlock a cube texture face.
*/
void
nGLTexture::UnlockCubeFace(CubeFace face, int level)
{
    //this->LockMutex();
    //n_assert(this->GetType() == TEXTURE_CUBE);
    //n_assert(this->textureCube);
    //HRESULT hr = this->textureCube->UnlockRect((D3DCUBEMAP_FACES) face, level);
    //n_dxtrace(hr, "UnlockRect() on cube surface failed");
    //this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Compute the byte size of the texture data.
*/
int
nGLTexture::GetByteSize()
{
    if (this->IsLoaded())
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
        case A8:
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

//------------------------------------------------------------------------------
/**
    Generante mip maps for surface data.

    - Feb-04 Kim, H.W. added to support ngameswf.
*/
void nGLTexture::GenerateMipMaps()
{
    //n_assert(this->texture2D);

    //HRESULT hr;

    //hr = D3DXFilterTexture(this->texture2D,    // pTexture
    //                       NULL,               // pPalette
    //                       D3DX_DEFAULT,       // SrcLevel(0)
    //                       D3DX_FILTER_LINEAR);// MipFilter

    //n_assert (SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
    Convert DDS texture params to GL texture ones and fill Nebula params.
*/
bool
nGLTexture::ParseDDSHeader(DDSHeader* header, GLTexParams* params)
{
    bool usingAlpha;

    // Set some p-buffer attributes so that we can use this p-buffer as a
    // RGBA texture target.
    //params->pba.PushBack(WGL_TEXTURE_FORMAT_ARB);
    //params->pba.PushBack(WGL_TEXTURE_RGBA_ARB);

    //pba[0] = WGL_TEXTURE_FORMAT_ARB;    pba[1] = WGL_TEXTURE_RGBA_ARB;
    //pba[2] = WGL_TEXTURE_TARGET_ARB;

    params->pfa.PushBack(WGL_SUPPORT_OPENGL_ARB);       // P-buffer will be used with OpenGL
    params->pfa.PushBack(GL_TRUE);

    params->pfa.PushBack(WGL_DRAW_TO_PBUFFER_ARB);      // Enable render to p-buffer
    params->pfa.PushBack(GL_TRUE);

    params->pfa.PushBack(WGL_BIND_TO_TEXTURE_RGBA_ARB); // P-buffer will be used as a texture
    params->pfa.PushBack(GL_TRUE);

    //params->pfa.PushBack(WGL_DOUBLE_BUFFER_ARB);        // We don't require double buffering
    //params->pfa.PushBack(GL_FALSE);

    //pfa[0] = WGL_SUPPORT_OPENGL_ARB;        pfa[1] = GL_TRUE;    // P-buffer will be used with OpenGL
    //pfa[2] = WGL_DRAW_TO_PBUFFER_ARB;       pfa[3] = GL_TRUE;    // Enable render to p-buffer
    //pfa[4] = WGL_BIND_TO_TEXTURE_RGBA_ARB;  pfa[5] = GL_TRUE;    // P-buffer will be used as a texture
    ////pfa[6] = WGL_DOUBLE_BUFFER_ARB;        pfa[7] = GL_FALSE;    // We don't require double buffering

    // Define the pixel format requirements we will need for our
    // p-buffer. A p-buffer is just like a frame buffer, it can have a depth
    // buffer associated with it and it can be double buffered.
    /*
    pfa[pfa_free_pos] = WGL_RED_BITS_ARB;
    pfa[pfa_free_pos + 2] = WGL_GREEN_BITS_ARB;
    pfa[pfa_free_pos + 4] = WGL_BLUE_BITS_ARB;
    pfa[pfa_free_pos + 6] = WGL_ALPHA_BITS_ARB;
    */

    params->compressed = false;
    params->usingAlpha = usingAlpha = (header->PixelFormat.Flags & DDS_ALPHA_PIXELS) != 0;
    
    // figure out what the image format is
    params->pba.PushBack(WGL_TEXTURE_FORMAT_ARB);
    if (header->PixelFormat.Flags & DDS_FOURCC)
    {
        params->compressed = true;
        switch(header->PixelFormat.FourCC)
        {
            case DDS_DXT1: // DXT1's compression ratio is 8:1
                params->pba.PushBack(usingAlpha ? WGL_TEXTURE_RGBA_ARB : WGL_TEXTURE_RGB_ARB);
                params->tformat = usingAlpha ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
                this->format = DXT1;
                n_printf("DDS texture format: DDS_DXT1\n");
                break;
            case DDS_DXT3: // DXT3's compression ratio is 4:1
                params->pba.PushBack(WGL_TEXTURE_RGBA_ARB);
                params->tformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                this->format = DXT3;
                n_printf("DDS texture format: DDS_DXT3\n");
                break;
            case DDS_DXT5: // DXT5's compression ratio is 4:1
                params->pba.PushBack(WGL_TEXTURE_RGBA_ARB);
                params->tformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                this->format = DXT5;
                n_printf("DDS texture format: DDS_DXT5\n");
                break;

            default:
                n_printf("The file \"%s\" has not supported copmpressed format:  %d!\n",
                    this->GetFilename().Get(), (uint) header->PixelFormat.FourCC);
                return false;
        }
    }
    else if (header->PixelFormat.Flags & DDS_RGB)
    {
        params->pfa.PushBack(WGL_RED_BITS_ARB); params->pfa.PushBack(8);
        //params->pfa.PushBack(WGL_RED_BITS_ARB); params->pfa.PushBack(bitsCount(header->PixelFormat.RBitMask));
        //params->pfa.PushBack(WGL_RED_SHIFT_ARB); params->pfa.PushBack(firstSetBitPos(header->PixelFormat.RBitMask));

        params->pfa.PushBack(WGL_GREEN_BITS_ARB); params->pfa.PushBack(8);
        //params->pfa.PushBack(WGL_GREEN_BITS_ARB); params->pfa.PushBack(bitsCount(header->PixelFormat.GBitMask));
        //params->pfa.PushBack(WGL_GREEN_SHIFT_ARB); params->pfa.PushBack(firstSetBitPos(header->PixelFormat.GBitMask));

        params->pfa.PushBack(WGL_BLUE_BITS_ARB); params->pfa.PushBack(8);
        //params->pfa.PushBack(WGL_BLUE_BITS_ARB); params->pfa.PushBack(bitsCount(header->PixelFormat.BBitMask));
        //params->pfa.PushBack(WGL_BLUE_SHIFT_ARB); params->pfa.PushBack(firstSetBitPos(header->PixelFormat.BBitMask));

        //pfa[pfa_free_pos++] = WGL_RED_BITS_ARB;    pfa[pfa_free_pos++] = 8;
        ////pfa[pfa_free_pos++] = WGL_RED_BITS_ARB;    pfa[pfa_free_pos++] = bitsCount(header->PixelFormat.RBitMask);
        ////pfa[pfa_free_pos++] = WGL_RED_SHIFT_ARB;   pfa[pfa_free_pos++] = firstSetBitPos(header->PixelFormat.RBitMask);
                                                                   
        //pfa[pfa_free_pos++] = WGL_GREEN_BITS_ARB;    pfa[pfa_free_pos++] = 8;
        ////pfa[pfa_free_pos++] = WGL_GREEN_BITS_ARB;  pfa[pfa_free_pos++] = bitsCount(header->PixelFormat.GBitMask);
        ////pfa[pfa_free_pos++] = WGL_GREEN_SHIFT_ARB; pfa[pfa_free_pos++] = firstSetBitPos(header->PixelFormat.GBitMask);
                                                                   
        //pfa[pfa_free_pos++] = WGL_BLUE_BITS_ARB;    pfa[pfa_free_pos++] = 8;
        ////pfa[pfa_free_pos++] = WGL_BLUE_BITS_ARB;   pfa[pfa_free_pos++] = bitsCount(header->PixelFormat.BBitMask);
        ////pfa[pfa_free_pos++] = WGL_BLUE_SHIFT_ARB;  pfa[pfa_free_pos++] = firstSetBitPos(header->PixelFormat.BBitMask);
        
        params->bytesPerPixel = header->PixelFormat.RGBBitCount >> 3;
        if (usingAlpha)
        {
            params->pba.PushBack(WGL_TEXTURE_RGBA_ARB);
            //pba[1] = WGL_TEXTURE_RGBA_ARB;
            
            params->pfa.PushBack(WGL_ALPHA_BITS_ARB); params->pfa.PushBack(8);
            //params->pfa.PushBack(WGL_ALPHA_BITS_ARB); params->pfa.PushBack(bitsCount(header->PixelFormat.RGBAlphaBitMask));
            //params->pfa.PushBack(WGL_ALPHA_SHIFT_ARB); params->pfa.PushBack(firstSetBitPos(header->PixelFormat.RGBAlphaBitMask));

            //pfa[pfa_free_pos++] = WGL_ALPHA_BITS_ARB;   pfa[pfa_free_pos++] = 8;
            ////pfa[pfa_free_pos++] = WGL_ALPHA_BITS_ARB;   pfa[pfa_free_pos++] = bitsCount(header->PixelFormat.RGBAlphaBitMask);
            ////pfa[pfa_free_pos++] = WGL_ALPHA_SHIFT_ARB;  pfa[pfa_free_pos++] = firstSetBitPos(header->PixelFormat.RGBAlphaBitMask);
            
            params->iformat = GL_RGBA;
            params->tformat = firstSetBitPos(header->PixelFormat.BBitMask) > firstSetBitPos(header->PixelFormat.RBitMask) ? GL_BGRA_EXT : GL_RGBA;

            if (header->PixelFormat.RGBBitCount == 32)
            {
                if (bitsCount(header->PixelFormat.ABitMask) == 8)
                {
                    this->format = A8R8G8B8;
                    params->ttype = GL_UNSIGNED_INT_8_8_8_8;
                    n_printf("DDS texture format(%d bit): DDS_A8R8G8B8\n", (uint) header->PixelFormat.RGBBitCount);
                }
                else
                {
                    //this->format = A2R10G10B10;
                    //ttype = GL_UNSIGNED_INT_10_10_10_2;
                    //n_printf("DDS texture format(%d bit): DDS_A2R10G10B10\n", (uint) header->PixelFormat.RGBBitCount);
                    n_printf("nGLTexture::LoadFromDDSCompoundFile: The file \"%s\" has not supported RGBA format R(%d), G(%d), B(%d), A(%d)!\n",
                        this->GetFilename().Get(),
                        bitsCount(header->PixelFormat.RBitMask),
                        bitsCount(header->PixelFormat.GBitMask),
                        bitsCount(header->PixelFormat.BBitMask),
                        bitsCount(header->PixelFormat.ABitMask));
                    return false;
                }
            }
            else if (header->PixelFormat.RGBBitCount == 16)
            {
                if (bitsCount(header->PixelFormat.ABitMask) == 1)
                {
                    this->format = A1R5G5B5;
                    params->ttype = GL_UNSIGNED_SHORT_5_5_5_1;
                    n_printf("DDS texture format(%d bit): DDS_A1R5G5B5\n", (uint) header->PixelFormat.RGBBitCount);
                }
                else
                {
                    this->format = A4R4G4B4;
                    params->ttype = GL_UNSIGNED_SHORT_4_4_4_4;
                    n_printf("DDS texture format(%d bit): DDS_A4R4G4B4\n", (uint) header->PixelFormat.RGBBitCount);
                }
            }
            else
            {
                n_printf("nGLTexture::LoadFromDDSCompoundFile: The file \"%s\" has not supported bit count (%d bits) for RGBA image!\n",
                    this->GetFilename().Get(), (uint) header->PixelFormat.RGBBitCount);
                return false;
            }
        }
        else // no alfa
        {
            params->pba.PushBack(WGL_TEXTURE_RGB_ARB);
            //pba[1] = WGL_TEXTURE_RGB_ARB;
            
            params->iformat = GL_RGB;
            params->tformat = firstSetBitPos(header->PixelFormat.BBitMask) > firstSetBitPos(header->PixelFormat.RBitMask) ? GL_BGR_EXT : GL_RGB;
            
            if (header->PixelFormat.RGBBitCount == 32)
            {
                params->iformat = GL_RGBA;
                if (params->tformat == GL_BGR_EXT) params->tformat = GL_BGRA_EXT;
                else params->tformat = GL_RGBA;

                this->format = X8R8G8B8;
                params->ttype = GL_UNSIGNED_INT_8_8_8_8;
                n_printf("DDS texture format(%d bit): DDS_X8R8G8B8\n", (uint) header->PixelFormat.RGBBitCount);
            }
            else if (header->PixelFormat.RGBBitCount == 24)
            {
                this->format = X8R8G8B8;
                params->ttype = GL_UNSIGNED_INT_8_8_8_8;
                n_printf("DDS texture format(%d bit): DDS_X8R8G8B8\n", (uint) header->PixelFormat.RGBBitCount);
            }
            else if (header->PixelFormat.RGBBitCount == 16)
            {
                this->format = R5G6B5;
                params->ttype = GL_UNSIGNED_SHORT_5_6_5;
                n_printf("DDS texture format(%d bit): DDS_R5G6B5\n", (uint) header->PixelFormat.RGBBitCount);
            }
            else
            {
                n_printf("nGLTexture::LoadFromDDSCompoundFile: The file \"%s\" has not supported bit count (%d bits) for RGBA image!\n",
                    this->GetFilename().Get(), (uint) header->PixelFormat.RGBBitCount);
                return false;
            }
        }
    }
/*
    else if (header->PixelFormat.RGBBitCount == 8)// check this
    {
        pba[1] = WGL_TEXTURE_RGB_ARB;
        pfa[1]    = 8;    // At least 8 bits for RED channel
        pfa[3]    = 0;    // At least 8 bits for GREEN channel
        pfa[5]    = 0;    // At least 8 bits for BLUE channel
        pfa[7]    = 0;    // At least 0 bits for ALPHA channel
        //iformat = usingAlfa ? 4 : 3;
        iformat = 1;
        bytesPerPixel = 1;
        //tformat = usingAlfa ? GL_RGBA  : GL_RGB;
        format = GL_LUMINANCE;
        ttype = GL_UNSIGNED_BYTE;
        this->format = P8;
        n_printf("DDS texture format(%d bit): DDS_P8\n", (uint) header->PixelFormat.RGBBitCount);
    }
*/
    else
    {
        n_printf("nGLTexture::LoadFromDDSCompoundFile: The file \"%s\" has not supported image format:  %d!\n",
            this->GetFilename().Get(), (uint)header->PixelFormat.RGBBitCount);
        return false;
    }

    int pt;
    this->SetType(TEXTURE_2D);
    target = GL_TEXTURE_2D;
    pt = WGL_TEXTURE_2D_ARB;
    //pba[3] = WGL_TEXTURE_2D_ARB;

    if (header->Caps1 & DDS_COMPLEX)
    {
        if (header->Caps2 & DDS_CUBEMAP)
        {
            this->SetType(TEXTURE_CUBE);
            target = GL_TEXTURE_CUBE_MAP;
            pt = WGL_TEXTURE_CUBE_MAP_ARB;
            //pba[3] = WGL_TEXTURE_CUBE_MAP_ARB;
        }
        //else if ((header->Caps2 & DDS_VOLUME) && (header->Depth > 0))
        //{
        //    this->SetType(TEXTURE_3D);
        //    target = GL_TEXTURE_3D;
        //    pt = WGL_TEXTURE_3D_ARB;
        //    //pba[3] = WGL_TEXTURE_3D_ARB;
        //}
    }

    params->pba.PushBack(WGL_TEXTURE_TARGET_ARB);
    params->pba.PushBack(pt);

    if (this->GetType() != TEXTURE_2D && this->GetType() != TEXTURE_CUBE)
    {
        // unsupported texture type
        n_error("nGLTexture::CreateEmptyTexture(): Unsupported texture type!\n");
        return false;
    }

    this->SetWidth(header->Width);
    this->SetHeight(header->Height);
    this->SetDepth(clamp_size(header->Depth));

    params->pba.PushBack(0);
    params->pfa.PushBack(0);

    return true;
}

//------------------------------------------------------------------------------
/**
    Create a pixel buffer.
*/
bool
nGLTexture::CreatePBuffer(const int *pfa, const int *pba)
{
    nGLServer2* gfxServer = (nGLServer2*)nGfxServer2::Instance();
    HDC hDC = gfxServer->hDC;
    HGLRC hRC = gfxServer->context;
    n_assert(hDC);
    n_assert(hRC);

	int pf_attr[] =
	{
		WGL_SUPPORT_OPENGL_ARB, TRUE,       // P-buffer will be used with OpenGL
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // Enable render to p-buffer
		WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE, // P-buffer will be used as a texture
		WGL_RED_BITS_ARB, 8,                // At least 8 bits for RED channel
		WGL_GREEN_BITS_ARB, 8,              // At least 8 bits for GREEN channel
		WGL_BLUE_BITS_ARB, 8,               // At least 8 bits for BLUE channel
		WGL_ALPHA_BITS_ARB, 8,              // At least 8 bits for ALPHA channel
		WGL_DEPTH_BITS_ARB, 16,             // At least 16 bits for depth buffer
		WGL_DOUBLE_BUFFER_ARB, FALSE,       // We don't require double buffering
		0                                   // Zero terminates the list
	};

    //choose pixel format
    unsigned int count = 0;
    int pixelFormat;
    wglChoosePixelFormatARB(hDC,(const int*)pf_attr, NULL, 1, &pixelFormat, &count);
    n_gltrace("nGLTexture::CreatePBuffer(): Some error occured while choosing pixel format.");

    if (count == 0)
    {
        n_printf("nGLTexture::CreatePBuffer: Could not find an acceptable pixel format for <%s>.\n",this->GetFilename());
        n_gltrace("nGLTexture::CreatePBuffer().");
        return false;
    }

    // Create the p-buffer...
    // but not work. Why?
    hPBuffer = wglCreatePbufferARB(hDC, pixelFormat, this->width, this->height, pba);
    if (!hPBuffer)
    {
        n_printf("nGLTexture: could not create the p-buffer for <%s>",this->GetName());
        n_gltrace("nGLTexture::CreatePBuffer().");
        return false;
    }
    hPBufferDC = wglGetPbufferDCARB(hPBuffer);
    hPBufferRC = wglCreateContext(hPBufferDC);

    int h;
    int w;
    wglQueryPbufferARB(hPBuffer, WGL_PBUFFER_WIDTH_ARB, &h);
    wglQueryPbufferARB(hPBuffer, WGL_PBUFFER_WIDTH_ARB, &w);

    if (h != this->height || w != this->width)
    {
        n_printf("nGLTexture: The width and height of the created p-buffer don't match the requirements for <%s>",this->GetName());
        n_gltrace("nGLTexture::CreatePBuffer().");
        return false;
    }
    n_gltrace("nGLTexture::CreatePBuffer().");
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the file through DevIL. This method should only be used in
    tools and viewers, not in an actual game application, because a lot
    of conversion happens in here, and all loaded textures are converted
    to 32 bit BGRA, which is not very memory efficient.
*/
#if USE_DEVIL
bool
nGLTexture::LoadILFile()
{
    n_assert(!this->GetFilename().IsEmpty());
    n_assert(0 != this->texID);
    //n_assert(0 == this->texture2D);
    //n_assert(0 == this->textureCube);

    n_printf("DevIL: start texture loading <%s>.\n", this->GetFilename().Get());

    nGLServer2* gfxServer = (nGLServer2*)nGfxServer2::Instance();
    HDC hDC = gfxServer->hDC;
    HGLRC hRC = gfxServer->context;
    n_assert(hDC);
    n_assert(hRC);

    // p-buffer attributes
    int pba[] =
    {
        WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB, // Our p-buffer will have a texture format of RGBA
        WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,   // Of texture target will be GL_TEXTURE_2D
        0                                             // Zero terminates the list
    };

    // Define the pixel format requirements we will need for our
    // p-buffer. A p-buffer is just like a frame buffer, it can have a depth
    // buffer associated with it and it can be double buffered.
    int pfa[] =
    {
        WGL_SUPPORT_OPENGL_ARB, TRUE,       // P-buffer will be used with OpenGL
        WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // Enable render to p-buffer
        WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE, // P-buffer will be used as a texture
        WGL_RED_BITS_ARB, 8,                // At least 8 bits for RED channel
        WGL_GREEN_BITS_ARB, 8,              // At least 8 bits for GREEN channel
        WGL_BLUE_BITS_ARB, 8,               // At least 8 bits for BLUE channel
        WGL_ALPHA_BITS_ARB, 8,              // At least 8 bits for ALPHA channel
        //WGL_DOUBLE_BUFFER_ARB, FALSE,       // We don't require double buffering
        0                                   // Zero terminates the list
    };

    if (N_GL_EXTENSION_SUPPORTED(WGL_ARB_render_texture))
    {
        if (CreatePBuffer(pfa,pba))
            // We were successful in creating a p-buffer. We can now make its
            // context current and set it up just like we would a regular
            // context attached to a window.
            if (!wglMakeCurrent(hPBufferDC,hPBufferRC))
            {
                n_printf("nGLTexture: Could not make the p-buffer's context current for <%s>",this->GetName());
                return false;
            }
    }

    // mangle pathname
    nString mangledPath;
    mangledPath = nFileServer2::Instance()->ManglePath(this->GetFilename().Get());

    // initialize DevIL
    ilInit();
    iluInit();

    // create IL image and load
    ilEnable(IL_CONV_PAL);
    ILuint image = iluGenImage();
    ilBindImage(image);
    if (!ilLoadImage((char*)mangledPath.Get()))
    {
        n_printf("nGLTexture: ilLoadImage() failed loading file '%s'\n", mangledPath);
        iluDeleteImage(image);
        return false;
    }

    // always convert the image to BGRA format
    ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);

    // get relevant image data and create an empty d3d9 texture
    int imageWidth = ilGetInteger(IL_IMAGE_WIDTH);
    int imageHeight = ilGetInteger(IL_IMAGE_HEIGHT);

    //generating texture
    glGenTextures(1, &this->texID);
    n_gltrace("nGLTexture::LoadILFile(): Can't generate texture id.");
    glBindTexture(GL_TEXTURE_2D, this->texID);
    n_gltrace("nGLTexture::LoadILFile(): Can't bind texture.");

    n_assert(this->texID != 0);

    //int pb_2d_attr[] = {WGL_MIPMAP_LEVEL_ARB,0,0};

    //wglSetPbufferAttribARB(hPBuffer,pb_2d_attr);
    // transfer image data to surface
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        4,
        imageWidth,
        imageHeight,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        (GLvoid*)ilGetData());
    n_gltrace("nGLTexture::LoadILFile(): Can't load texture.");

    this->SetFormat(A8R8G8B8);
    this->SetType(TEXTURE_2D);
    this->SetWidth(imageWidth);
    this->SetHeight(imageHeight);
    this->SetDepth(1);
    this->SetNumMipLevels(1);

    // cleanup
    iluDeleteImage(image);
    ilShutDown();

    n_printf("DevIL: stop texture loading.\n");
    return true;
}
#endif
