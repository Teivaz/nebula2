//------------------------------------------------------------------------------
//  nsceneserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nsceneserver.h"
#include "scene/nscenenode.h"
#include "scene/nrendercontext.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "kernel/ntimeserver.h"

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
nSceneServer::IsShaderUsed(uint fourcc) const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Begin building the scene. Must be called once before attaching 
    nSceneNode hierarchies using nSceneServer::Attach().

    @param  viewer      the viewer position and orientation
*/
void
nSceneServer::BeginScene(const matrix44& invView)
{
    n_assert(!this->inBeginScene);

    this->numGroups = 0;
    this->stackDepth = 0;
    matrix44 view = invView;
    view.invert_simple();
    this->refGfxServer->SetTransform(nGfxServer2::View, view);
    this->inBeginScene = true;
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
    n_assert(this->numGroups < MaxGroups);

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
        matrix44 topMatrix = renderContext->GetTransform();
        sceneNode->RenderTransform(this, renderContext, topMatrix);
    }
    else
    {
        sceneNode->RenderTransform(this, renderContext, group->parent->modelTransform);
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
    if (shd->IsParameterUsed(nShader2::View))
    {
        shd->SetMatrix(nShader2::View, gfxServer->GetTransform(nGfxServer2::View));
    }
    if (shd->IsParameterUsed(nShader2::Model))
    {
        shd->SetMatrix(nShader2::Model, gfxServer->GetTransform(nGfxServer2::Model));
    }
    if (shd->IsParameterUsed(nShader2::Projection))
    {
        shd->SetMatrix(nShader2::Projection, gfxServer->GetTransform(nGfxServer2::Projection));
    }
    if (shd->IsParameterUsed(nShader2::ModelView))
    {
        shd->SetMatrix(nShader2::ModelView, gfxServer->GetTransform(nGfxServer2::ModelView));
    }
    if (shd->IsParameterUsed(nShader2::ModelViewProjection))
    {
        shd->SetMatrix(nShader2::ModelViewProjection, gfxServer->GetTransform(nGfxServer2::ModelViewProjection));
    }
    if (shd->IsParameterUsed(nShader2::ModelEyePos))
    {
        shd->SetVector3(nShader2::ModelEyePos, invModelView.pos_component());
    }
    if (shd->IsParameterUsed(nShader2::InvModelView))
    {
        shd->SetMatrix(nShader2::InvModelView, gfxServer->GetTransform(nGfxServer2::InvModelView));
    }
    if (shd->IsParameterUsed(nShader2::Time))
    {
        nTime time = this->kernelServer->GetTimeServer()->GetTime();
        shd->SetFloat(nShader2::Time, float(time));
    }
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
