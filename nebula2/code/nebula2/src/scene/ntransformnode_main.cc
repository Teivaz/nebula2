#define N_IMPLEMENTS nTransformNode
//------------------------------------------------------------------------------
//  ntransformnode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ntransformnode.h"
#include "scene/nsceneserver.h"

nNebulaScriptClass(nTransformNode, "nscenenode");

//------------------------------------------------------------------------------
/**
*/
nTransformNode::nTransformNode() :
    scale(1.0f, 1.0f, 1.0f),
    transformFlags(ACTIVE)
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
    if (this->CheckFlags(ACTIVE))
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
void
nTransformNode::RenderTransform(nSceneServer* sceneServer, 
                                nRenderContext* renderContext, 
                                const matrix44& parentMatrix)
{
    n_assert(sceneServer);
    n_assert(renderContext);

    // call transform animators
    this->InvokeTransformAnimators(renderContext);

    // update local matrix
    this->UpdateMatrix();

    // local matrix to modelview matrix
    if (this->CheckFlags(VIEWSPACE))
    {
        sceneServer->SetModelView(this->matrix);
    }
    else
    {
        sceneServer->SetModelView(this->matrix * parentMatrix);
    }
}
