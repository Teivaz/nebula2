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
nSceneServer* nSceneServer::Singleton = 0;

// global data for qsort() compare function
nSceneServer* nSceneServer::self = 0;
vector3 nSceneServer::viewerPos;

//------------------------------------------------------------------------------
/**
*/
nSceneServer::nSceneServer() :
    isOpen(false),
    inBeginScene(false),
    groupArray(512, 1024),
    lightArray(64, 128),
    shadowArray(512, 1024),
    stackDepth(0),
    shapeBucket(0, 1024),
    bgColor(0.5f, 0.5f, 0.5f, 1.0f)
{
    n_assert(0 == Singleton);
    Singleton = this;

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
    Open the scene server. This initializes the embedded render path
    object.
*/
bool
nSceneServer::Open()
{
    n_assert(!this->isOpen);
    
    // read the renderpath definition file
    this->renderPath.SetFilename("shaders:renderpath.xml");
    bool renderPathOpened = this->renderPath.Open();
    n_assert(renderPathOpened);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the scene server.
*/
void
nSceneServer::Close()
{
    n_assert(this->isOpen);
    this->renderPath.Close();
    this->isOpen = false;
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
    n_assert(this->isOpen);
    n_assert(!this->inBeginScene);

    this->stackDepth = 0;
    this->groupStack.Clear(0);
    this->shapeBucket.Clear();
    this->groupArray.Reset();
    this->lightArray.Reset();
    this->shadowArray.Reset();

    matrix44 view = invView;
    view.invert_simple();
    nGfxServer2::Instance()->SetTransform(nGfxServer2::View, view);
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
        group.parentIndex = -1;
        isTopLevel = true;
    }
    else
    {
        group.parentIndex = this->groupStack[this->stackDepth - 1];
        isTopLevel = false;
    }
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
    Split the collected scene nodes into light and shape nodes. Fills
    the lightArray[] and shapeArray[] members. This method is available
    as a convenience method for subclasses.
*/
void
nSceneServer::SplitNodes(uint shaderFourCC)
{
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
                int shaderIndex = shader->GetShaderIndex();
                if (shaderIndex > -1)
                {
                    this->shapeBucket[shaderIndex].Append(i);
                }
                else
                {
                    n_printf("Trying to render shader '%s' through scene server,\nwhich is not defined in renderpath xml file!", shader->GetFilename().Get());
                }
            }
        }
        if (group.sceneNode->HasLight())
        {
            this->lightArray.Append(i);
        }
        if (group.sceneNode->HasShadow())
        {
            this->shadowArray.Append(i);
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
            DoSort(indexPtr, numIndices);
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

//------------------------------------------------------------------------------
/**
*/
void
nSceneServer::SetModelTransform(const matrix44& m)
{
    this->groupArray.Back().modelTransform = m;
}

//------------------------------------------------------------------------------
/**
*/
const matrix44&
nSceneServer::GetModelTransform() const
{
    return this->groupArray.Back().modelTransform;
}
