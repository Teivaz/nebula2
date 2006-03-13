//------------------------------------------------------------------------------
//  nsceneserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nsceneserver.h"
#include "scene/nscenenode.h"
#include "scene/nrendercontext.h"
#include "scene/nmaterialnode.h"
#include "scene/nlightnode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nocclusionquery.h"
#include "kernel/nfileserver2.h"
#include "shadow2/nshadowserver2.h"
#include "resource/nresourceserver.h"
#include "mathlib/bbox.h"
#include "mathlib/sphere.h"
#include "scene/nabstractcameranode.h"
#include "util/npriorityarray.h"

nNebulaScriptClass(nSceneServer, "nroot");
nSceneServer* nSceneServer::Singleton = 0;

// global data for qsort() compare function
nSceneServer* nSceneServer::self = 0;
vector3 nSceneServer::viewerPos;
int nSceneServer::sortingOrder = nRpPhase::BackToFront;

//------------------------------------------------------------------------------
/**
*/
nSceneServer::nSceneServer() :
    isOpen(false),
    inBeginScene(false),
    obeyLightLinks(false),
    gfxServerInBeginScene(false),
    ffpLightingApplied(false),
    renderDebug(false),
    stackDepth(0),
    shapeBucket(0, 1024),
    dbgNumInstanceGroups("sceneInstanceGroups", nArg::Int),
    dbgNumInstances("sceneInstances", nArg::Int),
    dbgNumOccluded("sceneNumOccluded", nArg::Int),
    dbgNumNotOccluded("sceneNumNotOccluded", nArg::Int),
    dbgOccludeViewerInBox("sceneNumOccludeViewerInBox", nArg::Int),
    occlusionQuery(0),
    occlusionQueryEnabled(true),
    clipPlaneFencing(true)
#if __NEBULA_STATS__
    ,profFrame("profSceneFrame"),
    profAttach("profSceneAttach"),
    profValidateResources("profSceneValidateResources"),
    profSplitNodes("profSceneSplitNodes"),
    profComputeScissors("profSceneComputeScissors"),
    profSortNodes("profSceneSortNodes"),
    profRenderShadow("profSceneRenderShadow"),
    profOcclusion("profSceneOcclusion"),
    profEndScene_TextBuffer("profEndScene_TextBuffer"),
    profEndScene_EndScene("profEndScene_EndScene"),
    profEndScene_PresentScene("profEndScene_PresentScene")
#endif
{
    n_assert(0 == Singleton);
    Singleton = this;

    this->groupArray.SetFlags(nArray<Group>::DoubleGrowSize);
    this->lightArray.SetFlags(nArray<LightInfo>::DoubleGrowSize);
    this->shadowLightArray.SetFlags(nArray<LightInfo>::DoubleGrowSize);
    this->rootArray.SetFlags(nArray<ushort>::DoubleGrowSize);
    this->shadowArray.SetFlags(nArray<ushort>::DoubleGrowSize);

    this->lightNodeClass = nKernelServer::Instance()->FindClass("nlightnode");
    this->groupStack.SetSize(MaxHierarchyDepth);
    this->groupStack.Clear(0);
    self = this;
}

