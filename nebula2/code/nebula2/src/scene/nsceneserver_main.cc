//------------------------------------------------------------------------------
//  nsceneserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nsceneserver.h"
#include "scene/nscenenode.h"
#include "scene/nrendercontext.h"
#include "scene/nmaterialnode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "kernel/ntimeserver.h"

nNebulaClass(nSceneServer, "nroot");

// global data for qsort() compare function
nSceneServer* nSceneServer::self = 0;
vector3 nSceneServer::viewerPos;

//------------------------------------------------------------------------------
/**
*/
nSceneServer::nSceneServer() :
    refGfxServer("/sys/servers/gfx"),
    inBeginScene(false),
    groupArray(512, 1024),
    lightArray(64, 128),
    stackDepth(0),
    shapeBucket(0, 1024),
    bgColor(0.5f, 0.5f, 0.5f, 1.0f)
{
    memset(this->groupStack, 0, sizeof(this->groupStack));
    self = this;
}

//------------------------------------------------------------------------------
/**
*/
nSceneServer::~nSceneServer()
{
    n_assert(!this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
    This method returns true if a specific shader type is used by this
    scene graph object. The shader type is defined by a FourCC signature.
    Scene node can ask their scene graph object whether a shader is used
    to decide whether a shader must be loaded. Subclasses of nSceneServer which
    implement specific scene rendering algorithms should override this
    method.

    @param  fourcc  a fourcc shader type signature
    @return         true if scene graph uses shaders of this type
*/
bool
nSceneServer::IsShaderUsed(uint /*fourcc*/) const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Begin building the scene. Must be called once before attaching 
    nSceneNode hierarchies using nSceneServer::Attach().

    @param  invView      the viewer position and orientation
*/
bool
nSceneServer::BeginScene(const matrix44& invView)
{
    n_assert(!this->inBeginScene);

    this->groupArray.Reset();
    this->lightArray.Reset();
    this->stackDepth = 0;
    matrix44 view = invView;
    view.invert_simple();
    this->refGfxServer->SetTransform(nGfxServer2::View, view);
    this->inBeginScene = true;

    return true;
}

//------------------------------------------------------------------------------
/**
    Attach a scene node to the scene. This will simply invoke 
    nSceneNode::Attach() on the scene node hierarchy's root object.
*/
void
nSceneServer::Attach(nRenderContext* renderContext)
{
    n_assert(renderContext);
    nSceneNode* rootNode = renderContext->GetRootNode();
    n_assert(rootNode);
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
    bool isTopLevel;
    if (0 == this->stackDepth)
    {
        group.parent = 0;
        isTopLevel = true;
    }
    else
    {
        group.parent = this->groupStack[this->stackDepth - 1];
        isTopLevel = false;
    }
    this->groupArray.Append(group);

    // push pointer to group onto hierarchy stack
    this->groupStack[this->stackDepth] = &(this->groupArray.Back());
    ++this->stackDepth;

    // immediately call the scene node's RenderTransform method
    if (isTopLevel)
    {
        matrix44 topMatrix = renderContext->GetTransform();
        sceneNode->RenderTransform(this, renderContext, topMatrix);
    }
    else
    {
        sceneNode->RenderTransform(this, renderContext, group.parent->modelTransform);
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
    // empty
}
    
//------------------------------------------------------------------------------
/**
    Finalize rendering and present the current frame. No additional rendering
    calls may be invoked after calling nSceneServer::PresentScene()
*/
void
nSceneServer::PresentScene()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This sets standard parameters, like the various matrices in the
    provided shader object. Provided to subclasses as a convenience method.
*/
void
nSceneServer::UpdateShader(nShader2* shd, nRenderContext* renderContext)
{
    n_assert(shd);
    n_assert(renderContext);

    // write global parameters the shader
    nGfxServer2* gfxServer = this->refGfxServer.get();
    const matrix44& invModelView  = refGfxServer->GetTransform(nGfxServer2::InvModelView);
    if (shd->IsParameterUsed(nShader2::Time))
    {
        nTime time = this->kernelServer->GetTimeServer()->GetTime();
        shd->SetFloat(nShader2::Time, float(time));
    }

    // FIXME: this should be a shared shader parameter
    if (shd->IsParameterUsed(nShader2::DisplayResolution))
    {
        const nDisplayMode2& mode = gfxServer->GetDisplayMode();
        nFloat4 dispRes;
        dispRes.x = (float) mode.GetWidth();
        dispRes.y = (float) mode.GetHeight();
        dispRes.z = 0.0f;
        dispRes.w = 0.0f;
        shd->SetFloat4(nShader2::DisplayResolution, dispRes);
    }

    // set shader overrides
    shd->SetParams(renderContext->GetShaderOverrides());
}

//------------------------------------------------------------------------------
/** 
    Split the collected scene nodes into light and shape nodes. Fills
    the lightArray[] and shapeArray[] members. This method is available
    as a convenience method for subclasses.
*/  
void
nSceneServer::SplitNodes(uint shaderFourCC)
{   
    this->shapeBucket.Clear();
    ushort i;
    ushort num = this->groupArray.Size();
    for (i = 0; i < num; i++)
    {
        Group& group = this->groupArray[i];
        n_assert(group.sceneNode);

        if (group.sceneNode->HasGeometry())
        {
            nMaterialNode* shapeNode = (nMaterialNode*) group.sceneNode;
            nShader2* shader = shapeNode->GetShaderObject(shaderFourCC);
            if (shader)
            {
                uint shaderBucket = shader->GetShaderIndex();
                this->shapeBucket[shaderBucket].Append(i);
            }
        }
        if (group.sceneNode->HasLight())
        {
            this->lightArray.Append(i);
        }
    }
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
    uint i;
    uint num = this->groupArray.Size();
    for (i = 0; i < num; i++)
    {
        const Group& group = this->groupArray[i];
        if (!group.sceneNode->AreResourcesValid())
        {
            group.sceneNode->LoadResources();
        }
    }
}

//------------------------------------------------------------------------------
/**
    The scene node sorting compare function. The goal is to sort the attached
    shape nodes for optimal rendering performance.

    FIXME: handling of transparent nodes, compare shaders
*/
int
__cdecl
nSceneServer::Compare(const ushort* i1, const ushort* i2)
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
    if (diff < 0.001f)      return -1;
    else if (diff > 0.001f) return +1;

    // nodes are identical
    return 0;
}

//------------------------------------------------------------------------------
/**
    Sort the indices in the shape array for optimal rendering.
*/
void
nSceneServer::SortNodes()
{
    // initialize the static viewer pos vector
    viewerPos = this->refGfxServer->GetTransform(nGfxServer2::InvView).pos_component();

    // for each bucket: call the sorter hook
    int i;
    int num = this->shapeBucket.Size();
    for (i = 0; i < num; i++)
    {
        ushort* indexPtr = (ushort*) this->shapeBucket[i].Begin();
        int numIndices = this->shapeBucket[i].Size();
        if (numIndices > 0)
        {
            DoSort( indexPtr, numIndices );
        }
    }
}

//------------------------------------------------------------------------------
/**
    The actual sort call is separated out to make overriding Compare possible.
*/
void
nSceneServer::DoSort( ushort* indexPtr, int numIndices )
{
    qsort(indexPtr, numIndices, sizeof(ushort), (int(__cdecl *)(const void *, const void *)) Compare);
}
//------------------------------------------------------------------------------
/**
    Get shader object valid for a whole bucket.
*/
nShader2*
nSceneServer::GetBucketShader(int bucketIndex, uint fourcc)
{
    const nArray<ushort>& shapeArray = this->shapeBucket[bucketIndex];
    nMaterialNode* shapeNode = (nMaterialNode*) this->groupArray[shapeArray[0]].sceneNode;
    n_assert(shapeNode);
    nShader2* shader = shapeNode->GetShaderObject(fourcc);
    return shader;
}

