//------------------------------------------------------------------------------
//  nglserver2_render.cc
//  2003      cubejk
//  2003-2004 Haron
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "opengl/nglmesh.h"
#include "opengl/ngltexture.h"

#include "gfx2/nshader2.h"
//#include "opengl/ncgfxshader.h"

//------------------------------------------------------------------------------
/**
    Update the gl projection matrix from the new camera settings.
*/
void
nGLServer2::SetCamera(nCamera2 &cam)
{
    nGfxServer2::SetCamera(cam);

    if (this->hDC)//??? this->context
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
nGLServer2::SetViewport(nViewport& vp)
{
    nGfxServer2::SetViewport(vp);

    if (this->hDC)
    {
        glViewport((GLint)vp.x, (GLint)vp.y, (GLsizei)vp.width, (GLsizei)vp.height);
        glDepthRange((GLclampd)vp.nearz, (GLclampd)vp.farz);
        //glScissor((GLint)vp.x, (GLint)vp.y, (GLsizei)vp.width, (GLsizei)vp.height);
        n_gltrace("nGLServer2::SetViewport().");
    }
}

//------------------------------------------------------------------------------
/**
    Reset lighting.
*/
void
nGLServer2::ClearLights()
{
/*    n_assert(this->d3d9Device);

    nGfxServer2::ClearLights();
    if (FFP == this->lightingType)
    {
        uint i;
        uint maxLights = this->devCaps.MaxActiveLights;
        if (maxLights > 8)
        {
            maxLights = 8;
        }
        for (i = 0; i < maxLights; i++)
        {
            HRESULT hr = this->d3d9Device->LightEnable(i, FALSE);
            n_assert(SUCCEEDED(hr));
        }
    }*/
}

//------------------------------------------------------------------------------
/**
    Reset lighting.
*/
void
nGLServer2::ClearLight(int index)
{
/*    nGfxServer2::ClearLight(index);

    uint maxLights = this->devCaps.MaxActiveLights;
    if(index < (int) maxLights)
    {
        HRESULT hr = this->d3d9Device->LightEnable(index, FALSE);
        n_assert(SUCCEEDED(hr));
    }*/
}

//------------------------------------------------------------------------------
/**
    Add a light to the light array. This will update the shared light
    effect state.
*/
int
nGLServer2::AddLight(const nLight& light)
{
    n_assert(light.GetRange() > 0.0f);
    int numLights = nGfxServer2::AddLight(light);
    
    if (Off == this->lightingType)
    {
        // no lighting, but set light transform
        this->SetTransform(nGfxServer2::Light, light.GetTransform());
    }
    else if (FFP == this->lightingType)
    {
        // TODO: Add OpenGL standart light
    }
    else if (Shader == this->lightingType)
    {
        // set light in shader pipeline
        this->SetTransform(nGfxServer2::Light, light.GetTransform());
        nShader2* shd = this->refSharedShader;

        if (light.GetType() == nLight::Directional)
        {
            // for directional lights, the light pos shader attributes
            // actually hold the light direction
            if (shd->IsParameterUsed(nShaderState::LightPos))
            {
                shd->SetVector3(nShaderState::LightPos, this->transform[Light].z_component());
            }
        }
        else
        {
            // point light position
            if (shd->IsParameterUsed(nShaderState::LightPos))
            {
                shd->SetVector3(nShaderState::LightPos, this->transform[Light].pos_component());
            }
        }
        if (shd->IsParameterUsed(nShaderState::LightType))
        {
            shd->SetInt(nShaderState::LightType, light.GetType());
        }
        if (shd->IsParameterUsed(nShaderState::LightRange))
        {
            shd->SetFloat(nShaderState::LightRange, light.GetRange());
        }
        if (shd->IsParameterUsed(nShaderState::LightDiffuse))
        {
            shd->SetVector4(nShaderState::LightDiffuse, light.GetDiffuse());
        }
        if (shd->IsParameterUsed(nShaderState::LightSpecular))
        {
            shd->SetVector4(nShaderState::LightSpecular, light.GetSpecular());
        }
        if (shd->IsParameterUsed(nShaderState::LightAmbient))
        {
            shd->SetVector4(nShaderState::LightAmbient, light.GetAmbient());
        }
        if (shd->IsParameterUsed(nShaderState::ShadowIndex))
        {
            shd->SetVector4(nShaderState::ShadowIndex, light.GetShadowLightMask());
        }
    }
    //if (1 == numLights)
    //{
    //    this->SetTransform(nGfxServer2::Light, light.GetTransform());

    //    nShader2* shd = (nShader2*)this->refSharedShader.get();
    //    shd->SetVector4(nShaderState::LightDiffuse, light.GetDiffuse());
    //    shd->SetVector4(nShaderState::LightSpecular, light.GetSpecular());
    //    shd->SetVector4(nShaderState::LightAmbient, light.GetAmbient());
    //    shd->SetVector4(nShaderState::LightDiffuse1, light.GetSecondaryDiffuse());
    //}
    return numLights;
/*
    if (FFP == this->lightingType)
    {
        n_assert(this->d3d9Device);
        HRESULT hr;

        // set ambient render state if this is the first light
        if (1 == numLights)
        {
            DWORD amb = D3DCOLOR_COLORVALUE(light.GetAmbient().x, light.GetAmbient().y, light.GetAmbient().z, light.GetAmbient().w);
            this->d3d9Device->SetRenderState(D3DRS_AMBIENT, amb);
        }

        // set light in fixed function pipeline
        D3DLIGHT9 d3dLight9;
        memset(&d3dLight9, 0, sizeof(d3dLight9));
        switch (light.GetType())
        {
            case nLight::Point:
                d3dLight9.Type = D3DLIGHT_POINT;
                break;

            case nLight::Directional:
                d3dLight9.Type = D3DLIGHT_DIRECTIONAL;
                break;

            case nLight::Spot:
                d3dLight9.Type = D3DLIGHT_SPOT;
                break;
        }
        d3dLight9.Diffuse.r    = light.GetDiffuse().x;
        d3dLight9.Diffuse.g    = light.GetDiffuse().y;
        d3dLight9.Diffuse.b    = light.GetDiffuse().z;
        d3dLight9.Diffuse.a    = light.GetDiffuse().w;
        d3dLight9.Specular.r   = light.GetSpecular().x;
        d3dLight9.Specular.g   = light.GetSpecular().y;
        d3dLight9.Specular.b   = light.GetSpecular().z;
        d3dLight9.Specular.a   = light.GetSpecular().w;
        d3dLight9.Ambient.r    = light.GetAmbient().x;
        d3dLight9.Ambient.g    = light.GetAmbient().y;
        d3dLight9.Ambient.b    = light.GetAmbient().z;
        d3dLight9.Ambient.a    = light.GetAmbient().w;
        d3dLight9.Position.x   = light.GetTransform().pos_component().x;
        d3dLight9.Position.y   = light.GetTransform().pos_component().y;
        d3dLight9.Position.z   = light.GetTransform().pos_component().z;

        const vector3& lightDir = -light.GetTransform().z_component();
        d3dLight9.Direction.x  = lightDir.x;
        d3dLight9.Direction.y  = lightDir.y;
        d3dLight9.Direction.z  = lightDir.z;
        d3dLight9.Range        = light.GetRange();
        d3dLight9.Falloff      = 1.0f;

        // set the attenuation values so that at the maximum range,
        // the light intensity is at 20%
        d3dLight9.Attenuation0 = 0.0f;
        d3dLight9.Attenuation1 = 5.0f / light.GetRange();
        d3dLight9.Attenuation2 = 0.0f;
        d3dLight9.Theta        = 0.0f;
        d3dLight9.Phi          = N_PI;
        hr = this->d3d9Device->SetLight(numLights - 1, &d3dLight9);
        n_assert(SUCCEEDED(hr));
        hr = this->d3d9Device->LightEnable(numLights - 1, TRUE);
        n_assert(SUCCEEDED(hr));
    }
*/
}

//------------------------------------------------------------------------------
/**
    Set a transformation matrix. This will update the shared state in
    the effect pool.
*/
void
nGLServer2::SetTransform(TransformType type, const matrix44& matrix)
{
    // let parent update the transform matrices
    nGfxServer2::SetTransform(type, matrix);

    // update the shared shader parameters
    if (this->refSharedShader.isvalid())
    {
        nShader2* shd = this->refSharedShader.get();
        bool mvpOnly = this->GetHint(MvpOnly);
        bool setMVP = false;
        bool setEyeDir = false;
        bool setEyePos = false;
        bool setModelEyePos = false;
        switch (type)
        {
            case Model:
                if (!mvpOnly)
                {
                    shd->SetMatrix(nShaderState::Model, this->transform[Model]);
                    shd->SetMatrix(nShaderState::InvModel, this->transform[InvModel]);
                    shd->SetMatrix(nShaderState::ModelView, this->transform[ModelView]);
                    shd->SetMatrix(nShaderState::InvModelView, this->transform[InvModelView]);
                }
                setModelEyePos = true;
                setMVP = true;
                break;

            case View:
                if (!mvpOnly)
                {
                    shd->SetMatrix(nShaderState::View, this->transform[View]);
                    shd->SetMatrix(nShaderState::InvView, this->transform[InvView]);
                    shd->SetMatrix(nShaderState::ModelView, this->transform[ModelView]);
                    shd->SetMatrix(nShaderState::InvModelView, this->transform[InvModelView]);
                    setEyePos = true;
                }
                setModelEyePos = true;
                setEyeDir = true;
                setMVP = true;
                break;

            case Projection:
                if (!mvpOnly)
                {
                    shd->SetMatrix(nShaderState::Projection, this->transform[Projection]);
                }
                setMVP = true;
                break;

            case Texture0:
                if (!mvpOnly)
                {
                    shd->SetMatrix(nShaderState::TextureTransform0, this->transform[Texture0]);
                }
                break;

            case Texture1:
                if (!mvpOnly)
                {
                    shd->SetMatrix(nShaderState::TextureTransform1, this->transform[Texture1]);
                }
                break;

            case Texture2:
                if (!mvpOnly)
                {
                    shd->SetMatrix(nShaderState::TextureTransform2, this->transform[Texture2]);
                }
                break;

            case Texture3:
                if (!mvpOnly)
                {
                    shd->SetMatrix(nShaderState::TextureTransform3, this->transform[Texture3]);
                }
                break;

            case Light:
                break;
        }
        if (setMVP)
        {
            shd->SetMatrix(nShaderState::ModelViewProjection, this->transform[ModelViewProjection]);
        }
        if (!mvpOnly && setEyePos)
        {
            shd->SetVector3(nShaderState::EyePos, this->transform[InvView].pos_component());
        }

        // model eye pos always needed in lighting formula
        if (setModelEyePos)
        {
            shd->SetVector3(nShaderState::ModelEyePos, this->transform[InvModelView].pos_component());
        }
        if (setEyeDir)
        {
            shd->SetVector3(nShaderState::EyeDir, -this->transform[View].z_component());
        }
        n_gltrace("nGLServer2::SetTransform().");
    }
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
nGLServer2::Clear(int bufferTypes, float red, float green, float blue, float alpha, float z, int stencil)
{
    n_assert(this->inBeginScene);
    int flags = 0;
    if (bufferTypes & ColorBuffer)
    {
        flags |= GL_COLOR_BUFFER_BIT;
        glClearColor(red, green, blue, alpha);
    }
    if (bufferTypes & DepthBuffer)
    {
        flags |= GL_DEPTH_BUFFER_BIT;
        glClearDepth(z);
    }
    if (bufferTypes & StencilBuffer)
    {
        flags |= GL_STENCIL_BUFFER_BIT;
        glClearStencil(stencil);
    }
    glClear(flags);
    n_gltrace("nGLServer2::Clear().");
}

//------------------------------------------------------------------------------
/**
    Bind vertex buffer and index buffer to vertex stream 0.

    @param  vbMesh  mesh which delivers the vertex buffer
    @param  ibMesh  mesh which delivers the index buffer
*/                  
void
nGLServer2::SetMesh(nMesh2* vbMesh, nMesh2* ibMesh)
{
    n_assert(this->hDC);

    // clear any set mesh array
    if (0 != this->GetMeshArray())
    {
        this->SetMeshArray(0);
    }

    if (0 != vbMesh)
    {    
        if ((this->refVbMesh.get_unsafe() != vbMesh) || (this->refIbMesh.get_unsafe() != ibMesh))
        {
            ((nGLMesh*)vbMesh)->SetVertices(this->vertexRangeFirst);
            
            if (0 != ibMesh)
            {
                ((nGLMesh*)ibMesh)->SetIndices(this->indexRangeFirst);
            }
            else
            {
                ((nGLMesh*)ibMesh)->UnsetIndices();
            }
        }
    }
    else
    {
        ((nGLMesh*)vbMesh)->UnsetVertices();
        ((nGLMesh*)ibMesh)->UnsetIndices();
    }

    nGfxServer2::SetMesh(vbMesh, ibMesh);
}

//------------------------------------------------------------------------------
/**
    Set a mesh array for multiple vertex streams. Must be a gl mesh array
    The mesh in the array at stream 0 must provide a index buffer!

    @param  meshArray   pointer to a gl mesh array object or 0 to clear the
                        current stream and index buffer
*/
void
nGLServer2::SetMeshArray(nMeshArray* meshArray)
{
    n_assert(this->hDC);

    // XXX: Needs implementing
    if (0 != meshArray)
    {
        if (0 != this->refVbMesh.get_unsafe())
        {
            this->SetMesh(0, 0);
        }
        
        if (this->GetMeshArray() != meshArray)
        {
            // clear old mesh array before settings new one
            this->SetMeshArray(0);
/*
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
*/
        }
    }
    else
    {
/*
        // clear vertex streams
        int i;
        for(i = 0; i < MaxVertexStreams; i++)
        {
            hr = this->d3d9Device->SetStreamSource(i, 0, 0, 0);
            n_dxtrace(hr, "SetStreamSource() on D3D device failed!");
        }
        
        // clear the vertex declaration
        // FIXME FLOH: Uncommented because this generates a D3D warning
        //hr = this->d3d9Device->SetVertexDeclaration(0);
        //n_dxtrace(hr, "SetVertexDeclaration() on D3D device failed!");

        // clear the indexbuffer            
        hr = this->d3d9Device->SetIndices(0);
        n_dxtrace(hr, "SetIndices() on D3D device failed!");
*/
    }

    nGfxServer2::SetMeshArray(meshArray);
}

//------------------------------------------------------------------------------
/**
    Set the current shader object.
*/
void
nGLServer2::SetShader(nShader2* shader)
{
    if (this->GetShader() != shader)
    {
        nGfxServer2::SetShader(shader);
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
nGLServer2::DrawIndexed(PrimitiveType primType)
{
    n_assert(this->hDC && this->inBeginScene);

    nShader2* shader = this->GetShader();
    n_assert(shader);

    // get primitive type and number of primitives
    GLenum glPrimType;
    int glNumPrimitives = this->GetGLPrimTypeAndNumIndexed(primType, glPrimType);

    // render current geometry, probably in multiple passes
    int numPasses = shader->Begin(false);
    int curPass;
    for (curPass = 0; curPass < numPasses; curPass++)
    {
        shader->BeginPass(curPass);
        glDrawElements(glPrimType,
                       this->indexRangeNum/*glNumPrimitives*/,
                       GL_UNSIGNED_SHORT,
                       ((nGLMesh*)this->refVbMesh.get_unsafe())->IndexOffset(0));
        n_gltrace("nGLServer2::DrawIndexed(): glDrawElements().");
        shader->EndPass();

        #ifdef __NEBULA_STATS__
        if (this->GetHint(CountStats))
        {
            this->statsNumDrawCalls++;
        }
        #endif
    }
    shader->End();                

    #ifdef __NEBULA_STATS__
    if (this->GetHint(CountStats))
    {
        // update num primitives rendered
        this->statsNumPrimitives += glNumPrimitives;
    }
    #endif
    n_gltrace("nGLServer2::DrawIndexed().");
}

//------------------------------------------------------------------------------
/**
    Draw the currently set mesh with non-indexed primitives.
*/
void
nGLServer2::Draw(PrimitiveType primType)
{
    n_assert(this->hDC && this->inBeginScene);

    //n_printf("nGLServer2::Draw()\n");
    nShader2* shader = this->GetShader();
    n_assert(shader);

    // get primitive type and number of primitives
    GLenum glPrimType;
    int glNumPrimitives = this->GetGLPrimTypeAndNum(primType, glPrimType);

    // render current geometry, probably in multiple passes
    int numPasses = shader->Begin(false);
    int curPass;
    //n_printf("numPasses=%d, vRenderFirst=%d\n", numPasses, this->vertexRangeFirst);
    for (curPass = 0; curPass < numPasses; curPass++)
    {
        shader->BeginPass(curPass);
        glDrawArrays(glPrimType, 0, glNumPrimitives); // 0 - ???
        n_gltrace("nGLServer2::Draw(): glDrawArrays().");
        shader->EndPass();

        #ifdef __NEBULA_STATS__
        if (this->GetHint(CountStats))
        {
            this->statsNumDrawCalls++;
        }
        #endif
    }
    shader->End();                

    #ifdef __NEBULA_STATS__
    if (this->GetHint(CountStats))
    {
        // update num primitives rendered
        this->statsNumPrimitives += glNumPrimitives;
    }
    #endif
    n_gltrace("nGLServer2::Draw().");
}

//------------------------------------------------------------------------------
/**
    Renders the currently set mesh without applying any shader state.
    You must call nShader2::Begin(), nShader2::Pass() and nShader2::End()
    yourself as needed.
*/
void
nGLServer2::DrawIndexedNS(PrimitiveType primType)
{
    n_assert(this->hDC && this->inBeginScene);

    n_printf("nGLServer2::DrawIndexedNS()\n");
    // get primitive type and number of primitives
    GLenum glPrimType;
    int glNumPrimitives = this->GetGLPrimTypeAndNumIndexed(primType, glPrimType);

    this->refShader->CommitChanges();

    glDrawElements(glPrimType,
                   glNumPrimitives,
                   GL_UNSIGNED_SHORT,
                   ((nGLMesh*)this->refVbMesh.get_unsafe())->IndexOffset(0));
    n_gltrace("nGLServer2::DrawIndexedNS(): glDrawElements().");
    
    #ifdef __NEBULA_STATS__
    if (this->GetHint(CountStats))
    {
        // update statistics
        this->statsNumDrawCalls++;
        this->statsNumPrimitives += glNumPrimitives;
    }
    #endif
    n_gltrace("nGLServer2::DrawIndexedNS().");
}

//------------------------------------------------------------------------------
/**
    Renders the currently set mesh without applying any shader state.
    You must call nShader2::Begin(), nShader2::Pass() and nShader2::End()
    yourself as needed.
*/
void
nGLServer2::DrawNS(PrimitiveType primType)
{
    n_assert(this->hDC && this->inBeginScene);

    n_printf("nGLServer2::DrawNS()\n");
    // get primitive type and number of primitives
    GLenum glPrimType;
    int glNumPrimitives = this->GetGLPrimTypeAndNum(primType, glPrimType);

    this->refShader->CommitChanges();

    glDrawArrays(glPrimType, 0, glNumPrimitives); // 0 - ???
    n_gltrace("nGLServer2::DrawNS(): glDrawArrays().");
    
    #ifdef __NEBULA_STATS__
    if (this->GetHint(CountStats))
    {
        // update statistics
        this->statsNumDrawCalls++;
        this->statsNumPrimitives += glNumPrimitives;
    }
    #endif
    n_gltrace("nGLServer2::DrawNS().");
}
