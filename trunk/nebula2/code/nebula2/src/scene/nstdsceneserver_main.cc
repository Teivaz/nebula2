//------------------------------------------------------------------------------
//  nstdsceneserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nstdsceneserver.h"
#include "scene/nscenenode.h"
#include "scene/nmaterialnode.h"
#include "scene/nshadownode.h"
#include "scene/nlightnode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ndisplaymode2.h"
#include "gfx2/nmesh2.h"

nNebulaClass(nStdSceneServer, "nsceneserver");

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::nStdSceneServer() :
    dbgNumInstanceGroups("sceneInstanceGroups", nArg::Int),
    dbgNumInstances("sceneInstances", nArg::Int)
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
    This implements the complete render path scene rendering. A render
    path is made of a shader hierarchy of passes, phases and sequences, designed
    to eliminate redundant shader state switches as much as possible.

    FIXME FIXME FIXME:
    Implement phase SORTING and LIGHTING hints!
*/
void
nStdSceneServer::DoRenderPath()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // for each scene pass...
    uint numPasses = this->renderPath.GetNumPasses();
    uint passIndex;
    for (passIndex = 0; passIndex < numPasses; passIndex++)
    {
        this->renderPath.BeginPass(passIndex);

        // for each phase in scene pass...
        uint numPhases = this->renderPath.GetNumPhases();
        uint phaseIndex;
        for (phaseIndex = 0; phaseIndex < numPhases; phaseIndex++)
        {
            this->renderPath.BeginPhase(phaseIndex);
            nFourCC shaderFourCC = renderPath.GetPhase().GetFourCC();

            // for each sequence in pass...
            uint numSeqs = this->renderPath.GetNumSequences();
            uint seqIndex;
            for (seqIndex = 0; seqIndex < numSeqs; seqIndex++)
            {
                // check if there is anything to render for the next sequence shader at all
                const nRenderPath::Sequence& seq = this->renderPath.GetSequence(seqIndex);
                nShader2* curShader = seq.GetShader();

                int bucketIndex = curShader->GetShaderIndex();
                const nArray<ushort>& shapeArray = this->shapeBucket[bucketIndex];
                int numShapes = shapeArray.Size();
                if (numShapes > 0)
                {
                    nShader2* shd = this->renderPath.GetSequenceShader(seqIndex);
                    uint seqNumPasses = shd->Begin(false);
                    uint seqPassIndex;
                    for (seqPassIndex = 0; seqPassIndex < seqNumPasses; seqPassIndex++)
                    {
                        shd->BeginPass(seqPassIndex);

                        // for each shape in bucket
                        int shapeIndex;
                        nMaterialNode* prevShapeNode = 0;
                        for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                        {
                            const Group& shapeGroup = this->groupArray[shapeArray[shapeIndex]];
                            nMaterialNode* shapeNode = (nMaterialNode*) shapeGroup.sceneNode;
                            if (shapeNode != prevShapeNode)
                            {
                                // start a new instance set
                                shapeNode->ApplyShader(shaderFourCC, this);
                                shapeNode->ApplyGeometry(this);
                                this->dbgNumInstanceGroups->SetI(this->dbgNumInstanceGroups->GetI() + 1);
                            }
                            prevShapeNode = shapeNode;

                            // set modelview matrix for the shape
                            gfxServer->SetTransform(nGfxServer2::Model, shapeGroup.modelTransform);

                            // set per-instance shader parameters
                            shapeNode->RenderShader(shaderFourCC, this, shapeGroup.renderContext);

                            // finally, render!
                            shapeNode->RenderGeometry(this, shapeGroup.renderContext);
                            this->dbgNumInstances->SetI(this->dbgNumInstances->GetI() + 1);
                        }
                        shd->EndPass();
                    }
                    shd->End();
                }
            }
            this->renderPath.EndPhase();
        }
        this->renderPath.EndPass();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nStdSceneServer::RenderShadow()
{
    nShadowServer* shadowServer = nShadowServer::Instance();
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // for lights in light array
    int numLights = this->lightArray.Size();
    if (numLights > 0)
    {
        //begin shadow scene
        if (shadowServer->BeginScene()) // is shadow enabled?
        {
            //sort shadow nodes by shadow caster geometry
            // TODO

            int numLights = 1; //FIXME: only render the 1st light in the moment
            int lightIndex;
            for(lightIndex = 0; lightIndex < numLights; lightIndex++)
            {
                // get light
                Group& lightGroup = this->groupArray[this->lightArray[lightIndex]];
                nLightNode* lightNode = (nLightNode*) lightGroup.sceneNode;

                // TODO: get light position in world space (without gfx server)
                lightNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);
                vector3 lightPosition(gfxServer->GetTransform(nGfxServer2::Light).pos_component());

                // begin light
                shadowServer->BeginLight(lightNode->GetType(), lightPosition);

                // nShadowNode* prevShadowNode = 0;
                // for shapes in shadow array
                int numShapes = this->shadowArray.Size();
                int shapeIndex;
                for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                {
                    // get shadow node
                    Group& shapeGroup = this->groupArray[shadowArray[shapeIndex]];
                    nShadowNode* shadowNode = (nShadowNode*) shapeGroup.sceneNode;

                    // render the instance
                    shadowNode->RenderShadow(this, shapeGroup.renderContext, shapeGroup.modelTransform);
                    this->dbgNumInstances->SetI(this->dbgNumInstances->GetI() + 1);
                }
                shadowServer->EndLight();
            }
            // end scene (render shadow plane)
            shadowServer->EndScene();
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
        nGfxServer2* gfxServer = nGfxServer2::Instance();

        // make sure all required resources are ok
        if (!this->AreResourcesValid())
        {
            bool success = this->LoadResources();
            n_assert(success);
        }

        this->inBeginScene = nGfxServer2::Instance()->BeginScene();

        this->dbgNumInstanceGroups->SetI(0);
        this->dbgNumInstances->SetI(0);
    }
    return this->inBeginScene;
}

//------------------------------------------------------------------------------
/**
    Render the scene. Rendering happens in multiple passes:

    (1) clear the screen
    (2) render the unlit scene into the offscreen color buffer
    (3) render lighting using the color buffer as texture
    (4) render shadow
*/
void
nStdSceneServer::RenderScene()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // make sure node resources are loaded
    this->ValidateNodeResources();

    // split nodes into shapes and lights
    this->SplitNodes(FOURCC('colr'));

    // sort shape nodes for optimal rendering
    this->SortNodes();

    gfxServer->Clear(nGfxServer2::AllBuffers, this->bgColor.x, this->bgColor.y, this->bgColor.z, this->bgColor.w, 1.0f, 0);

    this->DoRenderPath();

    this->RenderShadow();
}

//------------------------------------------------------------------------------
/**
    This finalizes rendering and presents the scene to the user.
*/
void
nStdSceneServer::PresentScene()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->DrawTextBuffer();
    gfxServer->EndScene();
    gfxServer->PresentScene();
}
