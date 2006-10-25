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
#include "mathlib/line.h"
#include "scene/nabstractcameranode.h"
#include "util/npriorityarray.h"
#include "scene/nshapenode.h"

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
    occlusionQuery(0),
    occlusionQueryEnabled(true),
    clipPlaneFencing(true),
    guiEnabled(true),
    camerasEnabled(true),
    perfGuiEnabled(false)
{
    n_assert(0 == Singleton);
    Singleton = this;

    PROFILER_INIT(profFrame, "profSceneFrame");
    PROFILER_INIT(profAttach, "profSceneAttach");
    PROFILER_INIT(profValidateResources, "profSceneValidateResources");
    PROFILER_INIT(profSplitNodes, "profSceneSplitNodes");
    PROFILER_INIT(profComputeScissors, "profSceneComputeScissors");
    PROFILER_INIT(profSortNodes, "profSceneSortNodes");
    PROFILER_INIT(profRenderShadow, "profSceneRenderShadow");
    PROFILER_INIT(profOcclusion, "profSceneOcclusion");
    PROFILER_INIT(profRenderPath, "profSceneRenderPath");
    PROFILER_INIT(profRenderCameras, "profSceneRenderCameras");

    WATCHER_INIT(watchNumInstanceGroups, "watchSceneNumInstanceGroups", nArg::Int);
    WATCHER_INIT(watchNumInstances, "watchSceneNumInstances", nArg::Int);
    WATCHER_INIT(watchNumOccluded, "watchSceneNumOccluded", nArg::Int);
    WATCHER_INIT(watchNumNotOccluded, "watchSceneNumNotOccluded", nArg::Int);
    WATCHER_INIT(watchNumPrimitives, "watchGfxNumPrimitives", nArg::Int);
    WATCHER_INIT(watchFPS, "watchGfxFPS", nArg::Float);
    WATCHER_INIT(watchNumDrawCalls, "watchGfxDrawCalls", nArg::Int);

    this->groupArray.SetFlags(nArray<Group>::DoubleGrowSize);
    this->lightArray.SetFlags(nArray<LightInfo>::DoubleGrowSize);
    this->shadowLightArray.SetFlags(nArray<LightInfo>::DoubleGrowSize);
    this->rootArray.SetFlags(nArray<ushort>::DoubleGrowSize);
    this->shadowArray.SetFlags(nArray<ushort>::DoubleGrowSize);

    this->lightNodeClass = nKernelServer::Instance()->FindClass("nlightnode");
    this->groupStack.SetSize(MaxHierarchyDepth);
    this->groupStack.Clear(0);

    // dummy far far away value^^
    this->renderedReflectorDistance = 99999999.9f;

    // default there is no rendered reflector
    this->renderContextPtr = 0;

     // get class pointer to compare, and check this stuff
    reqReflectClass = nKernelServer::Instance()->FindClass("nreflectioncameranode");
    reqRefractClass = nKernelServer::Instance()->FindClass("nclippingcameranode");
    n_assert(reqReflectClass);
    n_assert(reqRefractClass);

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

    PROFILER_START(this->profFrame);
    PROFILER_START(this->profAttach);

    this->stackDepth = 0;
    this->groupStack.Clear(0);
    this->groupArray.Reset();
    this->rootArray.Reset();

    matrix44 view = invView;
    view.invert_simple();
    nGfxServer2::Instance()->SetTransform(nGfxServer2::View, view);

    WATCHER_RESET_INT(watchNumInstanceGroups);
    WATCHER_RESET_INT(watchNumInstances);
    WATCHER_RESET_INT(watchNumOccluded);
    WATCHER_RESET_INT(watchNumNotOccluded);

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
    PROFILER_STOP(this->profAttach);
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

    // split nodes into shapes and lights
    this->SplitNodes();

    // NOTE: this must happen after make sure node resources are loaded
    // because the reflection/refraction camera stuff depends on it
    this->ValidateNodeResources();

    // compute light scissor rectangles and clip planes
    this->ComputeLightScissorsAndClipPlanes();

    // sort shape nodes for optimal rendering
    this->SortNodes();

    // render camera nodes in scene
    if (this->camerasEnabled)
    {
        this->RenderCameraScene();
    }

    /// reset light passes in shape groups between renderpath
    for (int i = 0; i < this->shapeBucket.Size(); i++)
    {
        const nArray<ushort>& shapeArray = this->shapeBucket[i];
        for (int j = 0; j < shapeArray.Size(); j++)
        {
            this->groupArray[shapeArray[j]].lightPass = 0;
        }
    }

    // render final scene
    PROFILER_START(this->profRenderPath);
    int sectionIndex = this->renderPath.FindSectionIndex("default");
    n_assert(-1 != sectionIndex);
    this->DoRenderPath(this->renderPath.GetSection(sectionIndex));
    PROFILER_STOP(this->profRenderPath);

    // HACK...
    this->gfxServerInBeginScene = gfxServer->BeginScene();
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
            this->DebugRenderShapes();
        }
        if (this->perfGuiEnabled)
        {
            this->DebugRenderPerfGui();
        }
        gfxServer->DrawTextBuffer();
        gfxServer->EndScene();
        gfxServer->PresentScene();
    }
    gfxServer->EndFrame();
    PROFILER_STOP(this->profFrame);
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
    PROFILER_START(this->profSplitNodes);

    // reset complex rendered reflector
    this->renderContextPtr = 0;
    this->renderedReflectorDistance = 999999.9f;

    // clear arrays which are filled by this method
    this->shapeBucket.Clear();
    this->lightArray.Clear();
    this->shadowLightArray.Reset();
    this->shadowArray.Reset();
    this->cameraArray.Reset();

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

                // if this is a reflecting shape, parse for render priority
                if (this->IsAReflectingShape(shapeNode))
                {
                    // reset complex flag (we think this one is not the one to be rendered complex)
                    group.renderContext->GetShaderOverrides().SetArg(nShaderState::RenderComplexity, 0);

                    // check if this one is the new (or old) node to be rendered complex
                    if (true == this->ParsePriority(group))
                    {
                        this->cameraArray.Reset();
                        group.renderContext->GetShaderOverrides().SetArg(nShaderState::RenderComplexity, 1);
                    }
                }

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

            // do the following stuff only if this camera is a child of the nearest seanode
            const nRenderContext* renderCandidate = (nRenderContext*) group.renderContext;

            // check if one reflecting camera has priority to be rendered
            if (this->renderContextPtr != 0)
            {

                // if this is the chosen one to be rendered
                if (renderCandidate == this->renderContextPtr)
                {
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
        }
    }
    PROFILER_STOP(this->profSplitNodes);
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
    PROFILER_START(this->profValidateResources);

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
    PROFILER_STOP(this->profValidateResources);
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
    PROFILER_START(this->profSortNodes);

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
    PROFILER_STOP(this->profSortNodes);
}

