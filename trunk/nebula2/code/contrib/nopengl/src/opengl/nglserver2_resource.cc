//------------------------------------------------------------------------------
//  nglserver2_resource.cc
//  29-Jun-2003 cubejk cloned from nd3d9server_resource.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"

#include "gfx2/nshader2.h"
#include "resource/nresourceserver.h"

//#include "gfx2/nmesh2.h"
//#include "gfx2/nfont2.h"
//#include "opengl/ncgfxshader.h"

//------------------------------------------------------------------------------
/**
    Create a new shared mesh object. If the object already exists, its refcount
    is increment.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nD3D8Mesh2 object
*/
nMesh2*
nGLServer2::NewMesh(const char* rsrcName)
{
    n_printf("\nNew Mesh: %s.\n", rsrcName);
    return (nMesh2*) this->refResource->NewResource("nglmesh", rsrcName, nResource::Mesh);
}

//------------------------------------------------------------------------------
/**
    Create a new mesh array object.

    @return             pointer to a nGLMeshArray object
*/
nMeshArray*
nGLServer2::NewMeshArray(const char* rsrcName)
{
    //return ((nMeshArray*) this->refResource->NewResource("nglmesharray", rsrcName, nResource::Mesh));
    return NULL;
}

//------------------------------------------------------------------------------
/**
    Create a new shared texture object. If the object already exists, its
    refcount is incremented.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nD3D8Texture2 object
*/
nTexture2*
nGLServer2::NewTexture(const char* rsrcName)
{
    n_printf("\nNew Texture: %s.\n", rsrcName);
    return (nTexture2*) this->refResource->NewResource("ngltexture", rsrcName, nResource::Texture);
}
//------------------------------------------------------------------------------
/**
    Create a new shared shader object. If the object already exists, its
    refcount is incremented.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nCgGLShaderObject object
*/
nShader2*
nGLServer2::NewShader(const char* rsrcName)
{
    n_printf("\nNew Shader: %s.\n", rsrcName);
    return (nShader2*) this->refResource->NewResource("nglslshader", rsrcName, nResource::Shader);
    //return (nShader2*) this->refResource->NewResource("ncgfxshader", rsrcName, nResource::Shader);
}

//------------------------------------------------------------------------------
/**
    Create a new shared font object. If the object already exists, its
    refcount is incremented.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nCgFXShader object
*/
nFont2*
nGLServer2::NewFont(const char* rsrcName, const nFontDesc& fontDesc)
{
    n_printf("\nNew Font: %s.\n", rsrcName);
    nFont2* font = (nFont2*) this->refResource->NewResource("nglfont", rsrcName, nResource::Font);
    n_assert(font);
    font->SetFontDesc(fontDesc);
    return font;
}

//------------------------------------------------------------------------------
/**
    Create a new render target object.

    @param  rsrcName    a resource name for resource sharing
    @param  width       width of render target
    @param  height      height of render target
    @param  format      pixel format of render target
    @param  usageFlags  combination of #nTexture2::Usage flags
*/
nTexture2*
nGLServer2::NewRenderTarget(const char* rsrcName,
                             int width,
                             int height,
                             nTexture2::Format format,
                             int usageFlags)
{
    nTexture2* renderTarget = (nTexture2*) this->refResource->NewResource("ngltexture", rsrcName, nResource::Texture);
    n_assert(renderTarget);
    if (!renderTarget->IsValid())
    {
        n_assert(0 != (usageFlags & (nTexture2::RenderTargetColor | nTexture2::RenderTargetDepth | nTexture2::RenderTargetStencil)));
        renderTarget->SetUsage(usageFlags);
        renderTarget->SetWidth(width);
        renderTarget->SetHeight(height);
        renderTarget->SetDepth(1);
        renderTarget->SetFormat(format);
        renderTarget->SetType(nTexture2::TEXTURE_2D);
        bool success = renderTarget->Load();
        if (!success)
        {
            renderTarget->Release();
            return 0;
        }
    }
    return renderTarget;
}

//------------------------------------------------------------------------------
/**
    Create a new occlusion query object.

    @return     pointer to a new occlusion query object
*/
nOcclusionQuery*
nGLServer2::NewOcclusionQuery()
{
    return NULL; //n_new(nGLOcclusionQuery);
}

