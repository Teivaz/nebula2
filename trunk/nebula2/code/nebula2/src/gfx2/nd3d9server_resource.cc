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
#include "gfx2/nd3d9occlusionquery.h"

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
    if (!renderTarget->IsLoaded())
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
nD3D9Server::NewOcclusionQuery()
{
    return n_new(nD3D9OcclusionQuery);
}
