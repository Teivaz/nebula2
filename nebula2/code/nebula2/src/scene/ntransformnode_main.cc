//------------------------------------------------------------------------------
//  ntransformnode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ntransformnode.h"
#include "scene/nsceneserver.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nanimator.h"

nNebulaScriptClass(nTransformNode, "nscenenode");

//------------------------------------------------------------------------------
/**
*/
nTransformNode::nTransformNode() :
    transformFlags(Active)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTransformNode::~nTransformNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Attach to the scene server.
*/
void
nTransformNode::Attach(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    if (this->CheckFlags(Active))
    {
        sceneServer->BeginGroup(this, renderContext);
        nSceneNode::Attach(sceneServer, renderContext);
        sceneServer->EndGroup();
    }
}

//------------------------------------------------------------------------------
/**
    Indicate to scene server that we provide transformation.
*/
bool
nTransformNode::HasTransform() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Compute the resulting modelview matrix and set it in the scene
    server as current modelview matrix.
*/
bool
nTransformNode::RenderTransform(nSceneServer* sceneServer, 
                                nRenderContext* renderContext, 
                                const matrix44& parentMatrix)
{
    n_assert(sceneServer);
    n_assert(renderContext);
    
    this->InvokeAnimators(nAnimator::Transform, renderContext);
    if (this->GetLockViewer())
    {
        // handle lock to viewer
        const matrix44& viewMatrix = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView);
        matrix44 m = this->tform.getmatrix();
        m = m * parentMatrix;
        m.M41 = viewMatrix.M41;
        m.M42 = viewMatrix.M42;
        m.M43 = viewMatrix.M43;
        sceneServer->SetModelTransform(m);
    }
    else
    {
        // default case
        sceneServer->SetModelTransform(this->tform.getmatrix() * parentMatrix);
    }
    return true;
}
