//------------------------------------------------------------------------------
//  nshadowskinshapenode_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nshadowskinshapenode.h"
#include "scene/nskinanimator.h"

nNebulaScriptClass(nShadowSkinShapeNode, "ntransformnode");

// HACK!
const float nShadowSkinShapeNode::maxDistance = 20.0f;

//------------------------------------------------------------------------------
/**
*/
nShadowSkinShapeNode::nShadowSkinShapeNode()
:   groupIndex(0)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nShadowSkinShapeNode::~nShadowSkinShapeNode()
{
    if (this->refShadowCaster.isvalid())
    {
        this->refShadowCaster->Release();
        this->refShadowCaster.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Unload mesh resource if valid.
*/
void
nShadowSkinShapeNode::UnloadShadowCaster()
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
nShadowSkinShapeNode::LoadShadowCaster()
{
    nSkinShadowCaster* shadowCaster = (nSkinShadowCaster*) nShadowServer::Instance()->NewShadowCaster(nShadowServer::Skin, this->GetMesh());
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
nShadowSkinShapeNode::SetMesh(const char* name)
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
nShadowSkinShapeNode::LoadResources()
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
nShadowSkinShapeNode::UnloadResources()
{
    nTransformNode::UnloadResources();
    this->UnloadShadowCaster();
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

*/
bool
nShadowSkinShapeNode::ApplyShadow(nSceneServer* /*sceneServer*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowSkinShapeNode::RenderShadow(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& modelMatrix)
{   
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // compute distance to viewer
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
        this->refShadowCaster->RenderShadow(modelMatrix, this->groupIndex);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Set relative path to the skin animator object.
*/
void
nShadowSkinShapeNode::SetSkinAnimator(const char* path)
{
    n_assert(path);
    this->refSkinAnimator = path;
}

//------------------------------------------------------------------------------
/**
    Get relative path to the skin animator object
*/
const char*
nShadowSkinShapeNode::GetSkinAnimator() const
{
    return this->refSkinAnimator.getname();
}

//------------------------------------------------------------------------------
/**
    Update the pointer to an uptodate nCharSkeleton object. This pointer
    is provided by the nSkinAnimator object and is routed to the
    nCharSkinRenderer so that the mesh can be properly deformed.
*/
void
nShadowSkinShapeNode::SetCharSkeleton(const nCharSkeleton* charSkeleton)
{
    n_assert(charSkeleton);
    this->extCharSkeleton = charSkeleton;
    // update the skeleton in the skin shadow caster
    this->refShadowCaster->SetCharSkeleton(charSkeleton);
}
