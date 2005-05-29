//------------------------------------------------------------------------------
//  ncharskinrenderer.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "character/ncharskinrenderer.h"
#include "gfx2/nshader2.h"
#include "deformers/nskinmeshdeformer.h"

//------------------------------------------------------------------------------
/**
*/
nCharSkinRenderer::nCharSkinRenderer() :
    initialized(false),
    useCpuSkinning(false),
    inBegin(false),
    charSkeleton(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCharSkinRenderer::~nCharSkinRenderer()
{
    if (this->refDstMesh.isvalid())
    {
        this->refDstMesh->Release();
    }
}

//------------------------------------------------------------------------------
/**
    This initialized the character skin renderer. Skinning can happen
    in the vertex shader, or on cpu. 
*/
bool
nCharSkinRenderer::Initialize(bool cpuSkinning, nMesh2* srcMesh)
{
    n_assert(!this->initialized);
    n_assert(srcMesh);
    n_assert(srcMesh->HasAllVertexComponents(nMesh2::Weights | nMesh2::JIndices));

    this->initialized = true;
    this->useCpuSkinning = cpuSkinning;
    this->refSrcMesh = srcMesh;
    this->charSkeleton = 0;

    if (this->refDstMesh.isvalid())
    {
        this->refDstMesh->Release();
    }

    this->Setup();
    return true;
}

//------------------------------------------------------------------------------
/**
    Internal setup function. This is only needed when cpu skinning
    is used.
*/
void
nCharSkinRenderer::Setup()
{
    // create a destination mesh, if cpu skinning used
    if (this->useCpuSkinning)
    {
        nMesh2* srcMesh = this->refSrcMesh;

        n_assert(srcMesh->GetUsage() & nMesh2::ReadOnly);
        nString dstMeshName = srcMesh->GetName();
        dstMeshName.Append("_csr");

        // FIXME: redundant data: index buffer and edge buffer
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        nMesh2* dstMesh = gfxServer->NewMesh(dstMeshName.Get());
        if (!dstMesh->IsLoaded())
        {
            // if the mesh hasn't been initialized yet, do it now
            int numVertices = srcMesh->GetNumVertices();
            int numIndices  = srcMesh->GetNumIndices();
            int numEdges    = srcMesh->GetNumEdges();

            dstMesh->SetUsage(nMesh2::WriteOnly);
            dstMesh->SetVertexComponents(srcMesh->GetVertexComponents() & ~(nMesh2::Weights|nMesh2::JIndices));
            dstMesh->SetNumVertices(numVertices);
            dstMesh->SetNumIndices(numIndices);
            dstMesh->SetNumEdges(numEdges);
            dstMesh->Load();

            // copy over the index and edge data
            if (numIndices > 0)
            {
                void* srcIndices = srcMesh->LockIndices();
                void* dstIndices = dstMesh->LockIndices();
                memcpy(dstIndices, srcIndices, srcMesh->GetIndexBufferByteSize());
                dstMesh->UnlockIndices();
                srcMesh->UnlockIndices();
            }
            if (numEdges > 0)
            {
                void* srcEdges = srcMesh->LockEdges();
                void* dstEdges = dstMesh->LockEdges();
                memcpy(dstEdges, srcEdges, srcMesh->GetEdgeBufferByteSize());
                dstMesh->UnlockEdges();
                srcMesh->UnlockEdges();
            }
            dstMesh->SetState(nResource::Valid);
            this->refDstMesh = dstMesh;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCharSkinRenderer::Begin(const nCharSkeleton* skel)
{
    n_assert(!this->inBegin);
    n_assert(skel);

    this->charSkeleton = skel;
    this->inBegin = true;
    if (this->useCpuSkinning)
    {
        if (!this->refDstMesh->IsValid())
        {
            this->Setup();
        }
        nGfxServer2::Instance()->SetMesh(this->refDstMesh);
    }
    else
    {
        nGfxServer2::Instance()->SetMesh(this->refSrcMesh);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCharSkinRenderer::End()
{
    n_assert(this->inBegin);
    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nCharSkinRenderer::Render(int meshGroupIndex, nCharJointPalette& jointPalette)
{
    n_assert(inBegin);

    if (this->useCpuSkinning)
    {
        this->RenderCpuSkinning(meshGroupIndex, jointPalette);
    }
    else
    {
        this->RenderShaderSkinning(meshGroupIndex, jointPalette);
    }
}

//------------------------------------------------------------------------------
/**
    Render the skinned character with CPU skinning.
*/
void
nCharSkinRenderer::RenderCpuSkinning(int meshGroupIndex, nCharJointPalette& jointPalette)
{
    // setup a skin deformer and perform deformation
    nSkinMeshDeformer skinDeformer;
    skinDeformer.SetInputMesh(this->refSrcMesh);
    skinDeformer.SetOutputMesh(this->refDstMesh);
    const nMeshGroup& meshGroup = this->refSrcMesh->GetGroup(meshGroupIndex);
    skinDeformer.SetStartVertex(meshGroup.GetFirstVertex());
    skinDeformer.SetNumVertices(meshGroup.GetNumVertices());
    skinDeformer.SetCharSkeleton(this->charSkeleton);
    skinDeformer.SetJointPalette(&jointPalette);
    skinDeformer.Compute();

    // render the deformed mesh
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->SetVertexRange(meshGroup.GetFirstVertex(), meshGroup.GetNumVertices());
    gfxServer->SetIndexRange(meshGroup.GetFirstIndex(), meshGroup.GetNumIndices());
    gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
}

//------------------------------------------------------------------------------
/**
    Render the skinned character with vertex shader skinning.
*/
void
nCharSkinRenderer::RenderShaderSkinning(int meshGroupIndex, nCharJointPalette& jointPalette)
{
    static const int maxJointPaletteSize = 72;
    static matrix44 jointArray[maxJointPaletteSize];
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // extract the current joint palette from the skeleton in the
    // right format for the skinning shader
    int paletteSize = jointPalette.GetNumJoints();
    n_assert(paletteSize <= maxJointPaletteSize);
    int paletteIndex;
    for (paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
    {
        const nCharJoint& joint = this->charSkeleton->GetJointAt(jointPalette.GetJointIndexAt(paletteIndex));
        jointArray[paletteIndex] = joint.GetSkinMatrix44();
    }

    // transfer the joint palette to the current shader
    nShader2* shd = gfxServer->GetShader();
    n_assert(shd);
    if (shd->IsParameterUsed(nShaderState::JointPalette))
    {
        shd->SetMatrixArray(nShaderState::JointPalette, jointArray, paletteSize);
    }

    // set current vertex and index range and draw mesh
    const nMeshGroup& meshGroup = this->refSrcMesh->GetGroup(meshGroupIndex);
    gfxServer->SetVertexRange(meshGroup.GetFirstVertex(), meshGroup.GetNumVertices());
    gfxServer->SetIndexRange(meshGroup.GetFirstIndex(), meshGroup.GetNumIndices());
    gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
}
