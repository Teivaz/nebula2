//------------------------------------------------------------------------------
//  nblendshapedeformer.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "deformers/nblendshapedeformer.h"

//------------------------------------------------------------------------------
/**
*/
nBlendShapeDeformer::nBlendShapeDeformer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nBlendShapeDeformer::~nBlendShapeDeformer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Perform the actual blend shape deformation.
*/
void
nBlendShapeDeformer::Compute()
{
/*
    n_assert(this->refInputMesh.Size() == this->weightArray.Size());
    n_assert(this->refOutputMesh.isvalid());

    // for each input mesh...
    int targetIndex;
    int numTargets = this->refInputMeshes.Size();
    for (targetIndex = 0; targetIndex < numTargets; targetIndex++)
    {
        nMesh2* srcMesh = this->refInputMeshes[targetIndex];
        nMesh2* dstMesh = this->refOutputMesh;
        float curWeight = this->weightArray[targetIndex];

        int numVertices = srcMesh->GetNumVertices();
        n_assert(dstMesh->GetNumVertices() == numVertices);
        n_assert(dstMesh->GetVertexComponents() == srcMesh->GetVertexComponents());
        float* srcPtr = srcMesh->LockVertices() + this->startVertex * srcMesh->GetVertexWidth();
        float* dstPtr = dstMesh->LockVertices() + this->startVertex * dstMesh->GetVertexWidth();
        int index = 0;
        int numFloats = this->numVertices * srcMesh->GetVertexWidth();

        if (0 == targetIndex)
        {
            for (index = 0; index < numFloats; index++)
            {
                float val = *srcPtr++ * curWeight;
                *dstPtr++ = val;
            }
        }
        else
        {
            // FIXME: hmm, reading back from a D3DPOOL_DEFAULT mesh may be very slow...
            for (index = 0; index < numFloats; index++)
            {
                float val = *srcPtr++ * curWeight;
                *dstPtr++ += val;
            }
        }
        dstMesh->UnlockVertices();
        srcMesh->UnlockVertices();
    }
*/
}