//------------------------------------------------------------------------------
/**
*/
nSceneServer::~nSceneServer()
{
    n_assert(!this->inBeginScene);
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the scene server. This will parse the render path, initialize
    the shaders assign from the render path, and finally invoke
    nGfxServer2::OpenDisplay().
*/
bool
nSceneServer::Open()
{
    n_assert(!this->isOpen);   
    
    // parse renderpath XML file
    if (this->renderPath.OpenXml())
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        nFileServer2* fileServer = nFileServer2::Instance();
        nShadowServer2* shadowServer = nShadowServer2::Instance();

        // initialize the shaders assign from the render path
        fileServer->SetAssign("shaders", this->renderPath.GetShaderPath().Get());

        // open the display
        bool displayOpened = gfxServer->OpenDisplay();
        n_assert(displayOpened);

        // open the shadow server (after opening display!)
        shadowServer->Open();

        // initialize the render path object
        bool renderPathOpened = this->renderPath.Open();
        n_assert(renderPathOpened);

        // unload the XML doc
        this->renderPath.CloseXml();

        // create an occlusion query object
        this->occlusionQuery = gfxServer->NewOcclusionQuery();

        this->isOpen = true;
    }
    else
    {
        n_error("nSceneServer could not open render path file '%s'!", this->renderPath.GetFilename().Get());
    }
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Close the scene server. This will also nGfxServer2::CloseDisplay().
*/
void
nSceneServer::Close()
{
    n_assert(this->isOpen);
    n_assert(this->occlusionQuery);

    this->occlusionQuery->Release();
    this->occlusionQuery = 0;

    this->renderPath.Close();
    nShadowServer2::Instance()->Close();
    nGfxServer2::Instance()->CloseDisplay();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Begin building the scene. Must be called once before attaching 
    nSceneNode hierarchies using nSceneServer::Attach().

    @param  viewer      the viewer position and orientation
*/
bool
nSceneServer::BeginScene(const matrix44& invView)
{
    n_assert(this->isOpen);
    n_assert(!this->inBeginScene);

    #if __NEBULA_STATS__
    this->profFrame.Start();
    this->profAttach.Start();
    #endif

    this->stackDepth = 0;
    this->groupStack.Clear(0);
    this->shapeBucket.Clear();
    this->groupArray.Reset();
    this->lightArray.Clear();
    this->shadowLightArray.Reset();
    this->shadowArray.Reset();
    this->cameraArray.Reset();
    this->rootArray.Reset();

    matrix44 view = invView;
    view.invert_simple();
    nGfxServer2::Instance()->SetTransform(nGfxServer2::View, view);

    this->dbgNumInstanceGroups->SetI(0);
    this->dbgNumInstances->SetI(0);
    this->dbgNumOccluded->SetI(0);
    this->dbgNumNotOccluded->SetI(0);
    this->dbgOccludeViewerInBox->SetI(0);

    this->inBeginScene = nGfxServer2::Instance()->BeginFrame();
    return this->inBeginScene;
}

//------------------------------------------------------------------------------
/**
    Attach a scene node to the scene. This will simply invoke 
    nSceneNode::Attach() on the scene node hierarchie's root object.
*/
void
nSceneServer::Attach(nRenderContext* renderContext)
{
    n_assert(renderContext);
    nSceneNode* rootNode = renderContext->GetRootNode();
    n_assert(rootNode);

    // put index of new root node on root array
    this->rootArray.Append(this->groupArray.Size());

    // reset hints in render context
    renderContext->SetFlag(nRenderContext::Occluded, false);

    // let root node hierarchy attach itself to scene
    rootNode->Attach(this, renderContext);
}

//------------------------------------------------------------------------------
/**
    Finish building the scene.
*/
void
nSceneServer::EndScene()
{
    // make sure the modelview stack is clear
    n_assert(0 == this->stackDepth);
    this->inBeginScene = false;

    #if __NEBULA_STATS__
    this->profAttach.Stop();
    #endif
}

//------------------------------------------------------------------------------
/**
    This method is called back by nSceneNode objects in their Attach() method
    to notify the scene server that a new hierarchy group starts.
*/
void
nSceneServer::BeginGroup(nSceneNode* sceneNode, nRenderContext* renderContext)
{
    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(this->stackDepth < MaxHierarchyDepth);

    // initialize new group node
    // FIXME: could be optimized to have no temporary 
    // object which must be copied onto array
    Group group;
    group.sceneNode = sceneNode;
    group.renderContext = renderContext;
    group.lightPass = 0;
    bool isTopLevel;
    if (0 == this->stackDepth)
    {
        group.parentIndex = -1;
        isTopLevel = true;
    }
    else
    {
        group.parentIndex = this->groupStack[this->stackDepth - 1];
        isTopLevel = false;
    }
    renderContext->SetSceneGroupIndex(this->groupArray.Size());
    this->groupArray.Append(group);

    // push pointer to group onto hierarchy stack
    this->groupStack[this->stackDepth] = this->groupArray.Size() - 1;
    ++this->stackDepth;

    // immediately call the scene node's RenderTransform method
    if (isTopLevel)
    {
        matrix44 topMatrix = renderContext->GetTransform();
        sceneNode->RenderTransform(this, renderContext, topMatrix);
    }
    else
    {
        sceneNode->RenderTransform(this, renderContext, this->groupArray[group.parentIndex].modelTransform);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called back by nSceneNode objects in their Attach() method
    to notify the scene server that a hierarchy group ends.
*/
void
nSceneServer::EndGroup()
{
    n_assert(this->stackDepth > 0);
    this->stackDepth--;
}

//------------------------------------------------------------------------------
/**
    Render the actual scene. This method should be implemented by
    subclasses of nSceneServer. The frame will not be visible until
    PresentScene() is called. Additional render calls to the gfx server
    can be invoked between RenderScene() and PresentScene().
*/
void
nSceneServer::RenderScene()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // make sure node resources are loaded
    this->ValidateNodeResources();

    // split nodes into shapes and lights
    this->SplitNodes();

    // compute light scissor rectangles and clip planes
    this->ComputeLightScissorsAndClipPlanes();

    // sort shape nodes for optimal rendering
    this->SortNodes();

    // render camera nodes in scene
    this->RenderCameraScene();

    // render final scene
    int sectionIndex = this->renderPath.FindSectionIndex("default");
    n_assert(-1 != sectionIndex);
    this->DoRenderPath(this->renderPath.GetSection(sectionIndex));

    // HACK...
    this->gfxServerInBeginScene = gfxServer->BeginScene();
}

//------------------------------------------------------------------------------
/**
    Render the scenes for each camera
*/
void
nSceneServer::RenderCameraScene()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    int i;
    for (i = 0; i < this->cameraArray.Size(); i++)
    {          
        // get the camera node
        Group& cameraNodeGroup = this->groupArray[cameraArray[i]];
        nAbstractCameraNode* cameraNode = (nAbstractCameraNode*) cameraNodeGroup.sceneNode;   
    
        // check if the render target available
        const nString& rpSectionName = cameraNode->GetRenderPathSection();
        int sectionIndex = this->renderPath.FindSectionIndex(rpSectionName);
        if (-1 != sectionIndex)
        {
            // update camera
            cameraNode->RenderCamera(cameraNodeGroup.modelTransform,
                                    nGfxServer2::Instance()->GetTransform(nGfxServer2::View),
                                    nGfxServer2::Instance()->GetTransform(nGfxServer2::Projection));

            // temp view- and projection matrix
            gfxServer->PushTransform(nGfxServer2::View, cameraNode->GetViewMatrix());
            gfxServer->PushTransform(nGfxServer2::Projection, cameraNode->GetProjectionMatrix());

            // perform rendering through the render path
            this->DoRenderPath(this->renderPath.GetSection(sectionIndex));

            // restore matrices
            gfxServer->PopTransform(nGfxServer2::Projection);
            gfxServer->PopTransform(nGfxServer2::View);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Finalize rendering and present the current frame. No additional rendering
    calls may be invoked after calling nSceneServer::PresentScene()
*/
void
nSceneServer::PresentScene()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    if (this->gfxServerInBeginScene)
    {
        if (this->renderDebug)
        {
            this->DebugRenderLightScissors();
        }

        #if __NEBULA_STATS__
        this->profEndScene_TextBuffer.Start();
        #endif
        gfxServer->DrawTextBuffer();
        
        #if __NEBULA_STATS__
        this->profEndScene_TextBuffer.Stop();
        this->profEndScene_EndScene.Start();
        #endif

        gfxServer->EndScene();
        
        #if __NEBULA_STATS__
        this->profEndScene_EndScene.Stop();
        this->profEndScene_PresentScene.Start();
        #endif

        gfxServer->PresentScene();
        
        #if __NEBULA_STATS__
        this->profEndScene_PresentScene.Stop();
        #endif
    }
    gfxServer->EndFrame();

    #if __NEBULA_STATS__
    this->profFrame.Stop();
    #endif
}

//------------------------------------------------------------------------------
/**
    Split the collected scene nodes into light and shape nodes. Fills
    the lightArray[] and shapeArray[] members. This method is available
    as a convenience method for subclasses.
*/
void
nSceneServer::SplitNodes()
{
    #if __NEBULA_STATS__
    this->profSplitNodes.Start();
    #endif   

    ushort i;
    ushort num = this->groupArray.Size();
    for (i = 0; i < num; i++)
    {
        Group& group = this->groupArray[i];
        n_assert(group.sceneNode);

        if (group.sceneNode->HasGeometry())
        {
            if (group.renderContext->GetFlag(nRenderContext::ShapeVisible))
            {
                nMaterialNode* shapeNode = (nMaterialNode*) group.sceneNode;
                int shaderIndex = shapeNode->GetShaderIndex();
                if (shaderIndex > -1)
                {
                    this->shapeBucket[shaderIndex].Append(i);
                }
            }
        }
        if (group.sceneNode->HasLight())
        {
            n_assert(group.sceneNode->IsA(this->lightNodeClass));
            group.renderContext->SetSceneLightIndex(this->lightArray.Size());
            LightInfo lightInfo;
            lightInfo.groupIndex = i;
            this->lightArray.Append(lightInfo);
        }
        if (group.sceneNode->HasShadow())
        {
            if (group.renderContext->GetFlag(nRenderContext::ShadowVisible))
            {
                this->shadowArray.Append(i);
            }
        }
        
        if (group.sceneNode->HasCamera())
        {
            nAbstractCameraNode* newCamera = (nAbstractCameraNode*) group.sceneNode;
            
            // HACK!!!: at the moment the cameras are only used for water, and all use
            // the same render target (because this is defined in the section, not by the
            // camera. Therefor it is useless to render more than one camera per section.
            // If later other cameras are used this must be fixed. A way must be found
            // to decide if 2 cameras are the same, or creating different rendertarget results.
            
            // check if we alread have a camera using the same renderpath section
            int c;
            bool uniqueCamera = true;
            for (c = 0; c < this->cameraArray.Size(); c++)
            {
                Group& group = this->groupArray[this->cameraArray[c]];
                nAbstractCameraNode* existingCamera = (nAbstractCameraNode*) group.sceneNode;   
                if (existingCamera->GetRenderPathSection() == newCamera->GetRenderPathSection())
                {
                    uniqueCamera = false;
                    break;
                }
            }
            
            if (uniqueCamera)
            {
                this->cameraArray.Append(i);
            }
        }
    }

    #if __NEBULA_STATS__
    this->profSplitNodes.Stop();
    #endif
}

//------------------------------------------------------------------------------
/**
    This makes sure that all attached shape and light nodes have
    loaded their resources. This method is available
    as a convenience method for subclasses.
*/
void
nSceneServer::ValidateNodeResources()
{
    #if __NEBULA_STATS__
    this->profValidateResources.Start();
    #endif
    
    // need to evaluate camera nodes first, because they create
    // textures used by other nodes
    ushort i;
    ushort num = this->cameraArray.Size();
    for (i = 0; i < num; i++)
    {
        Group& group = this->groupArray[this->cameraArray[i]];
        if (!group.sceneNode->AreResourcesValid())
        {
            group.sceneNode->LoadResources();
        }
    }
    
    // then evaluate the rest
    num = this->groupArray.Size();   
    for (i = 0; i < num; i++)
    {
        const Group& group = this->groupArray[i];
        if (!group.sceneNode->AreResourcesValid())
        {
            group.sceneNode->LoadResources();
        }
    }

    #if __NEBULA_STATS__
    this->profValidateResources.Stop();
    #endif
}

//------------------------------------------------------------------------------
/**
    The scene node sorting compare function. The goal is to sort the attached
    shape nodes for optimal rendering performance.
*/
int
__cdecl
nSceneServer::CompareNodes(const ushort* i1, const ushort* i2)
{
    nSceneServer* sceneServer = nSceneServer::self;
    const nSceneServer::Group& g1 = sceneServer->groupArray[*i1];
    const nSceneServer::Group& g2 = sceneServer->groupArray[*i2];
    int cmp;

    // by render pri
    cmp = g1.sceneNode->GetRenderPri() - g2.sceneNode->GetRenderPri();
    if (cmp != 0)
    {
        return cmp;
    }

    // by identical scene node
    cmp = int(g1.sceneNode) - int(g2.sceneNode);
    if (cmp != 0)
    {
        return cmp;
    }

    // distance to viewer (closest first)
    static vector3 dist1;
    static vector3 dist2;
    dist1.set(viewerPos.x - g1.modelTransform.M41,
              viewerPos.y - g1.modelTransform.M42,
              viewerPos.z - g1.modelTransform.M43);
    dist2.set(viewerPos.x - g2.modelTransform.M41,
              viewerPos.y - g2.modelTransform.M42,
              viewerPos.z - g2.modelTransform.M43);
    float diff = dist1.lensquared() - dist2.lensquared();

    if (sortingOrder == nRpPhase::FrontToBack)
	{	
		// (closest first)
        if (diff < 0.001f)      return -1;
		else if (diff > 0.001f) return +1;
    }
	else if (sortingOrder == nRpPhase::BackToFront)
    {		
        if (diff > 0.001f)      return -1;
        else if (diff < 0.001f) return +1;
    }

    // nodes are identical
    return 0;
}

//------------------------------------------------------------------------------
/**
    Specialized sorting function for shadow casting light sources,
    sorts lights by range and distance.
*/
int
__cdecl
nSceneServer::CompareShadowLights(const LightInfo* i1, const LightInfo* i2)
{
    nSceneServer* sceneServer = nSceneServer::self;
    const nSceneServer::Group& g1 = sceneServer->groupArray[i1->groupIndex];
    const nSceneServer::Group& g2 = sceneServer->groupArray[i2->groupIndex];

    // compute intensity
    static vector3 dist1;
    static vector3 dist2;
    dist1.set(viewerPos.x - g1.modelTransform.M41, viewerPos.y - g1.modelTransform.M42, viewerPos.z - g1.modelTransform.M43);
    dist2.set(viewerPos.x - g2.modelTransform.M41, viewerPos.y - g2.modelTransform.M42, viewerPos.z - g2.modelTransform.M43);
    nLightNode* ln1 = (nLightNode*) g1.sceneNode;
    nLightNode* ln2 = (nLightNode*) g2.sceneNode;
    n_assert(ln1->IsA(sceneServer->lightNodeClass));
    n_assert(ln2->IsA(sceneServer->lightNodeClass));
    float range1 = ln1->GetLocalBox().extents().x;
    float range2 = ln2->GetLocalBox().extents().x;
    float intensity1 = n_saturate(dist1.len() / range1);
    float intensity2 = n_saturate(dist2.len() / range2);
    float diff = intensity1 - intensity2;
    if (diff < 0.001f)      return -1;
    else if (diff > 0.001f) return +1;
    else                    return 0;
}

//------------------------------------------------------------------------------
/**
    Sort the indices in the shape array for optimal rendering.
*/
void
nSceneServer::SortNodes()
{
    #if __NEBULA_STATS__
    this->profSortNodes.Start();
    #endif

    // initialize the static viewer pos vector
    viewerPos = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView).pos_component();    

    // for each bucket: call the sorter hook
    int i;
    int num = this->shapeBucket.Size();
    for (i = 0; i < num; i++)
    {
        ushort* indexPtr = (ushort*) this->shapeBucket[i].Begin();
        int numIndices = this->shapeBucket[i].Size();
        if (numIndices > 0)
        {
            qsort(indexPtr, numIndices, sizeof(ushort), (int(__cdecl *)(const void *, const void *)) CompareNodes);
        }
    }

    // sort shadow casting light sources
    int numShadowLights = this->shadowLightArray.Size();
    if (numShadowLights > 0)
    {
        qsort(&(this->shadowLightArray[0]), numShadowLights, sizeof(LightInfo), (int(__cdecl *)(const void *, const void *)) CompareShadowLights);
    }

    #if __NEBULA_STATS__
    this->profSortNodes.Stop();
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneServer::RenderShadow(nRpPass& curPass)
{
    nShadowServer2* shadowServer = nShadowServer2::Instance();
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // check if simple or multilight shadows should be drawn
    // (DX7 can only do simple shadowing with 1 light source)
    // to 1 under DX7 and 4 under DX9
    int maxShadowLights = 1;
    if (curPass.GetDrawShadows() == nRpPass::MultiLight)
    {
        // initialize rendering for multilight shadows
        maxShadowLights = 4;
        nTexture2* renderTarget = (nTexture2*) nResourceServer::Instance()->FindResource(curPass.GetRenderTargetName(0).Get(), nResource::Texture);
        n_assert(renderTarget);
        gfxServer->SetRenderTarget(0, renderTarget);
    }
    gfxServer->SetLightingType(nGfxServer2::Off);
    gfxServer->SetHint(nGfxServer2::MvpOnly, true);

    // prepare the graphics server
    // set shadow projection matrix, this is the normal projection
    // matrix with slightly shifted near and far plane to reduce
    // z-fighting
    matrix44 shadowProj = gfxServer->GetTransform(nGfxServer2::ShadowProjection);
    gfxServer->PushTransform(nGfxServer2::Projection, shadowProj);
    
    if (gfxServer->BeginScene())
    {
        if (curPass.GetDrawShadows() == nRpPass::MultiLight)
        {
            gfxServer->Clear(nGfxServer2::ColorBuffer, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
        }

        // for each shadow casting light...
        int numShadowLights = this->shadowLightArray.Size();
        if (maxShadowLights > numShadowLights) 
        {
            maxShadowLights = numShadowLights;
        }

        if ((numShadowLights > 0) && (this->shadowArray.Size() > 0))
        {
            // begin shadow scene
            if (shadowServer->BeginScene()) 
            {    
                int shadowLightIndex;
                for (shadowLightIndex = 0; shadowLightIndex < maxShadowLights; shadowLightIndex++)
                {                    
                    // only process non-occluded lights
                    const LightInfo& lightInfo = this->shadowLightArray[shadowLightIndex];
                    Group& lightGroup = this->groupArray[lightInfo.groupIndex];
                    if (!lightGroup.renderContext->GetFlag(nRenderContext::Occluded))
                    {
                        nLightNode* lightNode = (nLightNode*) lightGroup.sceneNode;
                        n_assert(lightNode->GetCastShadows());

                        // get light position in world space
                        lightNode->ApplyLight(this, lightGroup.renderContext, lightGroup.modelTransform, lightInfo.shadowLightMask);
                        lightNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);
                        const nLight& light = lightNode->GetLight();
                        float shadowIntensity = lightGroup.renderContext->GetShadowIntensity();

                        shadowServer->BeginLight(light);
                        this->ApplyLightScissors(lightInfo);

                        // FIXME: sort shadow nodes by shadow caster geometry
                        int numShapes = this->shadowArray.Size();
                        int shapeIndex;
                        for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                        {
                            // render non-occluded shadow casters
                            Group& shapeGroup = this->groupArray[shadowArray[shapeIndex]];
                            n_assert(shapeGroup.renderContext->GetFlag(nRenderContext::ShadowVisible));
                            if (!shapeGroup.renderContext->GetFlag(nRenderContext::Occluded))
                            {
                                if (this->obeyLightLinks)
                                {
                                    // check if current shadow casting light sees this shape
                                    if (this->IsShapeLitByLight(shapeGroup, lightGroup))
                                    {
                                        shapeGroup.sceneNode->RenderShadow(this, shapeGroup.renderContext, shapeGroup.modelTransform);
                                        this->dbgNumInstances->SetI(this->dbgNumInstances->GetI() + 1);
                                    }
                                }
                                else
                                {
                                    // no obey light links, just render the shadow
                                    shapeGroup.sceneNode->RenderShadow(this, shapeGroup.renderContext, shapeGroup.modelTransform);
                                    this->dbgNumInstances->SetI(this->dbgNumInstances->GetI() + 1);
                                }
                            }
                        }
                        shadowServer->EndLight();

                        // if multilight shadowing, store the stencil buffer in an accumulation render target
                        if (curPass.GetDrawShadows() == nRpPass::MultiLight)
                        {
                            this->CopyStencilBufferToTexture(curPass, lightInfo.shadowLightMask);
                        }
                    }
                }    
                shadowServer->EndScene();
            }
        }
        gfxServer->EndScene();
    }
    if (curPass.GetDrawShadows() == nRpPass::MultiLight)
    {
        gfxServer->SetRenderTarget(0, 0);
    }        
    gfxServer->PopTransform(nGfxServer2::Projection);
    gfxServer->SetHint(nGfxServer2::MvpOnly, false);
}

//------------------------------------------------------------------------------
/**
    Render a single shape with light mode Off. Called by generic
    RenderShape() method.
*/
void
nSceneServer::RenderShapeLightModeOff(const Group& shapeGroup)
{
    this->ffpLightingApplied = false;
    shapeGroup.sceneNode->RenderGeometry(this, shapeGroup.renderContext);
    this->dbgNumInstances->SetI(this->dbgNumInstances->GetI() + 1);
}

//------------------------------------------------------------------------------
/**
    Render a complete phase for light mode "Off" or "FFP"
*/
void
nSceneServer::RenderPhaseLightModeOff(nRpPhase& curPhase)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->SetLightingType(nGfxServer2::Off);

    uint numSeqs = curPhase.Begin();
    uint seqIndex;
    for (seqIndex = 0; seqIndex < numSeqs; seqIndex++)
    {
        // check if there is anything to render for the next sequence shader at all
        nRpSequence& curSeq = curPhase.GetSequence(seqIndex);
        bool shaderUpdatesEnabled = curSeq.GetShaderUpdatesEnabled();
        int bucketIndex = curSeq.GetShaderBucketIndex();
        n_assert(bucketIndex >= 0);
        const nArray<ushort>& shapeArray = this->shapeBucket[bucketIndex];
        int numShapes = shapeArray.Size();
        if (numShapes > 0)
        {
            uint seqNumPasses = curSeq.Begin();
            uint seqPassIndex;
            for (seqPassIndex = 0; seqPassIndex < seqNumPasses; seqPassIndex++)
            {
                curSeq.BeginPass(seqPassIndex);

                // for each shape in bucket
                int shapeIndex;
                nMaterialNode* prevShapeNode = 0;
                for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                {
                    const Group& shapeGroup = this->groupArray[shapeArray[shapeIndex]];
                    n_assert(shapeGroup.renderContext->GetFlag(nRenderContext::ShapeVisible));
                    if (!shapeGroup.renderContext->GetFlag(nRenderContext::Occluded))
                    {
                        nMaterialNode* shapeNode = (nMaterialNode*) shapeGroup.sceneNode;
                        if (shapeNode != prevShapeNode)
                        {
                            // start a new instance set
                            shapeNode->ApplyShader(this);
                            shapeNode->ApplyGeometry(this);
                            this->dbgNumInstanceGroups->SetI(this->dbgNumInstanceGroups->GetI() + 1);
                        }
                        prevShapeNode = shapeNode;
                    
                        // set modelview matrix for the shape
                        gfxServer->SetTransform(nGfxServer2::Model, shapeGroup.modelTransform);

                        // set per-instance shader parameters
                        if (shaderUpdatesEnabled)
                        {
                            shapeNode->RenderShader(this, shapeGroup.renderContext);
                        }
                        this->RenderShapeLightModeOff(shapeGroup);
                    }
                }
                curSeq.EndPass();
            }
            curSeq.End();
        }
    }
    curPhase.End();
}

//------------------------------------------------------------------------------
/**
    Render a single shape with light mode FFP (Fixed Function Pipeline). 
    Called by generic RenderShape() method.

    FIXME: obey light OCCLUSION status!!!
*/
void
nSceneServer::RenderShapeLightModeFFP(const Group& shapeGroup)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    vector4 dummyShadowLightMask;
        
    // Render with vertex-lighting and multiple light sources
    if (this->obeyLightLinks)
    {
        // use light links, each shape rendercontext is linked to
        // all light rendercontexts which illuminate this shape,
        // light links are provided by the application
        gfxServer->ClearLights();
        int lightIndex;
        int numLights = shapeGroup.renderContext->GetNumLinks();
        if (numLights > nGfxServer2::MaxLights)
        {
            numLights = nGfxServer2::MaxLights;
        }
        for (lightIndex = 0; lightIndex < numLights; lightIndex++)
        {
            nRenderContext* lightRenderContext = shapeGroup.renderContext->GetLinkAt(lightIndex);
            const Group& lightGroup = this->groupArray[lightRenderContext->GetSceneGroupIndex()];
            n_assert(lightRenderContext == lightGroup.renderContext);
            n_assert(lightGroup.sceneNode->HasLight());
            lightGroup.sceneNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);
            lightGroup.sceneNode->ApplyLight(this, lightGroup.renderContext, lightGroup.modelTransform, dummyShadowLightMask);
        }
    }
    else
    {
        // ignore light links, each shape is influenced by each light
        // Optimization: if lighting has been applied for this 
        // frame already, we don't need to do it again. This will only
        // work if rendering doesn't go through light links though
        if (!this->ffpLightingApplied)
        {
            gfxServer->ClearLights();
            int lightIndex;
            int numLights = this->lightArray.Size();
            if (numLights > nGfxServer2::MaxLights)
            {
                numLights = nGfxServer2::MaxLights;
            }
            for (lightIndex = 0; lightIndex < numLights; lightIndex++)
            {
                const Group& lightGroup = this->groupArray[this->lightArray[lightIndex].groupIndex];
                n_assert(lightGroup.sceneNode->HasLight());
                lightGroup.sceneNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);
                lightGroup.sceneNode->ApplyLight(this, lightGroup.renderContext, lightGroup.modelTransform, dummyShadowLightMask);
            }
            this->ffpLightingApplied = true;
        }
    }
    shapeGroup.sceneNode->RenderGeometry(this, shapeGroup.renderContext);
    this->dbgNumInstances->SetI(this->dbgNumInstances->GetI() + 1);
}

