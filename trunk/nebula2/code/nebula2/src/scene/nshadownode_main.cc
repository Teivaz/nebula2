//------------------------------------------------------------------------------
//  nshadownode_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nshadownode.h"

nNebulaScriptClass(nShadowNode, "ntransformnode");

const float nShadowNode::maxSmallObjectDistance = 50.0f;

//------------------------------------------------------------------------------
/**
*/
nShadowNode::nShadowNode()
:   groupIndex(0)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nShadowNode::~nShadowNode()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Unload mesh resource if valid.
*/
void
nShadowNode::UnloadShadowCaster()
{
    if (this->refShadowCaster.isvalid())
    {
        this->refShadowCaster->Release();
        this->refShadowCaster.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Setup the ShadowCaster
*/
bool
nShadowNode::LoadShadowCaster()
{
    nStaticShadowCaster* shadowCaster = (nStaticShadowCaster*) nShadowServer::Instance()->NewShadowCaster(nShadowServer::Static, this->GetMesh());
    n_assert(shadowCaster);

    if (!shadowCaster->IsValid())
    {
        shadowCaster->SetFilename(this->GetMesh());
        n_assert(shadowCaster->Load());
    }
    
    this->refShadowCaster = shadowCaster;
    return true;
}

//------------------------------------------------------------------------------
/**
    Set the resource name. The mesh resource name consists of the
    filename of the mesh.
*/
void
nShadowNode::SetMesh(const char* name)
{
    n_assert(name);
    this->UnloadShadowCaster();
    this->meshName = name;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nShadowNode::LoadResources()
{
    if (nTransformNode::LoadResources())
    {
        if (this->LoadShadowCaster())
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nShadowNode::UnloadResources()
{
    nTransformNode::UnloadResources();
    this->UnloadShadowCaster();
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

*/
bool
nShadowNode::ApplyShadow(nSceneServer* /*sceneServer*/)
{
    return this->refShadowCaster->ApplyShadow();
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowNode::RenderShadow(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& modelMatrix)
{   
    // HACK:
    // check distance for small objects < smaller 4 meters diagonal)
    // should be replaced by some proper LODing!
    bool cull = false;
    if (this->GetLocalBox().diagonal_size() < 4.0f)
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
        this->refShadowCaster->RenderShadow(modelMatrix, this->GetGroupIndex());
    }
    return true;
}
