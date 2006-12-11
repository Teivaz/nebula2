//------------------------------------------------------------------------------
//  nstaticshadowcaster2_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "shadow2/nstaticshadowcaster2.h"
#include "shadow2/nshadowserver2.h"
#include "kernel/nfileserver2.h"
#include "gfx2/nn3d2loader.h"
#include "gfx2/nnvx2loader.h"

nNebulaClass(nStaticShadowCaster2, "nshadowcaster2");

//------------------------------------------------------------------------------
/**
*/
nStaticShadowCaster2::nStaticShadowCaster2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nStaticShadowCaster2::~nStaticShadowCaster2()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    This initializes the static shadow mesh data and the data buffers used
    by nShadowCaster2.
*/
bool
nStaticShadowCaster2::LoadResource()
{
    n_assert(!this->IsLoaded());
    n_assert(!this->refIndexBuffer.isvalid());
    n_assert(!this->refMesh.isvalid());
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // create a new shared mesh object
    nMesh2* newMesh = gfxServer->NewMesh(this->GetFilename());
    this->refMesh = newMesh;
    if (!newMesh->IsLoaded())
    {
        // initialize a mesh loader object
        nMeshLoader* meshLoader = 0;
        if (this->GetFilename().CheckExtension("nvx2"))
        {
            meshLoader = n_new(nNvx2Loader);
        }
        else if (this->GetFilename().CheckExtension("n3d2"))
        {
            meshLoader = n_new(nN3d2Loader);
        }
        else
        {
            n_error("nStaticShadowCaster2::LoadResource: filetype not supported!\n");
        }
        meshLoader->SetFilename(this->GetFilename().Get());
        meshLoader->SetIndexType(nMeshLoader::Index16);
        meshLoader->SetValidVertexComponents(nMesh2::Coord);
        if (meshLoader->Open())
        {
            // initialize the mesh from the mesh loader
            newMesh->SetVertexUsage(nMesh2::WriteOnce | nMesh2::NeedsVertexShader);
            newMesh->SetIndexUsage(nMesh2::ReadOnly);
            newMesh->SetVertexComponents(nMesh2::Coord4);
            newMesh->SetNumVertices(meshLoader->GetNumVertices() * 2);
            newMesh->SetNumIndices(meshLoader->GetNumIndices());
            newMesh->SetNumEdges(meshLoader->GetNumEdges());
            int numGroups = meshLoader->GetNumGroups();
            newMesh->SetNumGroups(numGroups);
            int groupIndex;
            for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
            {
                nMeshGroup group = meshLoader->GetGroupAt(groupIndex);
                group.SetFirstVertex(group.GetFirstVertex() * 2);
                group.SetNumVertices(group.GetNumVertices() * 2);
                newMesh->Group(groupIndex) = group;
            }
            newMesh->Load();

            // read vertices into temporary buffer and expand them into mesh
            int numVertices = meshLoader->GetNumVertices();
            int vertexWidth = meshLoader->GetVertexWidth();
            int srcBufferSize = numVertices * vertexWidth * sizeof(float);
            float* srcBuffer = (float*) n_malloc(srcBufferSize);
            meshLoader->ReadVertices(srcBuffer, srcBufferSize);

            // write extruded and un-extruded vertices into destination vertex buffer
            float* dstBuffer = newMesh->LockVertices();
            int vertexIndex;
            for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                int curOffset = vertexIndex * vertexWidth;

                // write unextruded vertex
                *dstBuffer++ = srcBuffer[curOffset];
                *dstBuffer++ = srcBuffer[curOffset + 1];
                *dstBuffer++ = srcBuffer[curOffset + 2];
                *dstBuffer++ = 0.0f;

                // write extruded vertex
                *dstBuffer++ = srcBuffer[curOffset];
                *dstBuffer++ = srcBuffer[curOffset + 1];
                *dstBuffer++ = srcBuffer[curOffset + 2];
                *dstBuffer++ = 1.0f;
            }
            newMesh->UnlockVertices();
            n_free(srcBuffer);
            srcBuffer = 0;

            // read indices directly, and fix them for the new vertex layout
            ushort* indices = newMesh->LockIndices();
            meshLoader->ReadIndices(indices, newMesh->GetIndexBufferByteSize());
            int ii;
            int numIndices = newMesh->GetNumIndices();
            for (ii = 0; ii < numIndices; ii++)
            {
                indices[ii] *= 2;
            }
            newMesh->UnlockIndices();
            indices = 0;

            // read edges directly, and fix them for the new vertex layout
            nMesh2::Edge* edges = newMesh->LockEdges();
            meshLoader->ReadEdges(edges, newMesh->GetEdgeBufferByteSize());
            int edgeIndex;
            int numEdges = newMesh->GetNumEdges();
            for (edgeIndex = 0; edgeIndex < numEdges; edgeIndex++)
            {
                edges[edgeIndex].vIndex[0] *= 2;
                edges[edgeIndex].vIndex[1] *= 2;
            }
            newMesh->UnlockEdges();
            edges = 0;

            meshLoader->Close();
        }
        else
        {
            n_error("Error loading shadow mesh data!");
            return false;
        }

        // release mesh loader object
        n_delete(meshLoader);
    }

    // initialize parent class buffers
    this->AllocateBuffers(newMesh);

    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload resources.
*/
void
nStaticShadowCaster2::UnloadResource()
{
    n_assert(Unloaded != this->GetState());

    // unload shadow mesh
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
        this->refMesh.invalidate();
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
nStaticShadowCaster2::SetupShadowVolume(const nLight& light, const matrix44& invModelLightMatrix)
{
    nShadowServer2* shadowServer = nShadowServer2::Instance();
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // FIXME: can we optimize all this through some sort of lazy evaluation?

    // split faces into lit and unlit
    if (this->LightingChanged(light, invModelLightMatrix))
    {
        this->UpdateFaceLitFlags(light, invModelLightMatrix);
    }

    // write shadow volume indices
    if (shadowServer->GetUseZFail())
    {
        this->BeginWriteIndices(this->refMesh);
        this->WriteSideIndices();
        this->WriteCapIndices(true);
        this->WriteCapIndices(false);
        this->EndWriteIndices();
    }
    else
    {
        this->BeginWriteIndices(this->refMesh);
        this->WriteSideIndices();
        this->EndWriteIndices();
    }

    // prepare for rendering
    gfxServer->SetMesh(this->refMesh, this->refIndexBuffer);
    const nMeshGroup& meshGroup = this->refMesh->Group(this->meshGroupIndex);
    gfxServer->SetVertexRange(meshGroup.GetFirstVertex(), meshGroup.GetNumVertices());
    gfxServer->SetIndexRange(0, this->GetNumDrawIndices());
}

//------------------------------------------------------------------------------
/**
    Actually render the shadow volume. This can be called 1 or 2 times by
    the shadow server, depending on whether 2-sided stencil operations are
    available.
*/
void
nStaticShadowCaster2::RenderShadowVolume()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
}