//------------------------------------------------------------------------------
/**
    Render a complete phase for light mode "FFP", this is pretty much the same
    as LightModeOff. For each shapes, the state for all lights influencing
    this shape are set, and then the shape is rendered.
*/
void
nSceneServer::RenderPhaseLightModeFFP(nRpPhase& curPhase)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->SetLightingType(nGfxServer2::FFP);
    
    uint numSeqs = curPhase.Begin();
    uint seqIndex;
    for (seqIndex = 0; seqIndex < numSeqs; seqIndex++)
    {
        // check if there is anything to render for the next sequence shader at all
        nRpSequence& curSeq = curPhase.GetSequence(seqIndex);
        bool shaderUpdatesEnabled = curSeq.GetShaderUpdatesEnabled();
        int bucketIndex = curSeq.GetShaderBucketIndex();
        n_assert(bucketIndex >= 0);
        const nArray<ushort>& shapeArray = this->shapeBucket[bucketIndex];
        int numShapes = shapeArray.Size();
        if (numShapes > 0)
        {
            uint seqNumPasses = curSeq.Begin();
            uint seqPassIndex;
            for (seqPassIndex = 0; seqPassIndex < seqNumPasses; seqPassIndex++)
            {
                curSeq.BeginPass(seqPassIndex);

                // for each shape in bucket
                int shapeIndex;
                nMaterialNode* prevShapeNode = 0;
                for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                {
                    const Group& shapeGroup = this->groupArray[shapeArray[shapeIndex]];
                    n_assert(shapeGroup.renderContext->GetFlag(nRenderContext::ShapeVisible));
                    if (!shapeGroup.renderContext->GetFlag(nRenderContext::Occluded))
                    {
                        nMaterialNode* shapeNode = (nMaterialNode*) shapeGroup.sceneNode;
                        if (shapeNode != prevShapeNode)
                        {
                            // start a new instance set
                            shapeNode->ApplyShader(this);
                            shapeNode->ApplyGeometry(this);
                            this->dbgNumInstanceGroups->SetI(this->dbgNumInstanceGroups->GetI() + 1);
                        }
                        prevShapeNode = shapeNode;
                    
                        // set modelview matrix for the shape
                        gfxServer->SetTransform(nGfxServer2::Model, shapeGroup.modelTransform);

                        // set per-instance shader parameters
                        if (shaderUpdatesEnabled)
                        {
                            shapeNode->RenderShader(this, shapeGroup.renderContext);
                        }
                        this->RenderShapeLightModeFFP(shapeGroup);
                    }
                }
                curSeq.EndPass();
            }
            curSeq.End();
        }
    }
    curPhase.End();
}

