//------------------------------------------------------------------------------
//  nd3d9server_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9mesh.h"
#include "gfx2/nd3d9texture.h"
#include "gfx2/nd3d9shader.h"
#include "gfx2/nd3d9mesharray.h"
#include "kernel/ntimeserver.h"

//------------------------------------------------------------------------------
/**
    Add a light to the light array. This will update the shared light
    effect state.
*/
int
nD3D9Server::AddLight(const nLight& light)
{
    // FIXME: only handle the first light for now!
    int numLights = nGfxServer2::AddLight(light);
    if (1 == numLights)
    {
        this->SetTransform(nGfxServer2::Light, light.GetTransform());

        nD3D9Shader* shd = this->refSharedShader.get();
        shd->SetVector4(nShaderState::LightDiffuse, light.GetDiffuse());
        shd->SetVector4(nShaderState::LightSpecular, light.GetSpecular());
        shd->SetVector4(nShaderState::LightAmbient, light.GetAmbient());
    }
    return numLights;
}

//------------------------------------------------------------------------------
/**
    Update the d3d projection matrix from the new camera settings.
*/
void
nD3D9Server::SetCamera(nCamera2& cam)
{
    nGfxServer2::SetCamera(cam);

    if (this->d3d9Device)
    {
        // set projection matrices
        this->SetTransform(Projection, cam.GetProjection());
    }
}

//------------------------------------------------------------------------------
/**
    Update the device viewport.
*/
void
nD3D9Server::SetViewport(nViewport& vp)
{
    nGfxServer2::SetViewport(vp);

    if (this->d3d9Device)
    {
        static D3DVIEWPORT9 dvp;
        dvp.X = (DWORD)vp.x;
        dvp.Y = (DWORD)vp.y;
        dvp.Width = (DWORD)vp.width;
        dvp.Height = (DWORD)vp.height;
        dvp.MinZ = vp.nearz;
        dvp.MaxZ = vp.farz;
        this->d3d9Device->SetViewport(&dvp);
    }
}

