#define N_IMPLEMENTS nD3D9Server
//------------------------------------------------------------------------------
//  nd3d9server_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9mesh.h"
#include "gfx2/nd3d9texture.h"
#include "gfx2/nd3d9shader.h"

//------------------------------------------------------------------------------
/**
    Update the d3d projection matrix from the new camera settings.
*/
void
nD3D9Server::SetCamera(const nCamera2& cam)
{
    nGfxServer2::SetCamera(cam);

    if (this->d3d9Device)
    {
        matrix44 projRH;

        D3DXMatrixPerspectiveFovRH(
            (D3DXMATRIX*) &projRH,
            n_deg2rad(cam.GetAngleOfView()),
            cam.GetAspectRatio(),
            cam.GetNearPlane(),
            cam.GetFarPlane());

        // set projection matrices
        this->SetTransform(PROJECTION, projRH);
    }
}

//------------------------------------------------------------------------------
/**
    Start rendering the scene.
*/
bool
nD3D9Server::BeginScene()
{
    HRESULT hr;
    if (nGfxServer2::BeginScene())
    {
        n_assert(this->d3d9Device);
        this->inBeginScene = false;

        // check if d3d device is in a valid state
        if (!this->TestResetDevice())
        {
            // device could not be restored at this time
            return false;
        }

        // tell d3d that a new frame is about to start
        hr = this->d3d9Device->BeginScene();
        if (FAILED(hr))
        {
            n_printf("nD3D9Server: BeginScene() on d3d device failed!\n");
            return false;
        }

        this->inBeginScene = true;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Clear buffers.

    @param  bufferTypes     a combination of nBufferType flags
                            (COLOR | DEPTH | STENCIL)
    @param  red             the red value to write into the color buffer
    @param  green           the green value to write into the color buffer
    @param  blue            the blue value to write into the color buffer
    @param  alpha           the alpha value to write into the color buffer
    @param  z               the z value to write into the depth buffer
    @param  stencil         the stencil value to write into the stencil buffer
*/
void
nD3D9Server::Clear(int bufferTypes, float red, float green, float blue, float alpha, float z, int stencil)
{
    DWORD flags = 0;
    if (bufferTypes & COLOR)
    {
        flags |= D3DCLEAR_TARGET;
    }
    if (bufferTypes & DEPTH)
    {
        flags |= D3DCLEAR_ZBUFFER;
    }
    if (bufferTypes & STENCIL)
    {
        flags |= D3DCLEAR_STENCIL;
    }

    DWORD d3dColor = D3DCOLOR_COLORVALUE(red, green, blue, alpha);

    // no stencil buffer
    HRESULT hr = this->d3d9Device->Clear(0, NULL, flags, d3dColor, z, stencil);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
    Finish rendering the scene and present the backbuffer.
*/
void
nD3D9Server::EndScene()
{
    n_assert(this->inBeginScene);
    n_assert(this->d3d9Device);
    HRESULT hr = this->d3d9Device->EndScene();
    n_assert(SUCCEEDED(hr));

    nGfxServer2::EndScene();
}

//------------------------------------------------------------------------------
/**
    Present the scene.
*/
void
nD3D9Server::PresentScene()
{
    n_assert(!this->inBeginScene);
    n_assert(this->d3d9Device);
    HRESULT hr = this->d3d9Device->Present(0, 0, 0, 0);
    n_assert(SUCCEEDED(hr));

    nGfxServer2::PresentScene();
}

//------------------------------------------------------------------------------
/**
    Bind a texture to a texture stage. 

    @param  stage       the texture stage (0..7)
    @param  tex         pointer to a nD3D9Texture2 object or 0 to clear the
                        texture stage
*/
void
nD3D9Server::SetTexture(int stage, nTexture2* tex)
{
    n_assert((stage >= 0) && (stage < MAX_TEXTURESTAGES));

    if (this->GetTexture(stage) != tex)
    {
        HRESULT hr;
        n_assert(this->d3d9Device);
        
        IDirect3DBaseTexture9* d3dTex = 0;
        if (tex)
        {
            d3dTex = ((nD3D9Texture*)tex)->GetTexture();
            n_assert(d3dTex);
        }
        hr = this->d3d9Device->SetTexture(stage, d3dTex);
        n_assert(SUCCEEDED(hr));

        nGfxServer2::SetTexture(stage, tex);
    }
}

//------------------------------------------------------------------------------
/**
    Bind vertex buffer to a vertex stream.

    @param  stream      stream identifier (0..15)
    @param  mesh        pointer to a nD3D9Mesh2 object or 0 to clear the
                        current stream and index buffer
*/                  
void
nD3D9Server::SetMesh(int stream, nMesh2* mesh)
{
    n_assert((stream >= 0) && (stream < MAX_VERTEXSTREAMS));
    HRESULT hr;

    if (this->GetMesh(stream) != mesh)
    {
        n_assert(this->d3d9Device);

        IDirect3DVertexBuffer9*      d3dVBuf = 0;
        IDirect3DIndexBuffer9*       d3dIBuf = 0;
        IDirect3DVertexDeclaration9* d3dVDecl = 0;
        UINT stride = 0;
        if (mesh)
        {
            d3dVBuf  = ((nD3D9Mesh*)mesh)->GetVertexBuffer();
            d3dIBuf  = ((nD3D9Mesh*)mesh)->GetIndexBuffer();
            d3dVDecl = ((nD3D9Mesh*)mesh)->GetVertexDeclaration();
            n_assert(d3dVBuf);
            n_assert(d3dIBuf);
            n_assert(d3dVDecl);

            stride = mesh->GetVertexWidth() << 2;
            switch (mesh->GetPrimitiveType())
            {
                case POINTLIST:     this->d3dPrimType = D3DPT_POINTLIST; break;
                case LINELIST:      this->d3dPrimType = D3DPT_LINELIST; break;
                case LINESTRIP:     this->d3dPrimType = D3DPT_LINESTRIP; break;
                case TRIANGLELIST:  this->d3dPrimType = D3DPT_TRIANGLELIST; break;
                case TRIANGLESTRIP: this->d3dPrimType = D3DPT_TRIANGLESTRIP; break;
                case TRIANGLEFAN:   this->d3dPrimType = D3DPT_TRIANGLEFAN; break;
            }
        }

        // set the vertex stream source
        hr = this->d3d9Device->SetStreamSource(stream, d3dVBuf, 0, stride);
        n_assert(SUCCEEDED(hr));

        // set the vertex declaration
        hr = this->d3d9Device->SetVertexDeclaration(d3dVDecl);
        n_assert(SUCCEEDED(hr));

        // indices are provided by the mesh associated with stream 0!
        if (0 == stream)
        {
            hr = this->d3d9Device->SetIndices(d3dIBuf);
            n_assert(SUCCEEDED(hr));
        }

        nGfxServer2::SetMesh(stream, mesh);
    }
}

//------------------------------------------------------------------------------
/**
    Set the current shader object.
    FIXME: this method should find out, whether Draw() should have to reapply
    the shader at all!
*/
void
nD3D9Server::SetShader(nShader2* shader)
{
    if (this->GetShader() != shader)
    {
        nGfxServer2::SetShader(shader);
    }
}

//------------------------------------------------------------------------------
/**
    Set a new render target. This method must be called outside 
    BeginScene()/EndScene() with a pointer to a nTexture2 object
    which must have been created as render target. A 0 pointer
    restores the original back buffer as render target.

    @param  t   pointer to nTexture2 object or 0
*/
void
nD3D9Server::SetRenderTarget(nTexture2* t)
{
    n_assert(!this->inBeginScene);
    n_assert(this->d3d9Device);

    HRESULT hr;
    if (t)
    {
        nD3D9Texture* d3d9Tex = (nD3D9Texture*) t;
        IDirect3DSurface9* renderTarget = d3d9Tex->GetRenderTarget();
        IDirect3DSurface9* depthStencil = d3d9Tex->GetDepthStencil();
        if (renderTarget)
        {
            hr = this->d3d9Device->SetRenderTarget(0, renderTarget);
            n_assert(SUCCEEDED(hr));
        }
        if (depthStencil)
        {
            hr = this->d3d9Device->SetDepthStencilSurface(depthStencil);
            n_assert(SUCCEEDED(hr));
        }
    }
    else
    {
        // null pointer: restore back buffer and original depth stencil surface as render target
        hr = this->d3d9Device->SetRenderTarget(0, this->backBufferSurface);
        n_assert(SUCCEEDED(hr));
        hr = this->d3d9Device->SetDepthStencilSurface(this->depthStencilSurface);
        n_assert(SUCCEEDED(hr));
    }
}

//------------------------------------------------------------------------------
/**
    Draw the currently set mesh, texture and shader to the current 
    render target.

    FIXME: the multipass renderer should check if state actually needs to
    be applied again. This is not necessary if the effect only has 1 pass,
    and is the same effect with the same parameters as in the last 
    invocation of Draw().
*/
void
nD3D9Server::Draw()
{
    n_assert(this->d3d9Device && this->inBeginScene);
    n_assert(this->GetMesh(0));
    HRESULT hr;

    nD3D9Shader* shader = (nD3D9Shader*) this->GetShader();
    n_assert(shader);

    // get number of primitives
    int d3dNumPrimitives;
    switch (this->d3dPrimType)
    {
        case D3DPT_POINTLIST:
            d3dNumPrimitives = this->indexRangeNum;
            break;

        case D3DPT_LINELIST:
            d3dNumPrimitives = this->indexRangeNum / 2;
            break;

        case D3DPT_LINESTRIP:
            d3dNumPrimitives = this->indexRangeNum - 1;
            break;

        case D3DPT_TRIANGLELIST:
            d3dNumPrimitives = this->indexRangeNum / 3;
            break;

        case D3DPT_TRIANGLESTRIP:
            d3dNumPrimitives = this->indexRangeNum - 2;
            break;

        case D3DPT_TRIANGLEFAN:
            d3dNumPrimitives = this->indexRangeNum - 2;
            break;
    }

    // render current geometry, probably in multiple passes
    int numPasses = shader->Begin();
    int curPass;
    for (curPass = 0; curPass < numPasses; curPass++)
    {
        shader->Pass(curPass);

        hr = this->d3d9Device->DrawIndexedPrimitive(
            this->d3dPrimType, 
            0,
            this->vertexRangeFirst,
            this->vertexRangeNum,
            this->indexRangeFirst,
            d3dNumPrimitives);
        n_assert(SUCCEEDED(hr));
    }
    shader->End();
}


