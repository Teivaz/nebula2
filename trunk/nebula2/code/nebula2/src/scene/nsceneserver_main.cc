#define N_IMPLEMENTS nSceneServer
//------------------------------------------------------------------------------
//  nsceneserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nsceneserver.h"
#include "scene/nscenenode.h"
#include "scene/nrendercontext.h"
#include "gfx2/ngfxserver2.h"

nNebulaClass(nSceneServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nSceneServer::nSceneServer() :
    refGfxServer("/sys/servers/gfx"),
    inBeginScene(false),
    numGroups(0),
    stackDepth(0)
{
    memset(this->groupArray, 0, sizeof(this->groupArray));
    memset(this->groupStack, 0, sizeof(this->groupStack));
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
    Begin building the scene. Must be called once before attaching 
    nSceneNode hierarchies using nSceneServer::Attach().

    @param  viewer      the viewer position and orientation
*/
void
nSceneServer::BeginScene(const matrix44& viewer)
{
    n_assert(!this->inBeginScene);

    this->numGroups = 0;
    this->stackDepth = 0;
    this->refGfxServer->SetTransform(nGfxServer2::VIEW, viewer);
    this->inBeginScene = true;
}

//------------------------------------------------------------------------------
/**
    Attach a scene node to the scene. This will simply invoke 
    nSceneNode::Attach() on the scene node hierarchy's root object.
*/
void
nSceneServer::Attach(nSceneNode* sceneNode, nRenderContext* renderContext)
{
    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(this->inBeginScene);
    sceneNode->Attach(this, renderContext);
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
    This will render the scene which has been built between BeginScene()
    and EndScene().
*/
void
nSceneServer::RenderScene()
{
    // empty
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
    n_assert(this->stackDepth < MAX_HIERARCHY_DEPTH);
    n_assert(this->numGroups < MAX_GROUPS);

    // initialize new group node
    Group* group = &(this->groupArray[this->numGroups]);
    group->sceneNode = sceneNode;
    group->renderContext = renderContext;
    bool isTopLevel;
    if (0 == this->stackDepth)
    {
        group->parent = 0;
        isTopLevel = true;
    }
    else
    {
        group->parent = this->groupStack[this->stackDepth - 1];
        isTopLevel = false;
    }

    // push pointer to group onto hierarchy stack
    this->groupStack[this->stackDepth] = group;
    ++this->stackDepth;
    ++this->numGroups;

    // immediately call the scene node's RenderTransform method
    if (isTopLevel)
    {
        matrix44 topMatrix = renderContext->GetTransform() * this->refGfxServer->GetTransform(nGfxServer2::INVVIEW);
        sceneNode->RenderTransform(this, renderContext, topMatrix);
    }
    else
    {
        sceneNode->RenderTransform(this, renderContext, group->parent->modelView);
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

    
