//------------------------------------------------------------------------------
//  nglserver2_resource.cc
//  29-Jun-2003 cubejk cloned from nd3d9server_resource.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "resource/nresourceserver.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nfont2.h"
#include "opengl/ncgfxshader.h"

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
    n_printf("New Mesh: %s.\n", rsrcName);
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
    n_printf("New Texture: %s.\n", rsrcName);
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
    n_printf("New Shader: %s.\n", rsrcName);
    return (nShader2*) this->refResource->NewResource("ncgfxshader", rsrcName, nResource::Shader);
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
    n_printf("New Font: %s.\n", rsrcName);
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
    Unloads all resources used by the GLServer2 object. This method must
    be called when the gl device has been lost.
*/
void
nGLServer2::OnDeviceLost(bool unloadManaged)
{
    // close the text renderer
    //this->CloseTextRenderer();

    // release other resources
    if (unloadManaged)
    {
        this->refResource->UnloadResources(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);
    }
    else
    {
        this->refResource->UnloadResources(nResource::Mesh | nResource::Shader);
    }

    // release shape shader
/*    if (this->refShapeShader.isvalid())
    {
        this->refShapeShader->Release();
        this->refShapeShader.invalidate();
    }
*/
    // release the shared state shader
    if (this->refSharedShader.isvalid())
    {
        this->refSharedShader->Release();
        this->refSharedShader.invalidate();
    }
/*
    // release refs on original backbuffer and depth/stencil surfaces
    if (this->backBufferSurface)
    {
        this->backBufferSurface->Release();
        this->backBufferSurface = 0;
    }
    if (this->depthStencilSurface)
    {
        this->depthStencilSurface->Release();
        this->depthStencilSurface = 0;
    }

    #ifdef __NEBULA_STATS__
    // release the d3d query object
    if (this->queryResourceManager)
    {
        this->queryResourceManager->Release();
        this->queryResourceManager = 0;
    }
    #endif
*/
}

//------------------------------------------------------------------------------
/**
    Reload all resources used by the nD3D8Server2 object. This method must
    be called when the d3d device has been restored.
*/
void
nGLServer2::OnRestoreDevice()
{
/*
    HRESULT hr;

    // get a pointer to the back buffer and depth/stencil surface
    hr = this->d3d9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &(this->backBufferSurface));
    n_assert(SUCCEEDED(hr));
    n_assert(this->backBufferSurface);

    hr = this->d3d9Device->GetDepthStencilSurface(&this->depthStencilSurface);
    n_assert(SUCCEEDED(hr));
    n_assert(this->depthStencilSurface);

    #ifdef __NEBULA_STATS__
    // create a query object for resource manager queries
    // this will fail if not running the debug runtime, so this is not critical
    hr = this->d3d9Device->CreateQuery(D3DQUERYTYPE_RESOURCEMANAGER, &(this->queryResourceManager));
    #endif
*/
    // (re-)-load resources
    this->refResource->ReloadResources(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);

    // open the text renderer
    //this->OpenTextRenderer();

    // flag the mouse cursor to be reset
    this->cursorDirty = true;
    this->UpdateCursor();

    // create the shared effect parameter reference shader
    this->refSharedShader = (nCgFXShader*) this->NewShader("shared");
    if (!this->refSharedShader->IsValid())
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