//------------------------------------------------------------------------------
/**
    Set a transformation matrix. This will update the shared state in
    the effect pool.
*/
void
nD3D9Server::SetTransform(TransformType type, const matrix44& matrix)
{
    // let parent update the transform matrices
    nGfxServer2::SetTransform(type, matrix);

    // update the shared shader parameters
    if (this->refSharedShader.isvalid())
    {
        nD3D9Shader* shd = this->refSharedShader.get();
        bool setMVP = false;
        bool setEyePos = false;
        bool setModelLightProjection = false;
        bool setLightPos = false;
        switch (type)
        {
            case Model:
				{
				#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
				// 2004 SDK ffp - matrix bug workaround
				// WARNING - this will be wrong when you use vertex shaders and dx7 HW
				matrix44 model(this->transform[Model]);
				model.transpose();
				shd->SetMatrix(nShaderState::Model, model);
				
				matrix44 invModel(this->transform[InvModel]);
				invModel.transpose();
                shd->SetMatrix(nShaderState::InvModel, invModel);
                
				matrix44 modelView(this->transform[ModelView]);
				modelView.transpose();
				shd->SetMatrix(nShaderState::ModelView, modelView);
                
				matrix44 invModelView(this->transform[InvModelView]);
				invModelView.transpose();
				shd->SetMatrix(nShaderState::InvModelView, invModelView);
				#else
				shd->SetMatrix(nShaderState::Model, this->transform[Model]);
                shd->SetMatrix(nShaderState::InvModel, this->transform[InvModel]);
                shd->SetMatrix(nShaderState::ModelView, this->transform[ModelView]);
                shd->SetMatrix(nShaderState::InvModelView, this->transform[InvModelView]);
				#endif
                setMVP = true;
                setEyePos = true;
                setModelLightProjection = true;
				}
                break;
            
            case View:
				{
				#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
				// 2004 SDK ffp - matrix bug workaround
				// WARNING - this will be wrong when you use vertex shaders and dx7 HW
				matrix44 view(this->transform[View]);
				view.transpose();
				shd->SetMatrix(nShaderState::View, view);
				
				matrix44 invView(this->transform[InvView]);
				invView.transpose();
                shd->SetMatrix(nShaderState::InvView, invView);
				
				matrix44 modelView(this->transform[ModelView]);
				modelView.transpose();
				shd->SetMatrix(nShaderState::ModelView, modelView);
				
				matrix44 invModelView(this->transform[InvModelView]);
				invModelView.transpose();
                shd->SetMatrix(nShaderState::InvModelView, invModelView);
				#else
                shd->SetMatrix(nShaderState::View, this->transform[View]);
                shd->SetMatrix(nShaderState::InvView, this->transform[InvView]);
                shd->SetMatrix(nShaderState::ModelView, this->transform[ModelView]);
                shd->SetMatrix(nShaderState::InvModelView, this->transform[InvModelView]);
				#endif
                setMVP = true;
                setEyePos = true;
				}
                break;

            case Projection:
				{
				#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
				// 2004 SDK ffp - matrix bug workaround
				// WARNING - this will be wrong when you use vertex shaders and dx7 HW
				matrix44 projection(this->transform[Projection]);
				projection.transpose();
				shd->SetMatrix(nShaderState::Projection, projection);
				#else
                shd->SetMatrix(nShaderState::Projection, this->transform[Projection]);
				#endif
                setMVP = true;
                setModelLightProjection = true;
				}
                break;

            case Texture0:
				{
				#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
				// 2004 SDK ffp - matrix bug workaround
				// WARNING - this will be wrong when you use vertex shaders and dx7 HW
				matrix44 texture0(this->transform[Texture0]);
				texture0.transpose();
                shd->SetMatrix(nShaderState::TextureTransform0, texture0);
				#else
				shd->SetMatrix(nShaderState::TextureTransform0, this->transform[Texture0]);
				#endif
				}
                break;

            case Texture1:
				{
				#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
				// 2004 SDK ffp - matrix bug workaround
				// WARNING - this will be wrong when you use vertex shaders and dx7 HW
				matrix44 transform1(this->transform[Texture1]);
				transform1.transpose();
				shd->SetMatrix(nShaderState::TextureTransform1, transform1);
				#else
                shd->SetMatrix(nShaderState::TextureTransform1, this->transform[Texture1]);
				#endif
				}
                break;

            case Texture2:
				{
				#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
				// 2004 SDK ffp - matrix bug workaround
				// WARNING - this will be wrong when you use vertex shaders and dx7 HW
				matrix44 transform2(this->transform[Texture2]);
				transform2.transpose();
                shd->SetMatrix(nShaderState::TextureTransform2, transform2);
				#else
				shd->SetMatrix(nShaderState::TextureTransform2, this->transform[Texture2]);
				#endif
				}
                break;

            case Texture3:
				{
				#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
				// 2004 SDK ffp - matrix bug workaround
				// WARNING - this will be wrong when you use vertex shaders and dx7 HW
				matrix44 transform3(this->transform[Texture3]);
				transform3.transpose();
                shd->SetMatrix(nShaderState::TextureTransform3, transform3);
				#else
				shd->SetMatrix(nShaderState::TextureTransform3, this->transform[Texture3]);
				#endif
				}
                break;

            case Light:
                setModelLightProjection = true;
                setLightPos = true;
                break;
        }
        if (setMVP)
        {
            shd->SetMatrix(nShaderState::ModelViewProjection, this->transform[ModelViewProjection]);
        }
        if (setEyePos)
        {
            shd->SetVector3(nShaderState::ModelEyePos, this->transform[InvModelView].pos_component());
        }
        if (setModelLightProjection)
        {
            shd->SetMatrix(nShaderState::ModelLightProjection, this->transform[ModelLightProjection]);
        }
        if (setLightPos)
        {
            shd->SetVector3(nShaderState::ModelLightPos, this->transform[InvModelLight].pos_component());
            shd->SetVector3(nShaderState::LightPos, this->transform[Light].pos_component());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Updates shared shader parameters. Called once at frame start.
*/
void
nD3D9Server::UpdateSharedShaderParams()
{
    if (this->refSharedShader.isvalid())
    {
        nShader2* shd = this->refSharedShader;

        // update global time
        nTime time = this->kernelServer->GetTimeServer()->GetTime();
        shd->SetFloat(nShaderState::Time, float(time));

        // display resolution
        const nDisplayMode2& mode = this->GetDisplayMode();
        nFloat4 dispRes;
        dispRes.x = (float) mode.GetWidth();
        dispRes.y = (float) mode.GetHeight();
        dispRes.z = 0.0f;
        dispRes.w = 0.0f;
        shd->SetFloat4(nShaderState::DisplayResolution, dispRes);
    }
}

//------------------------------------------------------------------------------
/**
    Start rendering the scene.
*/
bool
nD3D9Server::BeginScene()
{
    n_assert(this->displayOpen);

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

        // update mouse cursor image if necessary
        this->UpdateCursor();

        // tell d3d that a new frame is about to start
        hr = this->d3d9Device->BeginScene();
        if (FAILED(hr))
        {
            n_printf("nD3D9Server: BeginScene() on d3d device failed!\n");
            return false;
        }

        // update shared shader parameters
        this->UpdateSharedShaderParams();

        this->inBeginScene = true;

        #ifdef __NEBULA_STATS__
        // reset statistic variables
        this->dbgQueryNumPrimitives->SetI(0);
        this->dbgQueryNumDrawCalls->SetI(0);
        this->statsNumRenderStateChanges = 0;
        this->statsNumTextureChanges = 0;
        #endif
        
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
    if (bufferTypes & ColorBuffer)
    {
        flags |= D3DCLEAR_TARGET;
    }
    if (bufferTypes & DepthBuffer)
    {
        flags |= D3DCLEAR_ZBUFFER;
    }
    if ((bufferTypes & StencilBuffer) && 
        ((this->presentParams.AutoDepthStencilFormat == D3DFMT_D24S8) ||
         (this->presentParams.AutoDepthStencilFormat == D3DFMT_D24X4S4)))
    {
        flags |= D3DCLEAR_STENCIL;
    }

    DWORD d3dColor = D3DCOLOR_COLORVALUE(red, green, blue, alpha);

    // no stencil buffer
    HRESULT hr = this->d3d9Device->Clear(0, NULL, flags, d3dColor, z, stencil);
    n_dxtrace(hr, "In nD3D9Server::Clear(): Clear() on D3D device failed!");
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
    n_dxtrace(hr, "EndScene() on D3D device failed!");

    #ifdef __NEBULA_STATS__
    // query statistics
    this->QueryStatistics();
    #endif
    
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
    if (FAILED(hr))
    {
        n_printf("nD3D9Server::PresentScene(): failed to present scene!\n");
    }
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
    n_assert((stage >= 0) && (stage < MaxTextureStages));

    if (this->GetTexture(stage) != tex)
    {
        HRESULT hr;
        n_assert(this->d3d9Device);
        
        if (0 == tex)
        {
            // clear the texture stage
            hr = this->d3d9Device->SetTexture(stage, 0);
            n_dxtrace(hr, "SetTexture(0) on D3D device failed!");
        }
        else
        {
            hr = this->d3d9Device->SetTexture(stage, ((nD3D9Texture*)tex)->GetBaseTexture());
            n_dxtrace(hr, "SetTexture() on D3D device failed!");
        }
        nGfxServer2::SetTexture(stage, tex);
    }
}

//------------------------------------------------------------------------------
/**
    Bind vertex buffer to vertex stream 0.
    The mesh must have a index and a vertex buffer!

    @param  mesh        pointer to a nD3D9Mesh2 object or 0 to clear the
                        current stream and index buffer
*/                  
void
nD3D9Server::SetMesh(nMesh2* mesh)
{
    HRESULT hr;
    n_assert(this->d3d9Device);

    if (0 != mesh)
    {
        if (0 != this->GetMeshArray())
        {
            this->SetMeshArray(0);
        }
    
        if (this->GetMesh() != mesh)
        {
            //clean old mesh before set new
            this->SetMesh(0);
            
            IDirect3DVertexBuffer9*      d3dVBuf = 0;
            IDirect3DIndexBuffer9*       d3dIBuf = 0;
            IDirect3DVertexDeclaration9* d3dVDecl = 0;
            UINT stride = 0;
            d3dVBuf  = ((nD3D9Mesh*)mesh)->GetVertexBuffer();
            d3dIBuf  = ((nD3D9Mesh*)mesh)->GetIndexBuffer();
            d3dVDecl = ((nD3D9Mesh*)mesh)->GetVertexDeclaration();
            n_assert2(d3dVBuf, "The mesh must have a vertex buffer!\n");
            n_assert2(d3dIBuf, "The mesh must have a index buffer!\n");
            n_assert(d3dVDecl);

            stride = mesh->GetVertexWidth() << 2;
            
            // set the vertex stream source
            hr = this->d3d9Device->SetStreamSource(0, d3dVBuf, 0, stride);
            n_dxtrace(hr, "SetStreamSource() on D3D device failed!");
            
            // set the vertex declaration
            hr = this->d3d9Device->SetVertexDeclaration(d3dVDecl);
            n_dxtrace(hr, "SetVertexDeclaration() on D3D device failed!");

            // indices are provided by the mesh associated with stream 0!
            hr = this->d3d9Device->SetIndices(d3dIBuf);
            n_dxtrace(hr, "SetIndices() on D3D device failed!");

            if ((nMesh2::NeedsVertexShader & mesh->GetUsage()) && (this->GetFeatureSet() < DX9))
            {
                this->d3d9Device->SetSoftwareVertexProcessing(TRUE);
            }
            else
            {
                this->d3d9Device->SetSoftwareVertexProcessing(FALSE);
            }
        }
    }
    else
    {
        //clear vertex streams
        int i;
        for(i = 0; i < MaxVertexStreams; i++)
        {
            hr = this->d3d9Device->SetStreamSource(i, 0, 0, 0);
            n_dxtrace(hr, "SetStreamSource() on D3D device failed!");
        }
        
        //clear the vertex declaration
        hr = this->d3d9Device->SetVertexDeclaration(0);
        n_dxtrace(hr, "SetVertexDeclaration() on D3D device failed!");

        //clear the indexbuffer            
        hr = this->d3d9Device->SetIndices(0);
        n_dxtrace(hr, "SetIndices() on D3D device failed!");
    }
    nGfxServer2::SetMesh(mesh);
}

//------------------------------------------------------------------------------
/**
    Set a mesh array for multiple vertex streams. Must be a nD3D9MeshArray
    The mesh in the array at stream 0 must provide a index buffer!

    @param  meshArray   pointer to a nD3D9MeshArray object or 0 to clear the
                        current stream and index buffer
*/
void
nD3D9Server::SetMeshArray(nMeshArray* meshArray)
{
    HRESULT hr;
    n_assert(this->d3d9Device);
    
    if (0 != meshArray)
    {
        if (0 != this->GetMesh())
        {
            this->SetMesh(0);
        }
        
        if (this->GetMeshArray() != meshArray)
        {
            //clean old mesh array before set new
            this->SetMeshArray(0);

            bool needSoftwareProcessing = false;

            // set the vertex stream source
            IDirect3DVertexBuffer9* d3dVBuf = 0;
            UINT stride = 0;
            int i;
            for (i = 0; i < MaxVertexStreams; i++)
            {
                nMesh2* mesh = meshArray->GetMeshAt(i);
                if (0 != mesh)
                {
                    if (mesh->GetNumVertices())
                    {
                        d3dVBuf = ((nD3D9Mesh*)mesh)->GetVertexBuffer();
                    }

                    if (0 != d3dVBuf)
                    {
                        stride = ((nD3D9Mesh*)mesh)->GetVertexWidth() << 2;
                        hr = this->d3d9Device->SetStreamSource(i, d3dVBuf, 0, stride);
                        n_dxtrace(hr, "SetStreamSource() on D3D device failed!");
                    }

                    if (0 != (mesh->GetUsage() & nMesh2::NeedsVertexShader))
                    {
                        needSoftwareProcessing = true;
                    }
                }
            }
            
            // set the vertex declaration
            IDirect3DVertexDeclaration9* d3dVDecl = ((nD3D9MeshArray*)meshArray)->GetVertexDeclaration();
            n_assert(d3dVDecl);
            hr = this->d3d9Device->SetVertexDeclaration(d3dVDecl);
            n_dxtrace(hr, "SetVertexDeclaration() on D3D device failed!");

            // indices are provided by the mesh associated with stream 0!
            IDirect3DIndexBuffer9* d3dIBuf = ((nD3D9MeshArray*)meshArray)->GetIndexBuffer();
            n_assert2(d3dIBuf, "The mesh at stream 0 must provide a valid IndexBuffer!\n");
            hr = this->d3d9Device->SetIndices(d3dIBuf);
            n_dxtrace(hr, "SetIndices() on D3D device failed!");
            
            if (this->GetFeatureSet() < DX9 && needSoftwareProcessing)
            {                
                this->d3d9Device->SetSoftwareVertexProcessing(true);
            }
            else
            {
                this->d3d9Device->SetSoftwareVertexProcessing(false);
            }
        }
    }
    else
    {
        //clear vertex streams
        int i;
        for(i = 0; i < MaxVertexStreams; i++)
        {
            hr = this->d3d9Device->SetStreamSource(i, 0, 0, 0);
            n_dxtrace(hr, "SetStreamSource() on D3D device failed!");
        }
        
        //clear the vertex declaration
        hr = this->d3d9Device->SetVertexDeclaration(0);
        n_dxtrace(hr, "SetVertexDeclaration() on D3D device failed!");

        //clear the indexbuffer            
        hr = this->d3d9Device->SetIndices(0);
        n_dxtrace(hr, "SetIndices() on D3D device failed!");
    }

    nGfxServer2::SetMeshArray(meshArray);
}

//------------------------------------------------------------------------------
/**
    Set the current shader object.
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
            n_dxtrace(hr, "SetRenderTarget() on D3D device failed!");
        }
        if (depthStencil)
        {
            hr = this->d3d9Device->SetDepthStencilSurface(depthStencil);
            n_dxtrace(hr, "SetDepthStencilSurface() on D3D device failed!");
        }
    }
    else
    {
        // null pointer: restore back buffer and original depth stencil surface as render target
        hr = this->d3d9Device->SetRenderTarget(0, this->backBufferSurface);
        n_dxtrace(hr, "SetRenderTarget() on D3D device failed!");
        hr = this->d3d9Device->SetDepthStencilSurface(this->depthStencilSurface);
        n_dxtrace(hr, "SetDepthStencilSurface() on D3D device failed!");
    }
}

//------------------------------------------------------------------------------
/**
    Draw the currently set mesh with indexed primitives, texture and shader to 
    the current render target.

    FIXME: the multipass renderer should check if state actually needs to
    be applied again. This is not necessary if the effect only has 1 pass,
    and is the same effect with the same parameters as in the last 
    invocation of Draw().
*/
void
nD3D9Server::DrawIndexed(PrimitiveType primType)
{
    n_assert(this->d3d9Device && this->inBeginScene);
    n_assert((0 != this->GetMesh()) || (0 != this->GetMeshArray()));
    HRESULT hr;

    nD3D9Shader* shader = (nD3D9Shader*) this->GetShader();
    n_assert(shader);

    // get primitive type and number of primitives
    D3DPRIMITIVETYPE d3dPrimType;
    int d3dNumPrimitives = this->GetD3DPrimTypeAndNumIndexed(primType, d3dPrimType);

    // render current geometry, probably in multiple passes
    int numPasses = shader->Begin(false);
    int curPass;
    for (curPass = 0; curPass < numPasses; curPass++)
    {
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
        shader->BeginPass(curPass);
#else
        shader->Pass(curPass);
#endif
        hr = this->d3d9Device->DrawIndexedPrimitive(
            d3dPrimType, 
            0,
            this->vertexRangeFirst,
            this->vertexRangeNum,
            this->indexRangeFirst,
            d3dNumPrimitives);
        n_dxtrace(hr, "DrawIndexedPrimitive() failed!");

        #ifdef __NEBULA_STATS__
        this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + 1);
        #endif
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
        shader->EndPass();
#endif
    }
    shader->End();

    #ifdef __NEBULA_STATS__
    // update num primitives rendered
    this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + d3dNumPrimitives);
    #endif
}

//------------------------------------------------------------------------------
/**
    Draw the currently set mesh with non-indexed primitives.
*/
void
nD3D9Server::Draw(PrimitiveType primType)
{
    n_assert(this->d3d9Device && this->inBeginScene);
    n_assert((0 != this->GetMesh()) || (0 != this->GetMeshArray()));
    HRESULT hr;

    nD3D9Shader* shader = (nD3D9Shader*) this->GetShader();
    n_assert(shader);

    // get primitive type and number of primitives
    D3DPRIMITIVETYPE d3dPrimType;
    int d3dNumPrimitives = this->GetD3DPrimTypeAndNum(primType, d3dPrimType);

    // render current geometry, probably in multiple passes
    int numPasses = shader->Begin(false);
    int curPass;
    for (curPass = 0; curPass < numPasses; curPass++)
    {
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
        shader->BeginPass(curPass);
#else
        shader->Pass(curPass);
#endif
        hr = this->d3d9Device->DrawPrimitive(d3dPrimType, this->vertexRangeFirst, d3dNumPrimitives);
        n_dxtrace(hr, "DrawPrimitive() failed!");

        #ifdef __NEBULA_STATS__
        this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + 1);
        #endif
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
        shader->EndPass();
#endif
    }
    shader->End();

    #ifdef __NEBULA_STATS__
    // update num primitives rendered
    this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + d3dNumPrimitives);
    #endif
}