//------------------------------------------------------------------------------
/**
    Called either when the gl device is lost, or when the display is closed.
    This should perform the required cleanup work on all affected resources.
*/
void
nGLServer2::OnDeviceCleanup(bool shutdown)
{
    // close the text renderer
    //this->CloseTextRenderer();

    // handle resources
    if (shutdown)
    {
        // the display is about to be closed, do a real unload
        this->refResource->UnloadResources(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);
    }
    else
    {
        // the device has been lost, tell the resources about it
        this->refResource->OnLost(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);
    }

    // release shape shader
    //if (this->refShapeShader.isvalid())
    //{
    //    this->refShapeShader->Release();
    //    this->refShapeShader.invalidate();
    //}

    // release the shared state shader
    if (this->refSharedShader.isvalid())
    {
        this->refSharedShader->Release();
        this->refSharedShader.invalidate();
    }

    // release refs on original backbuffer and depth/stencil surfaces
    //if (this->captureSurface)
    //{
    //    this->captureSurface->Release();
    //    this->captureSurface = 0;
    //}
    //if (this->backBufferSurface)
    //{
    //    this->backBufferSurface->Release();
    //    this->backBufferSurface = 0;
    //}
    //if (this->depthStencilSurface)
    //{
    //    this->depthStencilSurface->Release();
    //    this->depthStencilSurface = 0;
    //}

    // inform line renderer
    //HRESULT hr = this->d3dxLine->OnLostDevice();
    //n_dxtrace(hr, "OnLostDevice() on d3dxLine failed");

    #ifdef __NEBULA_STATS__
    // release the d3d query object
    //if (this->queryResourceManager)
    //{
    //    this->queryResourceManager->Release();
    //    this->queryResourceManager = 0;
    //}
    #endif
}

//------------------------------------------------------------------------------
/**
    This method is called either after the software device has been
    created, or after the device has been restored.
*/
void
nGLServer2::OnDeviceInit(bool startup)
{
    n_assert(!this->refSharedShader.isvalid());
/*
    n_assert(0 == this->depthStencilSurface);
    n_assert(0 == this->backBufferSurface);
    n_assert(0 == this->captureSurface);

    HRESULT hr;

    // get a pointer to the back buffer and depth/stencil surface
    hr = this->d3d9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &(this->backBufferSurface));
    n_dxtrace(hr, "GetBackBuffer() on device failed.");
    n_assert(this->backBufferSurface);

    hr = this->d3d9Device->GetDepthStencilSurface(&this->depthStencilSurface);
    n_dxtrace(hr, "GetDepthStencilSurface() on device failed.");
    n_assert(this->depthStencilSurface);

    // create an offscreen surface for capturing data
    hr = this->d3d9Device->CreateOffscreenPlainSurface(this->presentParams.BackBufferWidth,
                                                       this->presentParams.BackBufferHeight,
                                                       this->presentParams.BackBufferFormat,
                                                       D3DPOOL_SYSTEMMEM,
                                                       &(this->captureSurface), NULL);
    n_dxtrace(hr, "CreateOffscreenPlainSurface() failed.");
    n_assert(this->captureSurface);

    #ifdef __NEBULA_STATS__
    // create a query object for resource manager queries
    // this will fail if not running the debug runtime, so this is not critical
    hr = this->d3d9Device->CreateQuery(D3DQUERYTYPE_RESOURCEMANAGER, &(this->queryResourceManager));
    #endif
*/
    // restore or load resources
    if (startup)
    {
        // this is a real startup
        this->refResource->LoadResources(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);
    }
    else
    {
        // the device has been restored...
        this->refResource->OnRestored(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);
    }

    // open the text renderer
    //this->OpenTextRenderer();

    // inform line renderer
    //hr = this->d3dxLine->OnResetDevice();
    //n_dxtrace(hr, "OnResetDevice() on d3dxLine failed");

    // update mouse cursor
    this->cursorDirty = true;
    this->UpdateCursor();

    // create the shape shader
    //this->refShapeShader = (nD3D9Shader*) this->NewShader("shape");
    //if (!this->refShapeShader->IsLoaded())
    //{
    //    this->refShapeShader->SetFilename("shaders:shape.fx");
    //    if (!this->refShapeShader->Load())
    //    {
    //        this->refShapeShader->Release();
    //        this->refShapeShader.invalidate();
    //    }
    //}

    // create the shared effect parameter reference shader
    this->refSharedShader = this->NewShader("shared");
    if (!this->refSharedShader->IsLoaded())
    {
        this->refSharedShader->SetFilename("shaders:shared.fx");
        if (!this->refSharedShader->Load())
        {
            this->refSharedShader->Release();
            this->refSharedShader.invalidate();
        }
    }

    // refresh projection matrix (necessary AFTER sharedShader has been created
    // so that the shared transform matrices can be set after a DeviceReset)
    this->SetCamera(this->GetCamera());
}