//------------------------------------------------------------------------------
/**
    Render a single shape with light mode Shader.
    Called by generic RenderShape() method.
*/
void
nSceneServer::RenderShapeLightModeShader(Group& shapeGroup, const nRpSequence& seq)
{
    bool firstLightAlpha = seq.GetFirstLightAlphaEnabled();
    nShader2* shd = nGfxServer2::Instance()->GetShader();
    if (0 == shapeGroup.lightPass++)
    {
        shd->SetBool(nShaderState::AlphaBlendEnable, firstLightAlpha);
    }
    else                             
    {
        shd->SetBool(nShaderState::AlphaBlendEnable, true);
    }
    shapeGroup.sceneNode->RenderGeometry(this, shapeGroup.renderContext);
    this->dbgNumInstances->SetI(this->dbgNumInstances->GetI() + 1);
}

//------------------------------------------------------------------------------
/**
    Return true if the given light is in the light links list of the
    shape.
*/
bool
nSceneServer::IsShapeLitByLight(const Group& shapeGroup, const Group& lightGroup)
{
    n_assert(shapeGroup.renderContext);
    n_assert(lightGroup.renderContext);

    int i;
    int num = shapeGroup.renderContext->GetNumLinks();
    for (i = 0; i < num; i++)
    {
        nRenderContext* shapeLightRenderContext = shapeGroup.renderContext->GetLinkAt(i);
        if (shapeLightRenderContext == lightGroup.renderContext)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render a complete phase for light mode "Shader". This updates the light's
    status in the outer loop and then renders all shapes influenced by this
    light. This will minimize render state switches between draw calls.
*/
void
nSceneServer::RenderPhaseLightModeShader(nRpPhase& curPhase)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->SetLightingType(nGfxServer2::Shader);
    this->ffpLightingApplied = false;

    // for each light...
    int numLights = this->lightArray.Size();
    int lightIndex;
    for (lightIndex = 0; lightIndex < numLights; lightIndex++)
    {
        gfxServer->ClearLights();
        const LightInfo& lightInfo = this->lightArray[lightIndex];
        const Group& lightGroup = this->groupArray[lightInfo.groupIndex];
        nRenderContext* lightRenderContext = lightGroup.renderContext;
        n_assert(lightGroup.sceneNode->HasLight());

        // do nothing if light is occluded
        if (!lightRenderContext->GetFlag(nRenderContext::Occluded))
        {
            // apply light state
            lightGroup.sceneNode->ApplyLight(this, lightGroup.renderContext, lightGroup.modelTransform, lightInfo.shadowLightMask);

            // now iterate through sequences...
            uint numSeqs = curPhase.Begin();

            // NOTE: nRpPhase::Begin resets the scissor rect, thus this must happen afterwards!
            this->ApplyLightScissors(lightInfo);
            this->ApplyLightClipPlanes(lightInfo);

            uint seqIndex;
            for (seqIndex = 0; seqIndex < numSeqs; seqIndex++)
            {
                // check if there is anything to render for the next sequence shader at all
                nRpSequence& curSeq = curPhase.GetSequence(seqIndex);
                bool shaderUpdatesEnabled = curSeq.GetShaderUpdatesEnabled();
                int bucketIndex = curSeq.GetShaderBucketIndex();
                n_assert(bucketIndex >= 0);
                const nArray<ushort>& shapeArray = this->shapeBucket[bucketIndex];
                int numShapes = shapeArray.Size();
                if (numShapes > 0)
                {
                    uint seqNumPasses = curSeq.Begin();
                    uint seqPassIndex;
                    for (seqPassIndex = 0; seqPassIndex < seqNumPasses; seqPassIndex++)
                    {
                        curSeq.BeginPass(seqPassIndex);

                        // for each shape in bucket
                        int shapeIndex;
                        nMaterialNode* prevShapeNode = 0;
                        for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                        {
                            Group& shapeGroup = this->groupArray[shapeArray[shapeIndex]];
                            n_assert(shapeGroup.renderContext->GetFlag(nRenderContext::ShapeVisible));

                            // don't render if shape is occluded
                            if (!shapeGroup.renderContext->GetFlag(nRenderContext::Occluded))
                            {
                                // don't render if shape not lit by current light
                                bool shapeInfluencedByLight = true;
                                if (this->obeyLightLinks)
                                {
                                    shapeInfluencedByLight = this->IsShapeLitByLight(shapeGroup, lightGroup);
                                }
                                if (shapeInfluencedByLight)
                                {
                                    nMaterialNode* shapeNode = (nMaterialNode*) shapeGroup.sceneNode;
                                    if (shapeNode != prevShapeNode)
                                    {
                                        // start a new instance set
                                        shapeNode->ApplyShader(this);
                                        shapeNode->ApplyGeometry(this);
                                        this->dbgNumInstanceGroups->SetI(this->dbgNumInstanceGroups->GetI() + 1);
                                    }
                                    prevShapeNode = shapeNode;
                                
                                    // set modelview matrix for the shape
                                    gfxServer->SetTransform(nGfxServer2::Model, shapeGroup.modelTransform);

                                    // set per-instance shader parameters
                                    if (shaderUpdatesEnabled)
                                    {
                                        shapeNode->RenderShader(this, shapeGroup.renderContext);
                                    }
                                    lightGroup.sceneNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);

                                    this->RenderShapeLightModeShader(shapeGroup, curSeq);
                                }
                            }
                        }
                        curSeq.EndPass();
                    }
                    curSeq.End();
                }
            }
            curPhase.End();
        }
    }
    this->ResetLightScissorsAndClipPlanes();
}

