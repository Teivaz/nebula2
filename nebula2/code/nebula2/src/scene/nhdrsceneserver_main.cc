//------------------------------------------------------------------------------
//  nhdrsceneserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nhdrsceneserver.h"
#include "scene/nscenenode.h"
#include "scene/nmaterialnode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ntexture2.h"

nNebulaScriptClass(nHdrSceneServer, "nsceneserver");

//------------------------------------------------------------------------------
/**
*/
nHdrSceneServer::nHdrSceneServer() :
    hueColor(1.0f, 1.0f, 1.0f, 1.0f),
    postBufferWidth(0),
    postBufferHeight(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nHdrSceneServer::~nHdrSceneServer()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
nHdrSceneServer::AreResourcesValid()
{
    bool valid = true;
    valid &= this->hdrSceneBuffer.isvalid();
    valid &= this->hdrPostBuffer0.isvalid();
    valid &= this->hdrPostBuffer1.isvalid();
    int i;
    for (i = 0; i < NumShaders; i++)
    {
        valid &= this->shaders[i].isvalid();
    }
    if (this->quadMesh.isvalid())
    {
        valid &= this->quadMesh->IsValid();
        valid &= (nMesh2::NeededNow != this->quadMesh->GetRefillBuffersMode());
    }
    else
    {
        valid = false;
    }
    return valid;
}

//------------------------------------------------------------------------------
/**
*/
void
nHdrSceneServer::UnloadResources()
{
    if (this->hdrSceneBuffer.isvalid())
    {
        this->hdrSceneBuffer->Release();
        n_assert(!this->hdrSceneBuffer.isvalid());
    }
    if (this->hdrPostBuffer0.isvalid())
    {
        this->hdrPostBuffer0->Release();
        n_assert(!this->hdrPostBuffer0.isvalid());
    }
    if (this->hdrPostBuffer1.isvalid())
    {
        this->hdrPostBuffer1->Release();
        n_assert(!this->hdrPostBuffer1.isvalid());
    }
    int i;
    for (i = 0; i < NumShaders; i++)
    {
        if (this->shaders[i].isvalid())
        {
            this->shaders[i]->Release();
            n_assert(!this->shaders[i].isvalid());
        }
    }
    if (this->quadMesh.isvalid())
    {
        this->quadMesh->Release();
        n_assert(!this->quadMesh.isvalid());
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nHdrSceneServer::LoadResources()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    n_assert(gfxServer->GetFeatureSet() >= nGfxServer2::DX9FLT);
    const nDisplayMode2& mode = gfxServer->GetDisplayMode();

    // create full size offscreen buffer for hdr scene
    if (!this->hdrSceneBuffer.isvalid())
    {
        this->hdrSceneBuffer = gfxServer->NewRenderTarget("hdrSceneBuffer", 
                                                          mode.GetWidth(), 
                                                          mode.GetHeight(), 
                                                          nTexture2::A16B16G16R16F, 
                                                          nTexture2::RenderTargetColor);
        n_assert(this->hdrSceneBuffer.isvalid());
    }

    // create quarter size post effect buffer
    this->postBufferWidth  = mode.GetWidth() / 4;
    this->postBufferHeight = mode.GetHeight() / 4;
    if (!this->hdrPostBuffer0.isvalid())
    {
        this->hdrPostBuffer0 = gfxServer->NewRenderTarget("hdrPostBuffer0", 
                                                          postBufferWidth,
                                                          postBufferHeight,
                                                          nTexture2::A16B16G16R16F, 
                                                          nTexture2::RenderTargetColor);
        n_assert(this->hdrPostBuffer0.isvalid());
    }
    if (!this->hdrPostBuffer1.isvalid())
    {
        this->hdrPostBuffer1 = gfxServer->NewRenderTarget("hdrPostBuffer1",
                                                          postBufferWidth,
                                                          postBufferHeight,
                                                          nTexture2::A16B16G16R16F, 
                                                          nTexture2::RenderTargetColor);
        n_assert(this->hdrPostBuffer0.isvalid());
    }

    // load the shaders
    int i;
    for (i = 0; i < NumShaders; i++)
    {
        nShader2* shader = 0;
        const char* name = 0;
        if (!this->shaders[i].isvalid())
        {
            switch (i)
            {
                case ExtractShader: name = "shaders:hdrextract.fx"; break;
                case FilterShader:  name = "shaders:hdrfilter.fx"; break;
                case ComposeShader: name = "shaders:hdrcompose.fx"; break;
            }
            n_assert(name);
            shader = gfxServer->NewShader(name);
            shader->SetFilename(name);
            this->shaders[i] = shader;
        }
        else
        {
            shader = this->shaders[i].get();
        }
        if (!shader->IsValid())
        {
            bool shaderLoaded = shader->Load();
            n_assert(shaderLoaded);
        }
    }

    // create a fullscreen quad mesh
    nMesh2* mesh = 0;
    if (!this->quadMesh.isvalid())
    {
        mesh = gfxServer->NewMesh(0);
        mesh->SetUsage(nMesh2::WriteOnce);
        mesh->SetRefillBuffersMode(nMesh2::Enabled);
        mesh->SetNumVertices(4);
        mesh->SetNumIndices(6);
        mesh->SetVertexComponents(nMesh2::Coord | nMesh2::Uv0);
        this->quadMesh = mesh;
    }
    else
    {
        mesh = this->quadMesh.get();
    }

    // fill mesh
    if (!mesh->IsValid() || (nMesh2::NeededNow == mesh->GetRefillBuffersMode()))
    {
        bool success = mesh->Load();
        n_assert(success);

        // fill rectangle vertices
        float* vPtr = mesh->LockVertices();
        n_assert(vPtr);
        *vPtr++ = -1.0f; *vPtr++ =  1.0f; *vPtr++ = 0.0f; *vPtr++ = 0.0f; *vPtr++ = 0.0f;
        *vPtr++ = -1.0f; *vPtr++ = -1.0f; *vPtr++ = 0.0f; *vPtr++ = 0.0f; *vPtr++ = 1.0f;
        *vPtr++ = 1.0f;  *vPtr++ =  1.0f; *vPtr++ = 0.0f; *vPtr++ = 1.0f; *vPtr++ = 0.0f;
        *vPtr++ = 1.0f;  *vPtr++ = -1.0f; *vPtr++ = 0.0f; *vPtr++ = 1.0f; *vPtr++ = 1.0f;
        mesh->UnlockVertices();

        // fill rectangle indices
        ushort* iPtr = mesh->LockIndices();
        *iPtr++ = 0; *iPtr++ = 1; *iPtr++ = 2;
        *iPtr++ = 1; *iPtr++ = 3; *iPtr++ = 2;
        mesh->UnlockIndices();

        mesh->SetRefillBuffersMode(nMesh2::Enabled);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Render the shape objects in the scene, using the shader defined by
    the fourcc code. If no such shader exists, no rendering is done.
*/
void
nHdrSceneServer::RenderShapes(uint shaderFourCC)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // for each bucket...
    int bucketIndex;
    int numBuckets = this->shapeBucket.Size();
    for (bucketIndex = 0; bucketIndex < numBuckets; bucketIndex++)
    {
        const nArray<ushort>& shapeArray = this->shapeBucket[bucketIndex];
        int numShapes = shapeArray.Size();

        // activate bucket shader
        if (numShapes > 0)
        {
            nShader2* curShader = this->GetBucketShader(bucketIndex, shaderFourCC);
            if (curShader)
            {
                // for each shader pass...
                int numPasses = curShader->Begin(false);
                int curPass;
                for (curPass = 0; curPass < numPasses; curPass++)
                {
                    curShader->BeginPass(curPass);

                    // for each shape...
                    int shapeIndex;
                    nMaterialNode* prevShapeNode = 0;
                    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                    {
                        Group& group = this->groupArray[shapeArray[shapeIndex]];
                        nMaterialNode* shapeNode = (nMaterialNode*) group.sceneNode;
                        if (shapeNode != prevShapeNode)
                        {
                            shapeNode->ApplyShader(shaderFourCC, this);
                            shapeNode->ApplyGeometry(this);
                        }
                        prevShapeNode = shapeNode;

                        gfxServer->SetTransform(nGfxServer2::Model, group.modelTransform);
                        shapeNode->RenderShader(shaderFourCC, this, group.renderContext);
                        shapeNode->RenderGeometry(this, group.renderContext);
                    }
                    curShader->EndPass();
                }
                curShader->End();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render all light/shape interactions matching a given fourcc shader code.
*/
void
nHdrSceneServer::RenderLightShapes(uint shaderFourCC)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // for each bucket...
    int bucketIndex;
    int numBuckets = this->shapeBucket.Size();
    for (bucketIndex = 0; bucketIndex < numBuckets; bucketIndex++)
    {
        const nArray<ushort>& shapeArray = this->shapeBucket[bucketIndex];
        int numShapes = shapeArray.Size();

        // activate bucket shader
        if (numShapes > 0)
        {
            nShader2* curShader = this->GetBucketShader(bucketIndex, shaderFourCC);
            if (curShader)
            {
                gfxServer->SetShader(curShader);

                // apply pre-instancing light parameters (FIXME: normally one pass per light source
                int numLights = this->lightArray.Size();
                if (numLights > 0)
                {
                    Group& lightGroup = this->groupArray[this->lightArray[0]];
                    lightGroup.sceneNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);
                }

                // for each shader pass...
                int numPasses = curShader->Begin(false);
                int curPass;
                for (curPass = 0; curPass < numPasses; curPass++)
                {
                    curShader->BeginPass(curPass);

                    // for each shape in bucket...
                    int shapeIndex;
                    nMaterialNode* prevShapeNode = 0;
                    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                    {
                        Group& shapeGroup = this->groupArray[shapeArray[shapeIndex]];
                        nMaterialNode* shapeNode = (nMaterialNode*) shapeGroup.sceneNode;
                        if (shapeNode != prevShapeNode)
                        {
                            shapeNode->ApplyShader(shaderFourCC, this);
                            shapeNode->ApplyGeometry(this);
                        }
                        prevShapeNode = shapeNode;

                        // set the modelview matrix for the shape
                        gfxServer->SetTransform(nGfxServer2::Model, shapeGroup.modelTransform);

                        // set per-instance shader parameters
                        shapeNode->RenderShader(shaderFourCC, this, shapeGroup.renderContext);

                        // render!
                        shapeNode->RenderGeometry(this, shapeGroup.renderContext);
                    }
                    curShader->EndPass();
                }
                curShader->End();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Prepare rendering the scene.
*/
bool
nHdrSceneServer::BeginScene(const matrix44& invView)
{
    if (nSceneServer::BeginScene(invView))
    {
        // make sure all required resources are ok
        if (!this->AreResourcesValid())
        {
            bool success = this->LoadResources();
            n_assert(success);
        }
        return true;
    }
    return false;
}         

//------------------------------------------------------------------------------
/**
    This creates the post effect by rendering overbright areas of the
    original scene into a smaller buffer, and bluring the buffer.
*/
void
nHdrSceneServer::RenderPostEffect()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    const nDisplayMode2& mode = gfxServer->GetDisplayMode();

    // render original overbright areas to post buffer 0
    this->UpdateSampleOffsetsGaussBlur5x5(mode.GetWidth() / 4, mode.GetHeight() / 4, 0.2f);
    gfxServer->SetRenderTarget(this->hdrPostBuffer0.get());
    if (gfxServer->BeginScene())
    {
        nShader2* shader = this->shaders[FilterShader].get();
        if (shader->IsParameterUsed(nShaderState::DiffMap0))
        {
            shader->SetTexture(nShaderState::DiffMap0, this->hdrSceneBuffer.get());
        }
        if (shader->IsParameterUsed(nShaderState::SampleOffsets))
        {
            shader->SetVector4Array(nShaderState::SampleOffsets, this->sampleOffsets, NumSamples);
        }
        if (shader->IsParameterUsed(nShaderState::SampleWeights))
        {
            shader->SetVector4Array(nShaderState::SampleWeights, this->sampleWeights, NumSamples);
        }
        gfxServer->SetShader(shader);
        gfxServer->SetMesh(this->quadMesh.get());
        gfxServer->SetVertexRange(0, 4);
        gfxServer->SetIndexRange(0, 6);
        gfxServer->DrawIndexed(nGfxServer2::TriangleList);
        gfxServer->EndScene();
    }

    // horizontal stretch blit 
    this->UpdateSampleOffsetsBloom(true, this->postBufferWidth / 2, 3.0f, 1.5f);
    gfxServer->SetRenderTarget(this->hdrPostBuffer1.get());
    if (gfxServer->BeginScene())
    {
        nShader2* shader = this->shaders[FilterShader].get();
        if (shader->IsParameterUsed(nShaderState::DiffMap0))
        {
            shader->SetTexture(nShaderState::DiffMap0, this->hdrPostBuffer0.get());
        }
        if (shader->IsParameterUsed(nShaderState::SampleOffsets))
        {
            shader->SetVector4Array(nShaderState::SampleOffsets, this->sampleOffsets, NumSamples);
        }
        if (shader->IsParameterUsed(nShaderState::SampleWeights))
        {
            shader->SetVector4Array(nShaderState::SampleWeights, this->sampleWeights, NumSamples);
        }
        gfxServer->SetShader(shader);
        gfxServer->SetMesh(this->quadMesh.get());
        gfxServer->SetVertexRange(0, 4);
        gfxServer->SetIndexRange(0, 6);
        gfxServer->DrawIndexed(nGfxServer2::TriangleList);
        gfxServer->EndScene();
    }

    // vertical stretch blit
    this->UpdateSampleOffsetsBloom(false, this->postBufferHeight / 2, 3.0f, 1.5f);
    gfxServer->SetRenderTarget(this->hdrPostBuffer0.get());
    if (gfxServer->BeginScene())
    {
        nShader2* shader = this->shaders[FilterShader].get();
        if (shader->IsParameterUsed(nShaderState::DiffMap0))
        {
            shader->SetTexture(nShaderState::DiffMap0, this->hdrPostBuffer1.get());
        }
        if (shader->IsParameterUsed(nShaderState::SampleOffsets))
        {
            shader->SetVector4Array(nShaderState::SampleOffsets, this->sampleOffsets, NumSamples);
        }
        if (shader->IsParameterUsed(nShaderState::SampleWeights))
        {
            shader->SetVector4Array(nShaderState::SampleWeights, this->sampleWeights, NumSamples);
        }
        gfxServer->SetShader(shader);
        gfxServer->SetMesh(this->quadMesh.get());
        gfxServer->SetVertexRange(0, 4);
        gfxServer->SetIndexRange(0, 6);
        gfxServer->DrawIndexed(nGfxServer2::TriangleList);
        gfxServer->EndScene();
    }
}

//------------------------------------------------------------------------------
/**
    Render the scene. This happens in several passes.
*/
void
nHdrSceneServer::RenderScene()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // make sure node resources are loaded
    this->ValidateNodeResources();

    // split nodes into shapes and lights
    this->SplitNodes(FOURCC('colr'));

    // sort shape nodes for optimal rendering
    this->SortNodes();

    // render the scene into the HDR offscreen buffer
    gfxServer->SetRenderTarget(this->hdrSceneBuffer.get());
    if (gfxServer->BeginScene())
    {
        gfxServer->Clear(nGfxServer2::AllBuffers, this->bgColor.x, this->bgColor.y, this->bgColor.z, this->bgColor.w, 1.0f, 0);
        this->RenderLightShapes(FOURCC('colr'));
        gfxServer->EndScene();
    }

    // create the post effect
    this->RenderPostEffect();

    // compose the final frame
    this->UpdateSampleOffsetsGaussBlur5x5(this->postBufferWidth / 2, this->postBufferHeight / 2, 1.0f);
    gfxServer->SetRenderTarget(0);
    if (gfxServer->BeginScene())
    {
        nShader2* shader = this->shaders[ComposeShader].get();
        if (shader->IsParameterUsed(nShaderState::DiffMap0))
        {
            shader->SetTexture(nShaderState::DiffMap0, this->hdrSceneBuffer.get());
        }
        if (shader->IsParameterUsed(nShaderState::DiffMap1))
        {
            shader->SetTexture(nShaderState::DiffMap1, this->hdrPostBuffer0.get());
        }
        if (shader->IsParameterUsed(nShaderState::MatDiffuse))
        {
            shader->SetVector4(nShaderState::MatDiffuse, this->hueColor);
        }
        if (shader->IsParameterUsed(nShaderState::SampleOffsets))
        {
            shader->SetVector4Array(nShaderState::SampleOffsets, this->sampleOffsets, NumSamples);
        }
        if (shader->IsParameterUsed(nShaderState::SampleWeights))
        {
            shader->SetVector4Array(nShaderState::SampleWeights, this->sampleWeights, NumSamples);
        }
        gfxServer->SetShader(shader);
        gfxServer->SetMesh(this->quadMesh.get());
        gfxServer->SetVertexRange(0, 4);
        gfxServer->SetIndexRange(0, 6);
        gfxServer->DrawIndexed(nGfxServer2::TriangleList);
    }
}

//------------------------------------------------------------------------------
/**
    Presents the scene.
*/
void
nHdrSceneServer::PresentScene()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->DrawTextBuffer();
    gfxServer->EndScene();
    gfxServer->PresentScene();
}

//------------------------------------------------------------------------------
/**
    Update sample offsets for a vertical or horizontal blur.
*/
void
nHdrSceneServer::UpdateSampleOffsetsBloom(bool horizontal, int texSize, float deviation, float multiplier)
{
    n_assert(texSize > 0);

    float tu = 1.0f / float(texSize);

    // fill center texel
    float weight = multiplier * this->GaussianDistribution(0.0f, 0.0f, deviation);
    this->sampleWeights[0].set(weight, weight, weight, 0.0f);
    this->sampleOffsets[0].set(0.0f, 0.0f, 0.0f, 0.0f);

    // fill first half
    int i;
    for (i = 0; i < 8; i++)
    {
        if (horizontal)
        {
            this->sampleOffsets[i].set(i * tu, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            this->sampleOffsets[i].set(0.0f, i * tu, 0.0f, 0.0f);
        }
        weight = multiplier * this->GaussianDistribution(float(i), 0, deviation);
        this->sampleWeights[i].set(weight, weight, weight, 0.0f);
    }

    // mirror second half
    for (i = 8; i < 15; i++)
    {
        this->sampleWeights[i].set(this->sampleWeights[i - 7]);
        this->sampleOffsets[i].set(-this->sampleOffsets[i - 7]);
    }
}

//------------------------------------------------------------------------------
/**
    Update sample offsets for a vertical or horizontal blur.
*/
void
nHdrSceneServer::UpdateSampleOffsetsGaussBlur5x5(int texWidth, int texHeight, float multiplier)
{
    n_assert(texWidth > 0);
    n_assert(texHeight > 0);

    float tu = 1.0f / float(texWidth);
    float tv = 1.0f / float(texHeight);

    const vector4 white(1.0f, 1.0f, 1.0f, 0.0f);
    float totalWeight = 0.0f;
    int index = 0;
    int x, y;
    for (x = -2; x <= 2; x++)
    {
        for (y = -2; y <= 2; y++)
        {
            // exclude pixels with block distance > 2, to reduce 5x5 filter
            // to 13 sample points instead of 25 (2.0 shaders can only do
            // 16 lookups)
            if ((abs(x) + abs(y)) > 2)
            {
                continue;
            }

            // get unscaled Gaussian intensity
            this->sampleOffsets[index].set(x * tu, y * tv, 0.0f, 0.0f);
            this->sampleWeights[index].set(white * GaussianDistribution(float(x), float(y), 1.0f));
            totalWeight += this->sampleWeights[index].x;

            index++;
        }
    }

    // normalize weights
    int i;
    float invTotalWeight = 1.0f / totalWeight;
    for (i = 0; i < index; i++)
    {
        this->sampleWeights[i] *= invTotalWeight;
        this->sampleWeights[i] *= multiplier;
    }

    // make sure the extra samples dont influence the result
    for (; index < NumSamples; index++)
    {
        this->sampleWeights[index].set(0.0f, 0.0f, 0.0f, 0.0f);
    }
}
