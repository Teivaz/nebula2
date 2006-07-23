//------------------------------------------------------------------------------
//  nshadowskinshapenode_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nshadowskinshapenode.h"
#include "scene/nskinanimator.h"
#include "shadow2/nshadowserver2.h"

nNebulaScriptClass(nShadowSkinShapeNode, "scene::ntransformnode");

// HACK!
const float nShadowSkinShapeNode::maxDistance = 30.0f;

//------------------------------------------------------------------------------
/**
*/
nShadowSkinShapeNode::nShadowSkinShapeNode() :
    groupIndex(0)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nShadowSkinShapeNode::~nShadowSkinShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Indicate to scene server that we provide shadow
*/
bool
nShadowSkinShapeNode::HasShadow() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Initializes the embedded shadow caster.
*/
bool
nShadowSkinShapeNode::LoadResources()
{
    if (nTransformNode::LoadResources())
    {
        nShadowServer2* shadowServer = nShadowServer2::Instance();
        nSkinnedShadowCaster2* shadowCaster = (nSkinnedShadowCaster2*) shadowServer->NewShadowCaster(nShadowCaster2::Skinned, 0);
        n_assert(!shadowCaster->IsLoaded());
        shadowCaster->SetMeshGroupIndex(this->groupIndex);
        shadowCaster->SetFilename(this->meshName);
        bool shadowCasterLoaded = shadowCaster->Load();
        n_assert(shadowCasterLoaded);
        this->refShadowCaster = shadowCaster;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Releases the embedded shadow caster.
*/
void
nShadowSkinShapeNode::UnloadResources()
{
    if (this->refShadowCaster.isvalid())
    {
        this->refShadowCaster->Release();
        this->refShadowCaster.invalidate();
    }
    nTransformNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowSkinShapeNode::RenderShadow(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& modelMatrix)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // perform some distance culling...
    const vector3& viewerPos = gfxServer->GetTransform(nGfxServer2::InvView).pos_component();
    const vector3& worldPos  = modelMatrix.pos_component();
    float dist = vector3::distance(viewerPos, worldPos);
    if (dist < maxDistance)
    {
        // render the shadow volume
        n_assert(sceneServer);
        n_assert(renderContext);

        // call my skin animator (updates the char skeleton pointer)
        kernelServer->PushCwd(this);
        if (this->refSkinAnimator.isvalid())
        {
            this->refSkinAnimator->Animate(this, renderContext);
        }
        kernelServer->PopCwd();
        nShadowServer2::Instance()->RenderShadowCaster(this->refShadowCaster, modelMatrix);
    }
    return true;
}