//------------------------------------------------------------------------------
/**
    This implements the complete render path scene rendering. A render
    path is made of a shader hierarchy of passes, phases and sequences, designed
    to eliminate redundant shader state switches as much as possible.

    FIXME FIXME FIXME:
    Implement phase SORTING hints!
*/
void
nSceneServer::DoRenderPath(nRpSection& rpSection)
{
    uint numPasses = rpSection.Begin();
    uint passIndex;

    for (passIndex = 0; passIndex < numPasses; passIndex++)
    {
        // for each phase...
        nRpPass& curPass = rpSection.GetPass(passIndex);

        if (curPass.GetDrawShadows() != nRpPass::NoShadows)
        { 
            #if __NEBULA_STATS__
            this->profRenderShadow.Start();
            #endif


            // find shadow casting light sources
            this->GatherShadowLights();

            // special case: shadow volume pass
            this->RenderShadow(curPass);

            #if __NEBULA_STATS__
            this->profRenderShadow.Stop();
            #endif
        }
        else if (curPass.GetOcclusionQuery())
        {
            // perform light source occlusion query, this
            // marks the light sources in the scene as occluded or not
            this->DoOcclusionQuery();
        }
        else
        {
            // default case: render phases and sequences
            uint numPhases = curPass.Begin();
            uint phaseIndex;
            for (phaseIndex = 0; phaseIndex < numPhases; phaseIndex++)
            {
                this->ffpLightingApplied = false;

                // for each sequence...
                nRpPhase& curPhase = curPass.GetPhase(phaseIndex);
                if (curPhase.GetLightMode() == nRpPhase::Off)
                {
                    this->RenderPhaseLightModeOff(curPhase);
                }
                else if (curPhase.GetLightMode() == nRpPhase::FFP)
                {
                    this->RenderPhaseLightModeFFP(curPhase);
                }
                else if (curPhase.GetLightMode() == nRpPhase::Shader)
                {
                    this->RenderPhaseLightModeShader(curPhase);
                }
            }
            curPass.End();
        }
    }
    rpSection.End();
}

