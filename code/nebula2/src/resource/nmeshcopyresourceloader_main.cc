#define N_IMPLEMENTS nMeshCopyResourceLoader
//------------------------------------------------------------------------------
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "resource/nmeshcopyresourceloader.h"

nNebulaClass(nMeshCopyResourceLoader, "resource::nresourceloader");

//------------------------------------------------------------------------------
/**
*/
nMeshCopyResourceLoader::nMeshCopyResourceLoader():
    reloaded(false)
{
}

//------------------------------------------------------------------------------
/**
*/
nMeshCopyResourceLoader::~nMeshCopyResourceLoader()
{
}

//------------------------------------------------------------------------------
/**
*/
bool nMeshCopyResourceLoader::InitResource(const char* sFilename, nResource* callingResource)
{
    n_assert(this->srcMesh.isvalid());
    n_assert(callingResource);
    n_assert(callingResource->IsA(nKernelServer::Instance()->FindClass("nmesh2")));

    nMesh2* dstMesh = (nMesh2*) callingResource;
    nMesh2* srcMesh = this->srcMesh.get();

    if (!dstMesh->IsLoaded())
    {
        dstMesh->SetNumVertices(srcMesh->GetNumVertices());
        dstMesh->SetNumIndices(srcMesh->GetNumIndices());
        dstMesh->SetNumEdges(srcMesh->GetNumEdges());
        int numGroups = srcMesh->GetNumGroups();
        dstMesh->SetNumGroups(numGroups);
        int groupIndex;
        for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
        {
            dstMesh->Group(groupIndex) = srcMesh->Group(groupIndex);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    @param const char*       path             the full path to the to-be-loaded file
    @param const nResource*  callingResource  ptr to the nResource calling nResourceLoader::Load()

    @return bool                               success/failure
*/
bool nMeshCopyResourceLoader::Load(const char* sFilename, nResource* callingResource)
{
    n_assert(this->srcMesh.isvalid());
    n_assert(callingResource);
    n_assert(callingResource->IsA(nKernelServer::Instance()->FindClass("nmesh2")));

    nMesh2* dstMesh = (nMesh2*) callingResource;
    nMesh2* srcMesh = this->srcMesh.get();


    // transfer vertices from source mesh
    int numVertices = srcMesh->GetNumVertices();
    int srcVertexWidth = srcMesh->GetVertexWidth();
    int dstVertexWidth = dstMesh->GetVertexWidth();
    float* srcVertices = srcMesh->LockVertices();
    float* dstVertices = dstMesh->LockVertices();
    int vertexWidth = dstMesh->GetVertexWidth();
    int dstComponents = dstMesh->GetVertexComponents();
    int srcComponents = srcMesh->GetVertexComponents();
    int intComponents = dstComponents & srcComponents;

    int vertexIndex;
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        int srcOffset = vertexIndex * srcVertexWidth;
        int dstOffset = vertexIndex * dstVertexWidth;

        int compOffset = 0;
        int mask;
        for (mask = 1; mask != 0; mask = mask << 1)
        {
            int curComp = mask & intComponents;
            if (curComp)
            {
                int dstCompOffset = dstMesh->GetVertexComponentOffset((nMesh2::VertexComponent) curComp);
                int srcCompOffset = srcMesh->GetVertexComponentOffset((nMesh2::VertexComponent) curComp);
                int compWidth = dstMesh->GetVertexWidthFromMask(curComp);
                int i;
                for (i=0; i < compWidth; i++)
                {
                    dstVertices[dstOffset + dstCompOffset + i] = srcVertices[srcOffset + srcCompOffset + i];
                }
            }
        }
    }
    srcMesh->UnlockVertices();
    dstMesh->UnlockVertices();

    // transfer indices from source mesh
    int numIndices = srcMesh->GetNumIndices();
    ushort* srcIndices = srcMesh->LockIndices();
    ushort* dstIndices = dstMesh->LockIndices();
    int ii;
    for (ii = 0; ii < numIndices; ii++)
    {
        dstIndices[ii] = srcIndices[ii];
    }
    srcMesh->UnlockIndices();
    dstMesh->UnlockIndices();

    this->reloaded = true;
    return true;
}

