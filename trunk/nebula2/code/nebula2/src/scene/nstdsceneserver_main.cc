//------------------------------------------------------------------------------
//  nstdsceneserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nstdsceneserver.h"
#include "scene/nscenenode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/ntexture2.h"
#include "scene/nmaterialnode.h"
#include "gfx2/nshader2.h"
#include "gfx2/ndisplaymode2.h"
#include "gfx2/nmesh2.h"
#include "scene/nspotlightnode.h"

nNebulaClass(nStdSceneServer, "nsceneserver");

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::nStdSceneServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::~nStdSceneServer()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
nStdSceneServer::AreResourcesValid()
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nStdSceneServer::UnloadResources()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nStdSceneServer::LoadResources()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Render the shape objects in the scene, using the shader defined by
    the fourcc code. If no such shader exists, no rendering is done.
*/
void
nStdSceneServer::RenderShapes(uint shaderFourCC)
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

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
                int numPasses = curShader->Begin();
                int curPass;
                for (curPass = 0; curPass < numPasses; curPass++)
                {
                    curShader->Pass(curPass);

                    // for each shape...
                    int shapeIndex;
                    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                    {
                        Group& group = this->groupArray[shapeArray[shapeIndex]];
                        gfxServer->SetTransform(nGfxServer2::Model, group.modelTransform);
                        group.sceneNode->RenderShader(shaderFourCC, this, group.renderContext);
                        this->UpdateShader(curShader, group.renderContext);
                        group.sceneNode->RenderGeometry(this, group.renderContext);
                    }
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
nStdSceneServer::RenderLightShapes(uint shaderFourCC)
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

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
                int numPasses = curShader->Begin();
                int curPass;
                for (curPass = 0; curPass < numPasses; curPass++)
                {
                    curShader->Pass(curPass);

                    // for each shape in bucket...
                    int shapeIndex;
                    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                    {
                        Group& shapeGroup = this->groupArray[shapeArray[shapeIndex]];
                        nMaterialNode* shapeNode = (nMaterialNode*) shapeGroup.sceneNode;

                        // set the modelview matrix for the shape
                        gfxServer->SetTransform(nGfxServer2::Model, shapeGroup.modelTransform);

                        // prepare shader
                        shapeNode->RenderShader(shaderFourCC, this, shapeGroup.renderContext);
                        this->UpdateShader(curShader, shapeGroup.renderContext);

                        // render the "most important light" into the shader (just the first light)
                        int numLights = this->lightArray.Size();
                        if (numLights > 0)
                        {
                            Group& lightGroup = this->groupArray[this->lightArray[0]];
                            lightGroup.sceneNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);
                        }

                        // render!
                        shapeNode->RenderGeometry(this, shapeGroup.renderContext);
                    }
                }
                curShader->End();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Prepare rendering & check if it can be done
*/
bool
nStdSceneServer::BeginScene(const matrix44& invView)
{
    if (nSceneServer::BeginScene(invView))
    {
        nGfxServer2* gfxServer = this->refGfxServer.get();

        // make sure all required resources are ok
        if (!this->AreResourcesValid())
        {
            bool success = this->LoadResources();
            n_assert(success);
        }

        //gfxServer->SetRenderTarget(0);
        this->inBeginScene = gfxServer->BeginScene();
    }
    return this->inBeginScene;
}         

//------------------------------------------------------------------------------
/**
    Render the scene. Rendering happens in multiple passes:

    (1) clear the screen
    (2) render the unlit scene into the offscreen color buffer
    (3) render lighting using the color buffer as texture
*/
void
nStdSceneServer::RenderScene()
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // make sure node resources are loaded
    this->ValidateNodeResources();

    // split nodes into shapes and lights
    this->SplitNodes(FOURCC('colr'));

    // sort shape nodes for optimal rendering
    this->SortNodes();

    gfxServer->Clear(nGfxServer2::AllBuffers, this->bgColor.x, this->bgColor.y, this->bgColor.z, this->bgColor.w, 1.0f, 0);
    this->RenderLightShapes(FOURCC('colr'));
}

//------------------------------------------------------------------------------
/**
    This finalizes rendering and presents the scene to the user.
*/
void
nStdSceneServer::PresentScene()
{
    nGfxServer2* gfxServer = this->refGfxServer.get();
    gfxServer->DrawTextBuffer();
    gfxServer->EndScene();
    gfxServer->PresentScene();
}