//------------------------------------------------------------------------------
/**
    Computes the scissor rect info for a single info structure.
*/
void
nSceneServer::ComputeLightScissor(LightInfo& lightInfo)
{
    #if __NEBULA_STATS__
    this->profComputeScissors.Start();
    #endif

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    const Group& lightGroup = this->groupArray[lightInfo.groupIndex];
    nLightNode* lightNode = (nLightNode*) lightGroup.sceneNode;
    n_assert(lightNode);
    n_assert(lightNode->IsA(this->lightNodeClass));

    nLight::Type lightType = lightNode->GetType();
    if (nLight::Point == lightType)
    {
        // compute the point light's projected rectangle on screen
        const bbox3& localBox = lightNode->GetLocalBox();
        sphere sphere(lightGroup.modelTransform.pos_component(), localBox.extents().x);

        const matrix44& view = gfxServer->GetTransform(nGfxServer2::View);
        const matrix44& projection = gfxServer->GetTransform(nGfxServer2::Projection);
        const nCamera2& cam = gfxServer->GetCamera();

        lightInfo.scissorRect = sphere.project_screen_rh(view, projection, cam.GetNearPlane());
    }
    else if (nLight::Directional == lightType)
    {
        // directional lights cover the whole screen
        static const rectangle fullScreenRect(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f));
        lightInfo.scissorRect = fullScreenRect;
    }
    else
    {
        n_error("nSceneServer::ComputeLightScissors(): unsupported light type!");
    }

    #if __NEBULA_STATS__
    this->profComputeScissors.Stop();
    #endif
}

