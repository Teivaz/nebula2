//------------------------------------------------------------------------------
//  nglserver2_render.cc
//  2003-cubejk
//  2003-2004 Haron
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "opengl/nglmesh.h"
#include "opengl/ngltexture.h"
#include "opengl/ncgfxshader.h"

//------------------------------------------------------------------------------
/**
    Add a light to the light array. This will update the shared light
    effect state.
*/
int
nGLServer2::AddLight(const nLight& light)
{
    // FIXME: only handle the first light for now!
    int numLights = nGfxServer2::AddLight(light);
    if (1 == numLights)
    {
        this->SetTransform(nGfxServer2::Light, light.GetTransform());

        nCgFXShader* shd = this->refSharedShader.get();
        shd->SetVector4(nShaderState::LightDiffuse, light.GetDiffuse());
        shd->SetVector4(nShaderState::LightSpecular, light.GetSpecular());
        shd->SetVector4(nShaderState::LightAmbient, light.GetAmbient());
        shd->SetVector4(nShaderState::LightDiffuse1, light.GetSecondaryDiffuse());
    }
    return numLights;
}

//------------------------------------------------------------------------------
/**
    Update the gl projection matrix from the new camera settings.
*/
void
nGLServer2::SetCamera(nCamera2 &cam)
{

    nGfxServer2::SetCamera(cam);
/*
    n_printf("Camera setup.\n");
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(n_deg2rad(cam.GetAngleOfView()),
                   cam.GetAspectRatio(),
                   cam.GetNearPlane(),
                   cam.GetFarPlane());
    //matrix44 proj;
    //glGetFloatv(GL_PROJECTION_MATRIX,(float *)&proj);
    glPopMatrix();
*/
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
    }
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
        nCgFXShader* shd = this->refSharedShader.get();
        switch (type)
        {
            case Model:
                shd->SetMatrix(nShaderState::Model, this->transform[Model]);
                shd->SetMatrix(nShaderState::InvModelView, this->transform[InvModel]);
                shd->SetMatrix(nShaderState::ModelView, this->transform[ModelView]);
                shd->SetMatrix(nShaderState::InvModelView, this->transform[InvModelView]);
                break;
            
            case View:
                shd->SetMatrix(nShaderState::View, this->transform[View]);
                shd->SetMatrix(nShaderState::InvView, this->transform[InvView]);
                shd->SetMatrix(nShaderState::ModelView, this->transform[ModelView]);
                shd->SetMatrix(nShaderState::InvModelView, this->transform[InvModelView]);
                break;

            case Projection:
                n_printf("SetTransform(Projection)\n");
                shd->SetMatrix(nShaderState::Projection, this->transform[Projection]);
                break;

            case Texture0:
                shd->SetMatrix(nShaderState::TextureTransform0, this->transform[Texture0]);
                break;

            case Texture1:
                shd->SetMatrix(nShaderState::TextureTransform1, this->transform[Texture1]);
                break;

            case Texture2:
                shd->SetMatrix(nShaderState::TextureTransform2, this->transform[Texture2]);
                break;

            case Texture3:
                shd->SetMatrix(nShaderState::TextureTransform3, this->transform[Texture3]);
                break;
        }
        shd->SetMatrix(nShaderState::ModelViewProjection, this->transform[ModelViewProjection]);
        shd->SetVector3(nShaderState::ModelEyePos, this->transform[InvModelView].pos_component());
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
}

//------------------------------------------------------------------------------
/**
    Bind a texture to a texture stage. 

    @param  stage       the texture stage (0..7)
    @param  tex         pointer to a nGLTexture object or 0 to clear the
                        texture stage
*/
void
nGLServer2::SetTexture(int stage, nTexture2* tex)
{
    n_assert((stage >= 0) && (stage < MaxTextureStages));

    if (this->GetTexture(stage) != tex)
    {
        n_assert(this->hDC);
        //FIXME: maybe there should be glBindTexture?

        nGfxServer2::SetTexture(stage, tex);
    }
}

//------------------------------------------------------------------------------
/**
    Bind vertex buffer to a vertex stream.

    @param  stream      stream identifier (0..15)
    @param  mesh        pointer to a nGLMesh object or 0 to clear the
                        current stream and index buffer
*/                  
void
nGLServer2::SetMesh(nMesh2* mesh)
{
    //if ((this->GetMesh() != mesh) || (SingleMesh != this->meshSource))
    {
        //FIXME: maybe there should be some mesh initialization

        nGfxServer2::SetMesh(mesh);
    }
    //else if (0 == mesh)
    {
    //    this->meshSource = NoSource; // for the case that it was MeshArray previously
    }
}