//------------------------------------------------------------------------------
/**
    Renders the currently set mesh without applying any shader state.
    You must call nShader2::Begin(), nShader2::Pass() and nShader2::End()
    yourself as needed.
*/
void
nD3D9Server::DrawIndexedNS(PrimitiveType primType)
{
    n_assert(this->d3d9Device && this->inBeginScene);
    n_assert((0 != this->GetMesh()) || (0 != this->GetMeshArray()));
    
    if (this->refInstanceStream.isvalid())
    {
        // do instanced rendering
        this->DrawIndexedInstancedNS(primType);
    }
    else
    {
        // do single instance rendering
        HRESULT hr;

        // get primitive type and number of primitives
        D3DPRIMITIVETYPE d3dPrimType;
        int d3dNumPrimitives = this->GetD3DPrimTypeAndNumIndexed(primType, d3dPrimType);

        hr = this->d3d9Device->DrawIndexedPrimitive(
            d3dPrimType, 
            0,
            this->vertexRangeFirst,
            this->vertexRangeNum,
            this->indexRangeFirst,
            d3dNumPrimitives);
        n_dxtrace(hr, "DrawIndexedPrimitive() failed!");

        #ifdef __NEBULA_STATS__
        // update statistics
        this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + 1);
        this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + d3dNumPrimitives);
        #endif
    }

}

