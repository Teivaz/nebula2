//------------------------------------------------------------------------------
//  ninstsceneserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ninstsceneserver.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nrendercontext.h"
#include "scene/nscenenode.h"

nNebulaClass(nInstSceneServer, "nsceneserver");

//------------------------------------------------------------------------------
/**
*/
nInstSceneServer::nInstSceneServer() :
    nodeBucket(0, 256)
{
    int i;
    int num = this->nodeBucket.Size();
    for (i = 0; i < num; i++)
    {
        this->nodeBucket[i].SetFlags(nArray::DoubleGrowSize);
    }
}

//------------------------------------------------------------------------------
/**
*/
nInstSceneServer::~nInstSceneServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Begin building the scene. Must be called once before attaching 
    nSceneNode hierarchies using nSceneServer::Attach().

    @param  invView      the viewer position and orientation
*/
bool
nInstSceneServer::BeginScene(const matrix44& invView)
{
    n_assert(!this->inBeginScene);

    this->nodeBucket.Reset();

    matrix44 view = invView;
    view.invert_simple();
    this->refGfxServer->SetTransform(nGfxServer2::View, view);
    
    // this->inBeginScene = this->refGfxServer->BeginScene();

    return true;
}

//------------------------------------------------------------------------------
/**
    Attach a scene node hierarchy to the scene.
*/
void
nInstSceneServer::Attach(nRenderContext* renderContext)
{
    n_assert(renderContext);
    n_assert(this->inBeginScene);
    nSceneNode* rootNode = renderContext->GetRootNode();
    n_assert(rootNode);
    rootNode->Attach(this, renderContext);
}

//------------------------------------------------------------------------------
/**
    Finish building the scene.
*/
void
nInstSceneServer::EndScene()
{
    n_assert(this->inBeginScene);
    this->inBeginScene = false;
}

//------------------------------------------------------------------------------
/**
    This method is called back by nSceneNode objects in their Attach() method
    to notify the scene server that a new hierarchy group starts.
*/
void
nInstSceneServer::BeginGroup(nSceneNode* sceneNode, nRenderContext* renderContext)
{
/*
    // first check if the scene node has already been added this frame
    if (this->frameId == sceneNode->GetFrameId())
    {
        // yes, has been added, just append render context to the node
        sceneNode->GetInstanceArray().Append(renderContext);
    }
    else
    {
        // has not been added yet, if this is a visible scene node, add it 
        // to the right shader bucket
        if (sceneNode->HasShader())
        {
            nMaterialNode* node = (nMaterialNode*) sceneNode;
            nShader2* shader = node->GetShaderObject(FOURCC('colr'));
            if (shader)
            {
                uint bucketIndex = shader->GetShaderIndex();
                this->nodeBucket[bucketIndex].Append(sceneNode);
                sceneNode->SetFrameId(this->frameId);
            }
        }

        // push the 
    }

                this->shapeBucket[shaderBucket].Append(i);


        if (sceneNode->HasShader(nFourCC



    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(this->nodeStackDepth < MaxHierarchyDepth);

    // initialize new group node
    // FIXME: could be optimized to have no temporary 
    // object which must be copied onto array
    Node node;
    node.sceneNode = sceneNode;
    node.rootSceneNode = this->rootSceneNodes.Back();

    bool isTopLevel;
    if (0 == this->nodeStackDepth)
    {
        node.parent = 0;
        isTopLevel = true;
    }
    else
    {
        node.parent = this->nodeStack[this->nodeStackDepth - 1];
        isTopLevel = false;
    }
    this->nodeArray.Append(node);

    // push pointer to node onto hierarchy stack
    this->nodeStack[this->nodeStackDepth] = &(this->nodeArray.Back());
    ++this->nodeStackDepth;

    // immediately call the scene node's RenderTransform method
    if (isTopLevel)
    {
        matrix44 identity;
        sceneNode->RenderTransform(this, renderContext, identity);
    }
    else
    {
        sceneNode->RenderTransform(this, renderContext, node.parent->relModelTransform);
    }
*/
}

//------------------------------------------------------------------------------
/**
    This method is called back by nSceneNode objects in their Attach() method
    to notify the scene server that a hierarchy group ends.
*/
void
nInstSceneServer::EndGroup()
{
    n_assert(this->nodeStackDepth > 0);
    this->nodeStackDepth--;
}

//------------------------------------------------------------------------------
/**
*/
void
nInstSceneServer::SetModelTransform(const matrix44& m)
{
    this->nodeArray.Back().relModelTransform = m;
}

//------------------------------------------------------------------------------
/**
*/
const matrix44&
nInstSceneServer::GetModelTransform() const
{
    return this->nodeArray.Back().relModelTransform;
}

//------------------------------------------------------------------------------
/**
    Render the actual scene. This method should be implemented by
    subclasses of nSceneServer. The frame will not be visible until
    PresentScene() is called. Additional render calls to the gfx server
    can be invoked between RenderScene() and PresentScene().
*/
void
nInstSceneServer::RenderScene()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Finalize rendering and present the current frame. No additional rendering
    calls may be invoked after calling nSceneServer::PresentScene()
*/
void
nInstSceneServer::PresentScene()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Add a new element to the instance stream of the provided scene node.
*/
void
nInstSceneServer::AddInstanceData(nRenderContext* renderContext)
{
    // FIXME!
}








