//------------------------------------------------------------------------------
//  nmrtsceneserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmrtsceneserver.h"
#include "scene/nscenenode.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nmaterialnode.h"
#include "gfx2/ndisplaymode2.h"
#include "scene/nshadownode.h"
#include "scene/nlightnode.h"

nNebulaClass(nMRTSceneServer, "nsceneserver");

//------------------------------------------------------------------------------
/**
*/
nMRTSceneServer::nMRTSceneServer() :
    dbgNumInstanceGroups("sceneInstanceGroups", nArg::Int),
    dbgNumInstances("sceneInstances", nArg::Int),
    refShadowServer("/sys/servers/shadow"),
    gfxServerInBeginScene(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMRTSceneServer::~nMRTSceneServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nMRTSceneServer::Open()
{
    nSceneServer::Open();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nMRTSceneServer::RenderShadow()
{
    nShadowServer* shadowServer = this->refShadowServer.get();
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    if (gfxServer->BeginScene())
    {
        // for lights in light array
        int numLights = this->lightArray.Size();
        if (numLights > 0)
        {
            // begin shadow scene
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
        gfxServer->EndScene();
    }
}

//------------------------------------------------------------------------------
/**
    Prepare rendering & check if it can be done
*/
bool
nMRTSceneServer::BeginScene(const matrix44& invView)
{
    nSceneServer::BeginScene(invView);
    this->dbgNumInstanceGroups->SetI(0);
    this->dbgNumInstances->SetI(0);
    return this->inBeginScene;
}         

//------------------------------------------------------------------------------
/**
    Render the scene according the render path definition.
*/
void
nMRTSceneServer::RenderScene()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // make sure node resources are loaded
    this->ValidateNodeResources();

    // split nodes into shapes and lights
    this->SplitNodes(FOURCC('colr'));

    // sort shape nodes for optimal rendering
    this->SortNodes();

    // perform rendering through the render path
    this->DoRenderPath();

    // HACK...
    this->gfxServerInBeginScene = gfxServer->BeginScene();
}

//------------------------------------------------------------------------------
/**
    This finalizes rendering and presents the scene to the user.
*/
void
nMRTSceneServer::PresentScene()
{
    if (this->gfxServerInBeginScene)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        gfxServer->DrawTextBuffer();
        gfxServer->EndScene();
        gfxServer->PresentScene();
    }
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
nMRTSceneServer::DoRenderPath()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // apply lighting
    // (NOTE: a more fine grained lighting model should set lights
    // on a per-shape level(?) by implementing light links between
    // lights and the shapes they are influencing
    int numLights = this->lightArray.Size();
    int i;
    for (i = 0; i < numLights; i++)
    {
        const Group& lightGroup = this->groupArray[this->lightArray[0]];
        lightGroup.sceneNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);
    }


    uint numPasses = this->renderPath.Begin();
    uint passIndex;
    for (passIndex = 0; passIndex < numPasses; passIndex++)
    {
        // for each phase...
        nRpPass& curPass = this->renderPath.GetPass(passIndex);
        
        // special case: shadow volume pass
        if (curPass.GetDrawShadowVolumes())
        {
            this->RenderShadow();
        }
        else
        {
            uint numPhases = curPass.Begin();
            uint phaseIndex;
            for (phaseIndex = 0; phaseIndex < numPhases; phaseIndex++)
            {
                // for each sequence...
                nRpPhase& curPhase = curPass.GetPhase(phaseIndex);
                nFourCC shaderFourCC = curPhase.GetFourCC();
                uint numSeqs = curPhase.Begin();
                uint seqIndex;
                for (seqIndex = 0; seqIndex < numSeqs; seqIndex++)
                {
                    // check if there is anything to render for the next sequence shader at all
                    nRpSequence& curSeq = curPhase.GetSequence(seqIndex);
                    nShader2* curShader = curSeq.GetShader();

                    int bucketIndex = curShader->GetShaderIndex();
                    const nArray<ushort>& shapeArray = this->shapeBucket[bucketIndex];
                    int numShapes = shapeArray.Size();
                    if (numShapes > 0)
                    {
                        uint seqNumPasses = curShader->Begin(false);
                        uint seqPassIndex;
                        for (seqPassIndex = 0; seqPassIndex < seqNumPasses; seqPassIndex++)
                        {
                            curShader->BeginPass(seqPassIndex);

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
                            curShader->EndPass();
                        }
                        curShader->End();
                    }
                }
                curPhase.End();
            }
            curPass.End();
        }
    }
    this->renderPath.End();
}