//------------------------------------------------------------------------------
/**
    Renders the currently set mesh without applying any shader state.
    You must call nShader2::Begin(), nShader2::Pass() and nShader2::End()
    yourself as needed.
*/
void
nD3D9Server::DrawNS(PrimitiveType primType)
{
    if (this->refInstanceStream.isvalid())
    {
        // do instanced rendering
        this->DrawInstancedNS(primType);
    }
    else
    {
        n_assert(this->d3d9Device && this->inBeginScene);
        n_assert((0 != this->GetMesh()) || (0 != this->GetMeshArray()));

        HRESULT hr;

        // get primitive type and number of primitives
        D3DPRIMITIVETYPE d3dPrimType;
        int d3dNumPrimitives = this->GetD3DPrimTypeAndNum(primType, d3dPrimType);

        hr = this->d3d9Device->DrawPrimitive(d3dPrimType, this->vertexRangeFirst, d3dNumPrimitives);
        n_dxtrace(hr, "DrawPrimitive() failed!");

        #ifdef __NEBULA_STATS__
        // update statistics
        this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + 1);
        this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + d3dNumPrimitives);
        #endif
    }
}

//------------------------------------------------------------------------------
/**
    Instancing version of DrawIndexedNS().
*/
void
nD3D9Server::DrawIndexedInstancedNS(PrimitiveType primType)
{
    n_assert(this->d3d9Device && this->inBeginScene);
    n_assert((0 != this->GetMesh()) || (0 != this->GetMeshArray()));
    n_assert(this->refInstanceStream.isvalid());

    HRESULT hr;

    // get primitive type and number of primitives
    D3DPRIMITIVETYPE d3dPrimType;
    int d3dNumPrimitives = this->GetD3DPrimTypeAndNumIndexed(primType, d3dPrimType);

    nShader2* curShader = this->GetShader();
    nInstanceStream* instStream = this->GetInstanceStream();
    const nInstanceStream::Declaration& decl = instStream->GetDeclaration();
    const int numInstances = instStream->GetCurrentSize();
    const int numComps = decl.Size();

    instStream->Lock(nInstanceStream::Read);
    int curInstIndex;
    int numDrawCalls = 0;
    int numPrimitives = 0;
    for (curInstIndex = 0; curInstIndex < numInstances; curInstIndex++)
    {
        // update shader
        int curCompIndex;
        for (curCompIndex = 0; curCompIndex < numComps; curCompIndex++)
        {
            const nInstanceStream::Component& comp = decl[curCompIndex];
            nShaderState::Param param = comp.GetParam();
            switch (comp.GetType())
            {
                case nShaderState::Float:
                    curShader->SetFloat(param, instStream->ReadFloat());
                    break;

                case nShaderState::Float4:
                    curShader->SetFloat4(param, instStream->ReadFloat4());
                    break;

                case nShaderState::Matrix44:
                    // FIXME???
                    // if modelview matrix, compute dependent matrices?
                    curShader->SetMatrix(param, instStream->ReadMatrix44());
                    break;
            }
        }

        // invoke DrawPrimitive()
	    hr = this->d3d9Device->DrawIndexedPrimitive(d3dPrimType, 0, this->vertexRangeFirst, this->vertexRangeNum, this->indexRangeFirst, d3dNumPrimitives);
        n_dxtrace(hr, "DrawIndexedPrimitive() failed!");
    }
    instStream->Unlock();

    #ifdef __NEBULA_STATS__
    // update statistics
    this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + numInstances);
    this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + d3dNumPrimitives * numInstances);
    #endif
}

