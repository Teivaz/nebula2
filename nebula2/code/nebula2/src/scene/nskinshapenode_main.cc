//------------------------------------------------------------------------------
//  nskinshapenode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nskinshapenode.h"
#include "scene/nskinanimator.h"

nNebulaScriptClass(nSkinShapeNode, "nshapenode");

//------------------------------------------------------------------------------
/**
*/
nSkinShapeNode::nSkinShapeNode() :
    extCharSkeleton(0)
{
    this->SetMeshUsage(nMesh2::WriteOnce | nMesh2::NeedsVertexShader);
}

//------------------------------------------------------------------------------
/**
*/
nSkinShapeNode::~nSkinShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Attach to the scene server.
*/
void
nSkinShapeNode::Attach(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    // call my skin animator (updates the char skeleton pointer)
    kernelServer->PushCwd(this);
    if (this->refSkinAnimator.isvalid())
    {
        this->refSkinAnimator->Animate(this, renderContext);
    }
    kernelServer->PopCwd();

    nShapeNode::Attach(sceneServer, renderContext);
}

//------------------------------------------------------------------------------
/**
*/
bool
nSkinShapeNode::ApplyGeometry(nSceneServer* sceneServer)
{
    if (!this->charSkinRenderer.IsInitialized())
    {
        // never use cpu skinning
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        this->charSkinRenderer.Initialize(false, this->refMesh);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This overrides the standard nShapeNode geometry rendering. Instead
    of setting the mesh directly in the gfx server, the embedded
    nCharSkinRenderer is asked to do the rendering for us.

    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
*/
bool
nSkinShapeNode::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);

    // call my skin animator (updates the char skeleton pointer)
    kernelServer->PushCwd(this);
    if (this->refSkinAnimator.isvalid())
    {
        this->refSkinAnimator->Animate(this, renderContext);
    }
    kernelServer->PopCwd();

    // render the skin in several passes (one per skin fragment)
    this->charSkinRenderer.Begin(this->extCharSkeleton);
    int numFragments = this->GetNumFragments();
    int fragIndex;
    for (fragIndex = 0; fragIndex < numFragments; fragIndex++)
    {
        Fragment& fragment = this->fragmentArray[fragIndex];
        charSkinRenderer.Render(fragment.GetMeshGroupIndex(), fragment.GetJointPalette());
    }
    this->charSkinRenderer.End();
    return true;
}

//------------------------------------------------------------------------------
/**
    Set relative path to the skin animator object.
*/
void
nSkinShapeNode::SetSkinAnimator(const char* path)
{
    n_assert(path);
    this->refSkinAnimator = path;
}

//------------------------------------------------------------------------------
/**
Get the pointer to the currently-set character skeleton
*/
const nCharSkeleton*
nSkinShapeNode::GetCharSkeleton()
{
    return this->extCharSkeleton;
}

//------------------------------------------------------------------------------
/**
    Get relative path to the skin animator object
*/
const char*
nSkinShapeNode::GetSkinAnimator() const
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
nSkinShapeNode::SetCharSkeleton(const nCharSkeleton* charSkeleton)
{
    n_assert(charSkeleton);
    this->extCharSkeleton = charSkeleton;
}

//------------------------------------------------------------------------------
/**
    Begin defining mesh fragment. A skin mesh may be divided into several
    fragments to account for gfx hardware which an only render a skinned
    mesh with a limited number of influence objects (joints).
*/
void
nSkinShapeNode::BeginFragments(int num)
{
    n_assert(num > 0);
    this->fragmentArray.SetFixedSize(num);
}

//------------------------------------------------------------------------------
/**
    Set the mesh group index for a skin fragment.
*/
void
nSkinShapeNode::SetFragGroupIndex(int fragIndex, int meshGroupIndex)
{
    this->fragmentArray[fragIndex].SetMeshGroupIndex(meshGroupIndex);
}

//------------------------------------------------------------------------------
/**
    Get the mesh group index for a skin fragment.
*/
int
nSkinShapeNode::GetFragGroupIndex(int fragIndex) const
{
    return this->fragmentArray[fragIndex].GetMeshGroupIndex();
}

//------------------------------------------------------------------------------
/**
    Begin defining the joint palette of a skin fragment.
*/
void
nSkinShapeNode::BeginJointPalette(int fragIndex, int numJoints)
{
    this->fragmentArray[fragIndex].GetJointPalette().BeginJoints(numJoints);
}

//------------------------------------------------------------------------------
/**
    Add up to 8 joints to a fragments joint palette starting at a given
    palette index.
*/
void
nSkinShapeNode::SetJointIndices(int fragIndex, int paletteIndex, int ji0, int ji1, int ji2, int ji3, int ji4, int ji5, int ji6, int ji7)
{
    nCharJointPalette& pal = this->fragmentArray[fragIndex].GetJointPalette();
    int numJoints = pal.GetNumJoints();
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji0);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji1);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji2);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji3);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji4);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji5);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji6);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji7);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinShapeNode::SetJointIndex(int fragIndex, int paletteIndex, int jointIndex)
{
    nCharJointPalette& pal = this->fragmentArray[fragIndex].GetJointPalette();
    pal.SetJointIndex(paletteIndex, jointIndex);
}

//------------------------------------------------------------------------------
/**
    Finish defining the joint palette of a skin fragment.
*/
void
nSkinShapeNode::EndJointPalette(int fragIndex)
{
    this->fragmentArray[fragIndex].GetJointPalette().EndJoints();
}

//------------------------------------------------------------------------------
/**
    Finish defining fragments.
*/
void
nSkinShapeNode::EndFragments()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Get number of fragments.
*/
int
nSkinShapeNode::GetNumFragments() const
{
    return this->fragmentArray.Size();
}

//------------------------------------------------------------------------------
/**
    Get joint palette size of a skin fragment.
*/
int
nSkinShapeNode::GetJointPaletteSize(int fragIndex) const
{
    return this->fragmentArray[fragIndex].GetJointPalette().GetNumJoints();
}

//------------------------------------------------------------------------------
/**
    Get a joint index from a fragment's joint index.
*/
int
nSkinShapeNode::GetJointIndex(int fragIndex, int paletteIndex) const
{
    return this->fragmentArray[fragIndex].GetJointPalette().GetJointIndexAt(paletteIndex);
}

