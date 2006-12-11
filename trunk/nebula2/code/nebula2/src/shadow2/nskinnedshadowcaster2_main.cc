//------------------------------------------------------------------------------
//  nskinnedshadowcaster2_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "shadow2/nskinnedshadowcaster2.h"
#include "shadow2/nshadowserver2.h"

nNebulaClass(nSkinnedShadowCaster2, "nshadowcaster2");

//------------------------------------------------------------------------------
/**
*/
nSkinnedShadowCaster2::nSkinnedShadowCaster2() :
    charSkeleton(0),
    charSkeletonDirty(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSkinnedShadowCaster2::~nSkinnedShadowCaster2()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    This initializes all mesh resources needed by nSkinnedShadowCaster2.
*/
bool
nSkinnedShadowCaster2::LoadResource()
{
    n_assert(!this->IsLoaded());
    n_assert(!this->refIndexBuffer.isvalid());
    n_assert(!this->refBindPoseMesh.isvalid());
    n_assert(!this->refSkinnedMesh.isvalid());
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // load the bind pose mesh, this will reside in system memory and
    // will ever only be read by the CPU as the source for skinning
    nMesh2* bindPoseMesh = gfxServer->NewMesh(this->GetFilename());
    this->refBindPoseMesh = bindPoseMesh;
    if (!bindPoseMesh->IsLoaded())
    {
        bindPoseMesh->SetFilename(this->GetFilename());
        bindPoseMesh->SetUsage(nMesh2::ReadOnly);
        if (!bindPoseMesh->Load())
        {
            n_error("Error loading shadow skin mesh '%s'!", this->GetFilename().Get());
            bindPoseMesh->Release();
            return false;
        }
    }

    // create the skinned mesh, this will be filled by the CPU with skinned
    // data, properly layed out for shadow volume extrusion (which means
    // vertices will be duplicated)
    nString skinnedMeshName = this->GetFilename() + "s";
    nMesh2* skinnedMesh = gfxServer->NewMesh(skinnedMeshName);
    this->refSkinnedMesh = skinnedMesh;
    if (!skinnedMesh->IsLoaded())
    {
        skinnedMesh->SetVertexUsage(nMesh2::ReadWrite | nMesh2::NeedsVertexShader);
        skinnedMesh->SetIndexUsage(nMesh2::ReadOnly);
        skinnedMesh->SetVertexComponents(nMesh2::Coord4);
        skinnedMesh->SetNumVertices(bindPoseMesh->GetNumVertices() * 2);
        skinnedMesh->SetNumIndices(bindPoseMesh->GetNumIndices());
        skinnedMesh->SetNumEdges(bindPoseMesh->GetNumEdges());
        int numGroups = bindPoseMesh->GetNumGroups();
        skinnedMesh->SetNumGroups(numGroups);
        int groupIndex;
        for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
        {
            nMeshGroup group = bindPoseMesh->Group(groupIndex);
            group.SetFirstVertex(group.GetFirstVertex() * 2);
            group.SetNumVertices(group.GetNumVertices() * 2);
            skinnedMesh->Group(groupIndex) = group;
        }
        skinnedMesh->Load();

        // transfer vertices from bindPoseMesh and duplicate them on
        // the way (one version for the un-extruded and one for the
        // extruded position)
        int numVertices = bindPoseMesh->GetNumVertices();
        int srcVertexWidth = bindPoseMesh->GetVertexWidth();
        float* srcVertices = bindPoseMesh->LockVertices();
        float* dstVertices = skinnedMesh->LockVertices();
        int vertexIndex;
        for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
        {
            int srcOffset = vertexIndex * srcVertexWidth;

            // write unextruded vertex
            *dstVertices++ = srcVertices[srcOffset];
            *dstVertices++ = srcVertices[srcOffset + 1];
            *dstVertices++ = srcVertices[srcOffset + 2];
            *dstVertices++ = 0.0f;

            // write extruded vertex
            *dstVertices++ = srcVertices[srcOffset];
            *dstVertices++ = srcVertices[srcOffset + 1];
            *dstVertices++ = srcVertices[srcOffset + 2];
            *dstVertices++ = 1.0f;
        }
        bindPoseMesh->UnlockVertices();
        skinnedMesh->UnlockVertices();

        // transfer indices into skinned mesh and fix them according to the new vertex layout
        int numIndices = bindPoseMesh->GetNumIndices();
        ushort* srcIndices = bindPoseMesh->LockIndices();
        ushort* dstIndices = skinnedMesh->LockIndices();
        int ii;
        for (ii = 0; ii < numIndices; ii++)
        {
            dstIndices[ii] = srcIndices[ii] * 2;
        }
        bindPoseMesh->UnlockIndices();
        skinnedMesh->UnlockIndices();

        // transfer indices into skinned mesh and fix their vertex indices according to the new vertex layout
        int numEdges = bindPoseMesh->GetNumEdges();
        nMesh2::Edge* srcEdges = bindPoseMesh->LockEdges();
        nMesh2::Edge* dstEdges = skinnedMesh->LockEdges();
        int edgeIndex;
        for (edgeIndex = 0; edgeIndex < numEdges; edgeIndex++)
        {
            dstEdges[edgeIndex] = srcEdges[edgeIndex];
            dstEdges[edgeIndex].vIndex[0] *= 2;
            dstEdges[edgeIndex].vIndex[1] *= 2;
        }
        bindPoseMesh->UnlockEdges();
        skinnedMesh->UnlockEdges();
    }

    // initialize parent class buffers
    this->AllocateBuffers(this->refSkinnedMesh);

    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload resources.
*/
void
nSkinnedShadowCaster2::UnloadResource()
{
    n_assert(Unloaded != this->GetState());

    // unload the skinned mesh
    if (this->refSkinnedMesh.isvalid())
    {
        this->refSkinnedMesh->Release();
        this->refSkinnedMesh.invalidate();
    }

    // unload the bind pose mesh
    if (this->refBindPoseMesh.isvalid())
    {
        this->refBindPoseMesh->Release();
        this->refBindPoseMesh.invalidate();
    }

    // unload parent class buffers
    this->ReleaseBuffers();

    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
    Setup the shadow volume for this caster. This method is called by
    nShadowServer2 right before rendering the shadow volume. Implement this
    method in a derived class.
*/
void
nSkinnedShadowCaster2::SetupShadowVolume(const nLight& light, const matrix44& invModelLightMatrix)
{
    nShadowServer2* shadowServer = nShadowServer2::Instance();
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // FIXME: can we optimize all of this more through some sort of lazy evaluation?

    if (this->charSkeletonDirty)
    {
        this->UpdateSkinning();
        this->UpdateFaceNormalsAndMidpoints(this->refSkinnedMesh);
    }

    // split faces into lit and unlit
    if (this->charSkeletonDirty || this->LightingChanged(light, invModelLightMatrix))
    {
        this->UpdateFaceLitFlags(light, invModelLightMatrix);
    }

    // write shadow volume indices
    if (shadowServer->GetUseZFail())
    {
        this->BeginWriteIndices(this->refSkinnedMesh);
        this->WriteSideIndices();
        this->WriteCapIndices(true);
        this->WriteCapIndices(false);
        this->EndWriteIndices();
    }
    else
    {
        this->BeginWriteIndices(this->refSkinnedMesh);
        this->WriteSideIndices();
        this->EndWriteIndices();
    }

    // prepare for rendering
    gfxServer->SetMesh(this->refSkinnedMesh, this->refIndexBuffer);
    const nMeshGroup& meshGroup = this->refSkinnedMesh->Group(this->meshGroupIndex);
    gfxServer->SetVertexRange(meshGroup.GetFirstVertex(), meshGroup.GetNumVertices());
    gfxServer->SetIndexRange(0, this->GetNumDrawIndices());

    // not forgetting: resetting the dirty flags!
    this->charSkeletonDirty = false;
}

//------------------------------------------------------------------------------
/**
    Actually render the shadow volume. This can be called 1 or 2 times by
    the shadow server, depending on whether 2-sided stencil operations are
    available.
*/
void
nSkinnedShadowCaster2::RenderShadowVolume()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
}

//------------------------------------------------------------------------------
/**
    This method performs the actual skinning on the CPU and writes the
    skinned vertices into the refSkinnedMesh. A valid and uptodate character
    skeleton must be set through

    FIXME: performance optimization: convert float joint indices to integer
    during load time?
*/
void
nSkinnedShadowCaster2::UpdateSkinning()
{
    n_assert(this->charSkeleton);

    nMesh2* srcMesh = this->refBindPoseMesh;
    nMesh2* dstMesh = this->refSkinnedMesh;
    const nMeshGroup& srcGroup = srcMesh->Group(this->meshGroupIndex);
    const nMeshGroup& dstGroup = dstMesh->Group(this->meshGroupIndex);
    int numSrcVertices = srcGroup.GetNumVertices();
    int numDstVertices = dstGroup.GetNumVertices();
    n_assert((2 * numSrcVertices) == numDstVertices);
    n_assert(srcMesh->GetVertexWidth() == 11); // COORD(3) + WEIGHTS(4) + JINDICES(4) = 11 floats

    float* srcVertices = srcMesh->LockVertices() + srcGroup.GetFirstVertex();
    float* dstVertices = dstMesh->LockVertices() + dstGroup.GetFirstVertex();
    vector3 v;
    int index;
    for (index = 0; index < numSrcVertices; index++)
    {
        // perform weighted skinning
        const vector3& srcVec = *((vector3*)srcVertices);
        float* weights = &srcVertices[3];
        float* indices = &srcVertices[7];
        v.set(0.0f, 0.0f, 0.0f);
        int i;
        for (i = 0; i < 4; i++)
        {
            if (weights[i] > 0.0f)
            {
                const matrix44& skinMatrix = this->charSkeleton->GetJointAt(int(indices[i])).GetSkinMatrix44();
                skinMatrix.weighted_madd(srcVec, v, weights[i]);
            }
        }

        // write skinned vertex to unextruded and extruded vertex position, note, we are filling
        // a dynamic vertex buffer which will be discarded after rendering, so we NEED to write
        // seemingly constant data (the extrude weights) as well!
        *dstVertices++ = v.x; *dstVertices++ = v.y; *dstVertices++ = v.z; *dstVertices++ = 0.0f;
        *dstVertices++ = v.x; *dstVertices++ = v.y; *dstVertices++ = v.z; *dstVertices++ = 1.0f;

        // set to next src vertex
        srcVertices += 11;
    }
    dstMesh->UnlockVertices();
    srcMesh->UnlockVertices();
}