//------------------------------------------------------------------------------
/**
    Instancing version of DrawNS().
*/
void
nD3D9Server::DrawInstancedNS(PrimitiveType primType)
{
    n_assert(this->d3d9Device && this->inBeginScene);
    n_assert((0 != this->GetMesh()) || (0 != this->GetMeshArray()));
    n_assert(this->refInstanceStream.isvalid());

    HRESULT hr;

    // get primitive type and number of primitives
    D3DPRIMITIVETYPE d3dPrimType;
    int d3dNumPrimitives = this->GetD3DPrimTypeAndNum(primType, d3dPrimType);

    nShader2* curShader = this->GetShader();
    nInstanceStream* instStream = this->GetInstanceStream();
    const nInstanceStream::Declaration& decl = instStream->GetDeclaration();
    const int numInstances = instStream->GetCurrentSize();
    const int numComps = decl.Size();

    instStream->Lock(nInstanceStream::Read);
    int curInstIndex;
    int numDrawCalls = 0;
    int numPrimitives = 0;
    for (curInstIndex = 0; curInstIndex < numInstances; curInstIndex++)
    {
        // update shader
        int curCompIndex;
        for (curCompIndex = 0; curCompIndex < numComps; curCompIndex++)
        {
            const nInstanceStream::Component& comp = decl[curCompIndex];
            nShaderState::Param param = comp.GetParam();
            switch (comp.GetType())
            {
                case nShaderState::Float:
                    curShader->SetFloat(param, instStream->ReadFloat());
                    break;

                case nShaderState::Float4:
                    curShader->SetFloat4(param, instStream->ReadFloat4());
                    break;

                case nShaderState::Matrix44:
                    // FIXME???
                    // if modelview matrix, compute dependent matrices?
                    curShader->SetMatrix(param, instStream->ReadMatrix44());
                    break;
            }
        }

        // invoke DrawPrimitive()
        hr = this->d3d9Device->DrawPrimitive(d3dPrimType, this->vertexRangeFirst, d3dNumPrimitives);
        n_dxtrace(hr, "DrawPrimitive() failed");
    }
    instStream->Unlock();

    #ifdef __NEBULA_STATS__
    // update statistics
    this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + numInstances);
    this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + d3dNumPrimitives * numInstances);
    #endif
}

