#define N_IMPLEMENTS nSceneNode
//------------------------------------------------------------------------------
//  nscenenode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nscenenode.h"
#include "scene/nanimator.h"

nNebulaScriptClass(nSceneNode, "nroot");

//------------------------------------------------------------------------------
/**
*/
nSceneNode::nSceneNode() :
    animatorArray(1, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode::~nSceneNode()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    This method makes sure that all resources needed by this object
    are loaded. The method does NOT recurse into its children.

    Subclasses should expect that the LoadResources() method can be 
    called on them although some or all of their resources are valid.
    Thus, a check should exist, whether the resource really needs to
    be reloaded.

    @return     true, if resource loading was successful
*/
bool
nSceneNode::LoadResources()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    This method makes sure that all resources used by this object are
    unloaded. The method does NOT recurse into its children. 

    @return     true, if resources have actually been unloaded
*/
void
nSceneNode::UnloadResources()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Returns true if all resources are valid. Subclasses should return
    false if at least one of their resources are invalid.
*/
bool
nSceneNode::AreResourcesValid() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Called by the client app when a new render context has been created for
    this scene node hierarchy. Scene node hierarchies must not contain
    frame-persistent data, since one hierarchy can be reused multiple times
    per frame. All frame-persistent data must be stored in nRenderContext 
    objects, which must be communicated to the scene node hierarchy when
    it is rendered. nRenderContext objects are the primary way to
    communicate data from the client app to a scene node hierarchy (i.e.
    time stamps, velocity, etc...).

    The RenderContextCreated() method should be called when a new
    'game object' which needs rendering has been created by the application.

    @param  renderContext   pointer to a nRenderContext object    
*/
void
nSceneNode::RenderContextCreated(nRenderContext* renderContext)
{
    n_assert(renderContext);

    nSceneNode* curChild;
    for (curChild = (nSceneNode*) this->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        curChild->RenderContextCreated(renderContext);
    }
}

//------------------------------------------------------------------------------
/**
    Called by the client app when a render context for this scene node
    hierarchy should be destroyed. This is usually the case when the
    game object associated with this scene node hierarchie goes away.

    The method will be invoked recursively on all child and depend nodes
    of the scene node object.

    @param  renderContext   pointer to a nRenderContext object    
*/
void
nSceneNode::RenderContextDestroyed(nRenderContext* renderContext)
{
    n_assert(renderContext);

    nSceneNode* curChild;
    for (curChild = (nSceneNode*) this->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        curChild->RenderContextDestroyed(renderContext);
    }
}

//------------------------------------------------------------------------------
/**
    Attach the object to the scene if necessary. This method is either
    called by the nSceneServer, or by another nSceneNode object at 
    scene construction time. If the nSceneNode needs rendering it should
    call the appropriate nSceneServer method to attach itself to the scene.

    The method will be invoked recursively on all child and depend nodes
    of the scene node object.

    @param  sceneServer     pointer to the nSceneServer object
    @param  renderContext   pointer to the nRenderContext object
    @param  caller          pointer to caller nSceneNode object, or 0 if called by nSceneServer
*/
void
nSceneNode::Attach(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);

    nSceneNode* curChild;
    for (curChild = (nSceneNode*) this->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        curChild->Attach(sceneServer, renderContext);
    }
}

//------------------------------------------------------------------------------
/**
    Render the node's transformtion. This should be implemented by a subclass.
    The method will only be called by nSceneServer if the method 
    HasTransform() returns true.
*/
void
nSceneNode::RenderTransform(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& parentMatrix)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render the node's geometry. This should be implemented by a subclass.
    The method will only be called by nSceneServer if the method 
    HasGeometry() returns true.
*/
void
nSceneNode::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render the node's shader. This should be implemented by a subclass.
    The method will only be called by nSceneServer if the method 
    HasShader() returns true.

    @param  fourcc          a fourcc code that identifies the "shader stage"
    @param  sceneServer     pointer to scene server object
    @param  renderContext   pointer to render context object
*/
void
nSceneNode::RenderShader(uint fourcc, nSceneServer* sceneServer, nRenderContext* renderContext)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render the node's volume. This should be implemented by a subclass.
    The method will only be called by nSceneServer if the method 
    HasLightVolume() returns true.
*/
void
nSceneNode::RenderLightVolume(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& lightModelView)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Return true if this node provides transformation. Should overriden 
    by subclasses.
*/
bool
nSceneNode::HasTransform() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Return true if this node provides geometry. Should overriden 
    by subclasses.
*/
bool
nSceneNode::HasGeometry() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Return true if this node provides a shader under the given fourcc code.

    @param  fourcc  a fourcc code identifying the shader
*/
bool
nSceneNode::HasShader(uint fourcc) const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Return true if this node provides light volume information. Should be 
    overriden by subclasses.
*/
bool
nSceneNode::HasLightVolume() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Add an animator object to this scene node.
*/
void
nSceneNode::AddAnimator(const char* relPath)
{
    n_assert(relPath);

    nDynAutoRef<nAnimator> newDynAutoRef;
    newDynAutoRef.set(relPath);
    this->animatorArray.PushBack(newDynAutoRef);
}

//------------------------------------------------------------------------------
/**
    Get number of animator objects.
*/
int
nSceneNode::GetNumAnimators() const
{
    return this->animatorArray.Size();
}

//------------------------------------------------------------------------------
/**
    Get path to animator object at given index.
*/
const char*
nSceneNode::GetAnimatorAt(int index)
{
    return this->animatorArray[index].getname();
}

//------------------------------------------------------------------------------
/**
    Invoke all shader animators. This method should be called classes
    which implement the RenderShader() method from inside this method.
*/
void
nSceneNode::InvokeShaderAnimators(nRenderContext* renderContext)
{
    int numAnimators = this->GetNumAnimators();
    if (numAnimators > 0)
    {
        n_assert(renderContext);

        kernelServer->PushCwd(this);
        int i;
        for (i = 0; i < numAnimators; i++)
        {
            nAnimator* animator = this->animatorArray[i].get();
            if (nAnimator::SHADER == animator->GetAnimatorType())
            {
                animator->Animate(this, renderContext);
            }
        }
        kernelServer->PopCwd();
    }
}

//------------------------------------------------------------------------------
/**
    Invoke all transform animators. This method should be called classes
    which implement the RenderTransform() method from inside this method.
*/
void
nSceneNode::InvokeTransformAnimators(nRenderContext* renderContext)
{
    int numAnimators = this->GetNumAnimators();
    if (numAnimators > 0)
    {
        n_assert(renderContext);

        kernelServer->PushCwd(this);
        int i;
        for (i = 0; i < numAnimators; i++)
        {
            nAnimator* animator = this->animatorArray[i].get();
            if (nAnimator::TRANSFORM == animator->GetAnimatorType())
            {
                animator->Animate(this, renderContext);
            }
        }
        kernelServer->PopCwd();
    }
}






