#define N_IMPLEMENTS nD3D9Server
//------------------------------------------------------------------------------
//  nd3d9server_resource.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "resource/nresourceserver.h"
#include "gfx2/nmesh2.h"

//------------------------------------------------------------------------------
/**
    Create a new shared mesh object. If the object already exists, its refcount
    is increment.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nD3D8Mesh2 object
*/
nMesh2*
nD3D9Server::NewMesh(const char* rsrcName)
{
    return (nMesh2*) this->refResource->NewResource("nd3d9mesh", rsrcName, nResource::MESH);
}

//------------------------------------------------------------------------------
/**
    Create a new shared texture object. If the object already exists, its
    refcount is incremented.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nD3D8Texture2 object
*/
nTexture2*
nD3D9Server::NewTexture(const char* rsrcName)
{
    return (nTexture2*) this->refResource->NewResource("nd3d9texture", rsrcName, nResource::TEXTURE);
}

//------------------------------------------------------------------------------
/**
    Create a new shared shader object. If the object already exists, its
    refcount is incremented.

    @param  rsrcName    a resource name (used for resource sharing)
    @return             pointer to a nD3D8Shader2 object
*/
nShader2*
nD3D9Server::NewShader(const char* rsrcName)
{
    return (nShader2*) this->refResource->NewResource("nd3d9shader", rsrcName, nResource::SHADER);
}

//------------------------------------------------------------------------------
/**
    Create a new render target object.

    @param  rsrcName    a resource name for resource sharing
    @param  width       width of render target
    @param  height      height of render target
    @param  format      pixel format of render target
    @param  hasColor    true if render target has a color buffer
    @param  hasDepth    true if render target has a depth buffer
    @param  hasStencil  true if render target has a stencil buffer
*/
nTexture2*
nD3D9Server::NewRenderTarget(const char* rsrcName,
                             int width,
                             int height,
                             nTexture2::Format format,
                             bool hasColor,
                             bool hasDepth,
                             bool hasStencil)
{
    nTexture2* renderTarget = (nTexture2*) this->refResource->NewResource("nd3d9texture", rsrcName, nResource::TEXTURE);
    n_assert(renderTarget);
    if (!renderTarget->IsValid())
    {
        renderTarget->SetRenderTarget(hasColor, hasDepth, hasStencil);
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
nD3D9Server::UnloadResources()
{
    // release the dynamic mesh object
    this->refDynMesh->Release();

    // close the text renderer
    this->CloseTextRenderer();

    // release other resources
    this->refResource->UnloadResources(nResource::MESH);
    this->refResource->UnloadResources(nResource::TEXTURE);
    this->refResource->UnloadResources(nResource::SHADER);
}

//------------------------------------------------------------------------------
/**
    Reload all resources used by the nD3D8Server2 object. This method must
    be called when the d3d device has been restored.
*/
void
nD3D9Server::ReloadResources()
{
    this->refResource->ReloadResources(nResource::MESH);
    this->refResource->ReloadResources(nResource::TEXTURE);
    this->refResource->ReloadResources(nResource::SHADER);

    // open the text renderer
    this->OpenTextRenderer();
    
    // create the global dynamic mesh object
    n_assert(!this->refDynMesh.isvalid());
    this->refDynMesh = this->NewMesh(0);
    this->refDynMesh->SetUsage(nMesh2::WriteOnly);
    this->refDynMesh->SetNumVertices(DYNAMIC_VERTEXBUFFER_SIZE);
    this->refDynMesh->SetNumIndices(DYNAMIC_INDEXBUFFER_SIZE);
    this->refDynMesh->SetVertexComponents(nMesh2::Coord | nMesh2::Normal | nMesh2::Tangent | nMesh2::Uv0);
    this->refDynMesh->SetPrimitiveType(TRIANGLELIST);
    this->refDynMesh->Load();
}

//------------------------------------------------------------------------------
/**
    Claim ownership of the global dynamic mesh.
*/
nMesh2*
nD3D9Server::LockDynamicMesh()
{
    n_assert(!this->dynMeshLocked);
    this->dynMeshLocked = true;
    return this->refDynMesh.get();
}

//------------------------------------------------------------------------------
/**
    Give up ownership of the global dynamic mesh.
*/
void
nD3D9Server::UnlockDynamicMesh(nMesh2* mesh)
{
    n_assert(this->dynMeshLocked);
    n_assert(mesh == this->refDynMesh.get());
    this->dynMeshLocked = false;
}