//------------------------------------------------------------------------------
/**
    Computes the clip planes for a single light source.
*/
void
nSceneServer::ComputeLightClipPlanes(LightInfo& lightInfo)
{
    if (this->clipPlaneFencing)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();

        const Group& lightGroup = this->groupArray[lightInfo.groupIndex];
        nLightNode* lightNode = (nLightNode*) lightGroup.sceneNode;

        n_assert(0 != lightNode && lightNode->IsA(this->lightNodeClass));
        
        lightInfo.clipPlanes.Reset();

        nLight::Type lightType = lightNode->GetType();
        if (nLight::Point == lightType)
        {
            // get the point light's global space bounding box
            matrix44 mvp = lightGroup.modelTransform * gfxServer->GetTransform(nGfxServer2::ViewProjection);
            lightNode->GetLocalBox().get_clipplanes(mvp, lightInfo.clipPlanes);
        }
        else if (nLight::Directional == lightType)
        {
            // directional light have no user clip planes
        }
        else
        {
            n_error("nSceneServer::ComputeLightClipPlanes(): unsupported light type!");
        }
    }
}

//------------------------------------------------------------------------------
/**
    Iterates through the light groups and computes the scissor rectangle
    for each light.
*/
void
nSceneServer::ComputeLightScissorsAndClipPlanes()
{
    // update lights
    int lightIndex;
    int numLights = this->lightArray.Size();
    for (lightIndex = 0; lightIndex < numLights; lightIndex++)
    {
        LightInfo& lightInfo = this->lightArray[lightIndex];
        this->ComputeLightScissor(lightInfo);
        this->ComputeLightClipPlanes(lightInfo);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneServer::ApplyLightScissors(const LightInfo& lightInfo)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->SetScissorRect(lightInfo.scissorRect);
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneServer::ApplyLightClipPlanes(const LightInfo& lightInfo)
{
    if (this->clipPlaneFencing)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        gfxServer->SetClipPlanes(lightInfo.clipPlanes);
    }
}

//------------------------------------------------------------------------------
/**
    Reset the light scissor rect and clip planes.
*/
void
nSceneServer::ResetLightScissorsAndClipPlanes()
{
    if (this->clipPlaneFencing)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        static const rectangle fullScreenRect(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f));
        gfxServer->SetScissorRect(fullScreenRect);
        nArray<plane> nullArray(0, 0);
        gfxServer->SetClipPlanes(nullArray);
    }
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the light scissors.
*/
void
nSceneServer::DebugRenderLightScissors()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    static const vector4 red(1.0f, 0.0f, 0.0f, 1.0f);
    static const vector4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
    static const vector4 blue(0.0f, 0.0f, 1.0f, 1.0f);
    static const vector4 pink(1.0f, 0.0f, 1.0f, 1.0f);
    gfxServer->BeginLines();

    // render light scissors
    int lightIndex;
    int numLights = this->lightArray.Size();
    for (lightIndex = 0; lightIndex < numLights; lightIndex++)
    {
        const LightInfo& lightInfo = this->lightArray[lightIndex];
        const Group& lightGroup = this->groupArray[lightInfo.groupIndex];
        const rectangle& r = lightInfo.scissorRect;
        vector2 lines[5];
        lines[0].set(r.v0.x, r.v0.y);
        lines[1].set(r.v1.x, r.v0.y);
        lines[2].set(r.v1.x, r.v1.y);
        lines[3].set(r.v0.x, r.v1.y);
        lines[4].set(r.v0.x, r.v0.y);
        if (lightGroup.renderContext->GetFlag(nRenderContext::Occluded)) gfxServer->DrawLines2d(lines, 5, blue);
        else                                                             gfxServer->DrawLines2d(lines, 5, red);
    }

    // render shadow light scissors
    int shadowLightIndex;
    int numShadowLights = this->shadowLightArray.Size();
    for (shadowLightIndex = 0; shadowLightIndex < numShadowLights; shadowLightIndex++)
    {
        const LightInfo& lightInfo = this->shadowLightArray[shadowLightIndex];
        const Group& lightGroup = this->groupArray[lightInfo.groupIndex];
        const rectangle& r = lightInfo.scissorRect;
        vector2 lines[5];
        lines[0].set(r.v0.x, r.v0.y);
        lines[1].set(r.v1.x, r.v0.y);
        lines[2].set(r.v1.x, r.v1.y);
        lines[3].set(r.v0.x, r.v1.y);
        lines[4].set(r.v0.x, r.v0.y);
        if (lightGroup.renderContext->GetFlag(nRenderContext::Occluded)) gfxServer->DrawLines2d(lines, 5, pink);
        else                                                             gfxServer->DrawLines2d(lines, 5, yellow);
    }
    gfxServer->EndLines();
}

//------------------------------------------------------------------------------
/**
    Copy the current stencil buffer state into a texture color channel.
    This accumulates the stencil bits for up to 4 light sources
    (one per RGBA channel).
*/
void
nSceneServer::CopyStencilBufferToTexture(nRpPass& rpPass, const vector4& shadowLightMask)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nShader2* shd = rpPass.GetShader();
    if (shd)
    {
        shd->SetVector4(nShaderState::ShadowIndex, shadowLightMask);   
        gfxServer->SetShader(shd);    
        shd->Begin(true);
        shd->BeginPass(0);
        rpPass.DrawFullScreenQuad();
        shd->EndPass();
        shd->End();
    }
}

