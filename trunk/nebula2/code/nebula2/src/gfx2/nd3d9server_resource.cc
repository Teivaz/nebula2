//------------------------------------------------------------------------------
//  nd3d9server_resource.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "resource/nresourceserver.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nfont2.h"
#include "gfx2/nd3d9mesharray.h"
#include "gfx2/nd3d9shader.h"

//------------------------------------------------------------------------------
/**
    Create a new shared mesh object. If the object already exists, its refcount
    is increment.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nD3D9Mesh2 object
*/
nMesh2*
nD3D9Server::NewMesh(const char* rsrcName)
{
    return (nMesh2*) this->refResource->NewResource("nd3d9mesh", rsrcName, nResource::Mesh);
}

//------------------------------------------------------------------------------
/**
    Create a new mesh array object.

    @return             pointer to a nD3D9MeshArray object
*/
nMeshArray*
nD3D9Server::NewMeshArray(const char* rsrcName)
{
    return ((nMeshArray*) this->refResource->NewResource("nd3d9mesharray", rsrcName, nResource::Mesh));
}

//------------------------------------------------------------------------------
/**
    Create a new shared texture object. If the object already exists, its
    refcount is incremented.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nD3D9Texture2 object
*/
nTexture2*
nD3D9Server::NewTexture(const char* rsrcName)
{
    return (nTexture2*) this->refResource->NewResource("nd3d9texture", rsrcName, nResource::Texture);
}

//------------------------------------------------------------------------------
/**
    Create a new shared shader object. If the object already exists, its
    refcount is incremented.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nD3D9Shader2 object
*/
nShader2*
nD3D9Server::NewShader(const char* rsrcName)
{
    return (nShader2*) this->refResource->NewResource("nd3d9shader", rsrcName, nResource::Shader);
}

//------------------------------------------------------------------------------
/**
    Create a new shared font object. If the object already exists, its
    refcount is incremented.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nD3D9Shader2 object
*/
nFont2*
nD3D9Server::NewFont(const char* rsrcName, const nFontDesc& fontDesc)
{
    nFont2* font = (nFont2*) this->refResource->NewResource("nd3d9font", rsrcName, nResource::Font);
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
    @param  usageFlags  a combination of nTexture2::Usage flags
*/
nTexture2*
nD3D9Server::NewRenderTarget(const char* rsrcName,
                             int width,
                             int height,
                             nTexture2::Format format,
                             int usageFlags)
{
    nTexture2* renderTarget = (nTexture2*) this->refResource->NewResource("nd3d9texture", rsrcName, nResource::Texture);
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
    Unloads all resources used by the D3D8Server2 object. This method must
    be called when the d3d device has been lost.
*/
void
nD3D9Server::OnDeviceLost()
{
    // close the text renderer
    this->CloseTextRenderer();

    // release other resources
    this->refResource->UnloadResources(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);

    // release shape shader
    if (this->refShapeShader.isvalid())
    {
        this->refShapeShader->Release();
        this->refShapeShader.invalidate();
    }

    // release the shared state shader
    if (this->refSharedShader.isvalid())
    {
        this->refSharedShader->Release();
        this->refSharedShader.invalidate();
    }

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
    
    // inform line renderer
    HRESULT hr = this->d3dxLine->OnLostDevice();
    n_dxtrace(hr, "OnLostDevice() on d3dxLine failed");

    #ifdef __NEBULA_STATS__
    // release the d3d query object
    if (this->queryResourceManager)
    {
        this->queryResourceManager->Release();
        this->queryResourceManager = 0;
    }
    #endif
}

//------------------------------------------------------------------------------
/**
    Reload all resources used by the nD3D8Server2 object. This method must
    be called when the d3d device has been restored.
*/
void
nD3D9Server::OnRestoreDevice()
{
    n_assert(!this->refSharedShader.isvalid());
    n_assert(0 == this->depthStencilSurface);

    HRESULT hr;

    // get a pointer to the back buffer and depth/stencil surface
    hr = this->d3d9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &(this->backBufferSurface));
    n_dxtrace(hr, "GetBackBuffer() on device failed.");
    n_assert(this->backBufferSurface);

    hr = this->d3d9Device->GetDepthStencilSurface(&this->depthStencilSurface);
    n_dxtrace(hr, "GetDepthStencilSurface() on device failed.");
    n_assert(this->depthStencilSurface);

    #ifdef __NEBULA_STATS__
    // create a query object for resource manager queries
    // this will fail if not running the debug runtime, so this is not critical
    hr = this->d3d9Device->CreateQuery(D3DQUERYTYPE_RESOURCEMANAGER, &(this->queryResourceManager));
    #endif
    
    // (re-)-load resources
    this->refResource->ReloadResources(nResource::Mesh | nResource::Texture | nResource::Shader | nResource::Font);

    // open the text renderer
    this->OpenTextRenderer();

    // inform line renderer
    hr = this->d3dxLine->OnResetDevice();
    n_dxtrace(hr, "OnResetDevice() on d3dxLine failed");

    // update mouse cursor
    this->cursorDirty = true;
    this->UpdateCursor();

    // create the shape shader
    this->refShapeShader = (nD3D9Shader*) this->NewShader("shape");
    if (!this->refShapeShader->IsValid())
    {
        this->refShapeShader->SetFilename("shaders:shape.fx");
        if (!this->refShapeShader->Load())
        {
            this->refShapeShader->Release();
            this->refShapeShader.invalidate();
        }
    }

    // create the shared effect parameter reference shader
    this->refSharedShader = (nD3D9Shader*) this->NewShader("shared");
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