//------------------------------------------------------------------------------
/**
    Set a mesh array for multiple vertex streams. Must be a nD3D9MeshArray
    The mesh in the array at stream 0 must provide a index buffer!

    @param  meshArray   pointer to a nD3D9MeshArray object or 0 to clear the
                        current stream and index buffer
*/
void
nGLServer2::SetMeshArray(nMeshArray* meshArray)
{
    // XXX: Needs implementing
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
    n_assert(this->GetMesh());

    //n_assert(!this->getGLErrors("nGLServer2::DrawIndexed - test"));
    //n_printf("nGLServer2::DrawIndexed()\n");
    nCgFXShader* shader = (nCgFXShader*) this->GetShader();
    n_assert(shader);

    // get primitive type and number of primitives
    GLenum glPrimType;
    int glNumPrimitives = this->GetGLPrimTypeAndNumIndexed(primType, glPrimType);

    //n_printf("NumIndices = %d, vRange1st = %d, iRange1st = %d\n",
    //    this->indexRangeNum, this->vertexRangeFirst, this->indexRangeFirst);

    nGLMesh* mesh;
    if ( (mesh = (nGLMesh*) this->GetMesh()) != 0)
    {
        if (mesh->GetNumIndices() > 0 &&
            mesh->BeginRender(this->vertexRangeFirst, true, this->indexRangeFirst))
        {
            // render current geometry, probably in multiple passes
            int numPasses = shader->Begin(false);
            int curPass;
            for (curPass = 0; curPass < numPasses; curPass++)
            {
                shader->BeginPass(curPass);
                glDrawElements(glPrimType, this->indexRangeNum/*glNumPrimitives*/, GL_UNSIGNED_SHORT, mesh->indexOffset(0));
                shader->EndPass();

                #ifdef __NEBULA_STATS__
                this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + 1);
                #endif
            }
            shader->End();                
            mesh->EndRender(true);
        }
    }

    #ifdef __NEBULA_STATS__
    // update num primitives rendered
    this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + glNumPrimitives);
    #endif
}

//------------------------------------------------------------------------------
/**
    Draw the currently set mesh with non-indexed primitives.
*/
void
nGLServer2::Draw(PrimitiveType primType)
{
    n_assert(this->hDC && this->inBeginScene);
    n_assert(this->GetMesh());

    n_printf("nGLServer2::Draw()\n");
    nCgFXShader* shader = (nCgFXShader*) this->GetShader();
    n_assert(shader);

    // get primitive type and number of primitives
    GLenum glPrimType;
    int glNumPrimitives = this->GetGLPrimTypeAndNum(primType, glPrimType);

    nGLMesh* mesh;
    if ( (mesh = (nGLMesh*) this->GetMesh()) != 0)
    {
        n_printf("shader: %s, numIndices=%d\n", mesh->GetFilename().Get(), mesh->GetNumIndices());
        if (mesh->GetNumIndices() > 0 &&
            mesh->BeginRender(this->vertexRangeFirst))
        {
            // render current geometry, probably in multiple passes
            int numPasses = shader->Begin(false);
            int curPass;
            //n_printf("numPasses=%d, vRenderFirst=%d\n", numPasses, this->vertexRangeFirst);
            for (curPass = 0; curPass < numPasses; curPass++)
            {
                shader->BeginPass(curPass);
                glDrawArrays(glPrimType, 0, glNumPrimitives); // 0 - ???
                shader->EndPass();

                #ifdef __NEBULA_STATS__
                this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + 1);
                #endif
            }
            shader->End();                
            mesh->EndRender();
        }
    }

    #ifdef __NEBULA_STATS__
    // update num primitives rendered
    this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + glNumPrimitives);
    #endif
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
    n_assert(this->GetMesh());

    n_printf("nGLServer2::DrawIndexedNS()\n");
    // get primitive type and number of primitives
    GLenum glPrimType;
    int glNumPrimitives = this->GetGLPrimTypeAndNumIndexed(primType, glPrimType);

    nGLMesh* mesh;
    if ( (mesh = (nGLMesh*) this->GetMesh()) != 0)
    {
        if (mesh->GetNumIndices() > 0 &&
            mesh->BeginRender(this->vertexRangeFirst, true, this->indexRangeFirst))

        {
            glDrawElements(glPrimType, glNumPrimitives, GL_UNSIGNED_SHORT, mesh->indexOffset(0));
            
            #ifdef __NEBULA_STATS__
            this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + 1);
            #endif

            mesh->EndRender(true);
        }
    }

    #ifdef __NEBULA_STATS__
    // update statistics
    this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + glNumPrimitives);
    #endif
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
    n_assert(this->GetMesh());

    n_printf("nGLServer2::DrawNS()\n");
    // get primitive type and number of primitives
    GLenum glPrimType;
    int glNumPrimitives = this->GetGLPrimTypeAndNum(primType, glPrimType);

    nGLMesh* mesh;
    if ( (mesh = (nGLMesh*) this->GetMesh()) != 0)
    {
        if (mesh->GetNumIndices() > 0 &&
            mesh->BeginRender(this->vertexRangeFirst))

        {
            glDrawArrays(glPrimType, 0, glNumPrimitives); // 0 - ???
            
            #ifdef __NEBULA_STATS__
            this->dbgQueryNumDrawCalls->SetI(this->dbgQueryNumDrawCalls->GetI() + 1);
            #endif

            mesh->EndRender();
        }
    }

    #ifdef __NEBULA_STATS__
    // update statistics
    this->dbgQueryNumPrimitives->SetI(this->dbgQueryNumPrimitives->GetI() + glNumPrimitives);
    #endif
}