//------------------------------------------------------------------------------
/**
    Issue a general occlusion query for a root scene node group.
*/
void
nSceneServer::IssueOcclusionQuery(Group& group, const vector3& viewerPos)
{
    nSceneNode* sceneNode = group.sceneNode;
    n_assert(sceneNode);

    // initialize occlusion flags
    group.renderContext->SetFlag(nRenderContext::Occluded, false);

    // special case light:
    if (sceneNode->HasLight())
    {
        // don't do occlusion check for directional light
        nLightNode* lightNode = (nLightNode*) sceneNode;
        if (nLight::Directional == lightNode->GetType())
        {
            return;
        }
    }

    // get conservative bounding boxes in global space, the first for
    // the occlusion check is grown a little to prevent that the object
    // occludes itself, the second checks if the viewer is in the bounding
    // box, and if yes, no occlusion check is done
    const bbox3& globalBox = group.renderContext->GetGlobalBox();
    
    // check whether the bounding box is very small in one or more dimensions
    // which may lead to zbuffer artefacts during the occlusion check, in that
    // case, don't do an occlusion query for this object
    // FIXME: could also be done once at load time in Mangalore...
    vector3 extents = globalBox.extents();
    if ((extents.x < 0.001f) || (extents.y < 0.001f) || (extents.z < 0.001f))
    {
        return;
    }
    bbox3 occlusionBox(globalBox.center(), globalBox.extents() * 1.1f);
    bbox3 viewerCheckBox(globalBox.center(), globalBox.extents() * 1.2f);

    // convert back to a matrix for shape rendering
    matrix44 occlusionShapeMatrix = occlusionBox.to_matrix44();

    // check if viewer position is inside current bounding box,
    // if yes, don't perform occlusion check
    if (!viewerCheckBox.contains(viewerPos))
    {
        this->occlusionQuery->AddShapeQuery(nGfxServer2::Box, occlusionShapeMatrix, &group);
    }
    else
    {
        this->dbgOccludeViewerInBox->SetI(this->dbgOccludeViewerInBox->GetI() + 1);
    }
}

//------------------------------------------------------------------------------
/**
    This performs a general occlusion query on all root nodes in the scene.
    Note that light nodes especially benefit from the occlusion query
    since objects which are lit by occluded light sources don't need to
    render the light pass for their lit objects.
*/
void
nSceneServer::DoOcclusionQuery()
{
    #if __NEBULA_STATS__
    this->profOcclusion.Start();
    #endif

    n_assert(this->occlusionQuery);

    if (this->occlusionQueryEnabled)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();

        // get current viewer position, NOTE: move the viewer position
        // into the screen onto the near plane since the occlusion check
        // needs to check whether the viewer is inside the occlusion bounding box
        // to check, if we don't account for the near plane then the front plane
        // of the occlusion plane might be clipped which would return 0 drawn pixels
        // when the object really isn't occluded (simply turning off backface
        // culling won't help either in some cases!)
        
        // FIXME: hmm, this method sucks... better to check viewer position against
        // a slightly scaled bounding box in IssueOcclusionQuery()!
        const vector3& viewerPos = gfxServer->GetTransform(nGfxServer2::InvView).pos_component();
        if (gfxServer->BeginScene())
        {
            // only update ModelViewProjection matrix in shaders...
            gfxServer->SetHint(nGfxServer2::MvpOnly, true);

            // issue queries...
            int i;
            int num = this->rootArray.Size();
            this->occlusionQuery->Begin();
            for (i = 0; i < num; i++)
            {
                Group& group = this->groupArray[this->rootArray[i]];
                if (group.renderContext->GetFlag(nRenderContext::DoOcclusionQuery))
                {
                    this->IssueOcclusionQuery(group, viewerPos);
                }
            }
            this->occlusionQuery->End();

            // get query results...
            // (NOTE: we could split this out and query the results
            // later, since the query will run in parallel to the CPU...
            // if only we had something useful todo in the meantime)
            int queryIndex;
            int numQueries = this->occlusionQuery->GetNumQueries();
            for (queryIndex = 0; queryIndex < numQueries; queryIndex++)
            {
                Group* group = (Group*) this->occlusionQuery->GetUserData(queryIndex);
                bool occluded = this->occlusionQuery->GetOcclusionStatus(queryIndex);
                if (occluded)
                {
                    group->renderContext->SetFlag(nRenderContext::Occluded, true);
                    this->dbgNumOccluded->SetI(this->dbgNumOccluded->GetI() + 1);
                }
                else
                {
                    this->dbgNumNotOccluded->SetI(this->dbgNumNotOccluded->GetI() + 1);
                }
            }
            this->occlusionQuery->Clear();

            gfxServer->SetHint(nGfxServer2::MvpOnly, false);
            gfxServer->EndScene();
        }
    }

    #if __NEBULA_STATS__
    this->profOcclusion.Stop();
    #endif
}

//------------------------------------------------------------------------------
/**
    This method goes through all attached light sources and decides which 
    4 of them should cast shadows. This takes the occlusion status, distance and
    range and intensity into account. The method should be called after 
    occlusion culling. The result is that the shadowLightArray will be filled.
*/
void
nSceneServer::GatherShadowLights()
{
    n_assert(this->shadowLightArray.Size() == 0);

    vector3 viewerPos = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView).pos_component();
    nPriorityArray<int> priorityArray(MaxShadowLights);

    int numLights = this->lightArray.Size();
    int lightIndex;
    for (lightIndex = 0; lightIndex < numLights; lightIndex++)
    {
        const LightInfo& lightInfo = this->lightArray[lightIndex];
        const Group& lightGroup = this->groupArray[lightInfo.groupIndex];

        // only look at shadow casting light sources
        if (lightGroup.renderContext->GetFlag(nRenderContext::CastShadows))
        {
            // ignore occluded light sources
            if (!lightGroup.renderContext->GetFlag(nRenderContext::Occluded))
            {
                nLightNode* lightNode = (nLightNode*) lightGroup.sceneNode;
                vector3 distVec = lightGroup.modelTransform.pos_component() - viewerPos;
                float dist = distVec.len();
                float range = lightNode->GetFloat(nShaderState::LightRange);
                float priority;
                switch (lightNode->GetType())
                {
                    case nLight::Point:
                        priority = -(dist / range);
                        break;

                    case nLight::Directional:
                        priority = 100000.0f;
                        break;

                    default:
                        priority = 0.0f;
                        break;
                }
                priorityArray.Add(lightIndex, priority);
            }
        }
    }

    // the 4 highest priority light sources are now in the priority array
    int i;
    for (i = 0; i < priorityArray.Size(); i++)
    {
        LightInfo& lightInfo = this->lightArray[priorityArray[i]];
        const Group& lightGroup = this->groupArray[lightInfo.groupIndex];
        float shadowIntensity = lightGroup.renderContext->GetShadowIntensity();
        lightInfo.shadowLightMask = nGfxServer2::GetShadowLightIndexVector(i, shadowIntensity);
        this->shadowLightArray.Append(lightInfo);
    }
}
