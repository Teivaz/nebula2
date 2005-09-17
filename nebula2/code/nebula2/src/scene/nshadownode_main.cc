//------------------------------------------------------------------------------
//  nshadownode_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nshadownode.h"
#include "shadow2/nshadowserver2.h"

nNebulaScriptClass(nShadowNode, "ntransformnode");

const float nShadowNode::maxSmallObjectDistance = 30.0f;

//------------------------------------------------------------------------------
/**
*/
nShadowNode::nShadowNode() :
    groupIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShadowNode::~nShadowNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Indicate to scene server that we provide shadow
*/
bool
nShadowNode::HasShadow() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Initializes the embedded shadow caster.
*/
bool
nShadowNode::LoadResources()
{
    if (nTransformNode::LoadResources())
    {
        nShadowServer2* shadowServer = nShadowServer2::Instance();
        nStaticShadowCaster2* shadowCaster = (nStaticShadowCaster2*) shadowServer->NewShadowCaster(nShadowCaster2::Static, 0);
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
nShadowNode::UnloadResources()
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
nShadowNode::RenderShadow(nSceneServer* /*sceneServer*/, nRenderContext* /*renderContext*/, const matrix44& modelMatrix)
{   
    // HACK:
    // check distance for small objects < smaller 3 meters diagonal)
    // should be replaced by some proper LODing!
    bool cull = false;
    if (this->GetLocalBox().diagonal_size() < 3.0f)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        const vector3& viewerPos = gfxServer->GetTransform(nGfxServer2::InvView).pos_component();
        const vector3& worldPos  = modelMatrix.pos_component();
        float dist = vector3::distance(viewerPos, worldPos);
        if (dist > maxSmallObjectDistance)
        {
            cull = true;
        }
    }

    // render the shadow volume
    if (!cull)
    {
        nShadowServer2::Instance()->RenderShadowCaster(this->refShadowCaster, modelMatrix);
    }
    return true;
}
