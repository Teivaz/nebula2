//------------------------------------------------------------------------------
//  nskinshadowcaster_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "shadow/nskinshadowcaster.h"
#include "character/ncharskeleton.h"

nNebulaClass(nSkinShadowCaster, "nshadowcaster");

//------------------------------------------------------------------------------
/**
*/
nSkinShadowCaster::nSkinShadowCaster() :
    faceNormales(0), 
    numFaceNormales(0),
    skinnedVerticies(0), 
    numSkinnedVerticies(0),
    charSkeletonDirty(true), 
    charSkeleton(0),
    weightsAndJIndicies(0), 
    numWeightsAndJIndicies(0)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nSkinShadowCaster::~nSkinShadowCaster()
{
    if (this->IsValid())
    {
        this->UnloadResource();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinShadowCaster::RenderShadow(const matrix44& modelMatrix, int groupIndex)
{
    //if (this->charSkeletonDirty)
    //{
        this->dirty = true;
        this->UpdateSkinnedVerticies();
        this->CreateFaceNormales();
    //}

    nShadowCaster::RenderShadow(modelMatrix, groupIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinShadowCaster::LoadShadowData(nMesh2* sourceMesh)
{
    n_assert(sourceMesh);

    nShadowCaster::LoadShadowData(sourceMesh);
    
    this->LoadVerticies(sourceMesh);    
    this->LoadWeightsAndJIndicies(sourceMesh);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinShadowCaster::UnloadResource()
{
    nShadowCaster::UnloadResource();
    
    if (this->refCoordMesh.isvalid())
    {
        this->refCoordMesh->Release();
        this->refCoordMesh.invalidate();
    }

    if (0 != this->faceNormales)
    {
        n_delete_array(this->faceNormales);
        this->faceNormales = 0;
        this->numFaceNormales = 0;
    }
    
    if (0 != this->skinnedVerticies)
    {
        n_delete_array(this->skinnedVerticies);
        this->skinnedVerticies = 0;
        this->numSkinnedVerticies = 0;
    }

    if (0 != this->weightsAndJIndicies)
    {
        n_delete_array(this->weightsAndJIndicies);
        this->weightsAndJIndicies = 0;
        this->numWeightsAndJIndicies = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nSkinShadowCaster::GetNumCoords() const
{
    return this->numSkinnedVerticies;
}

//------------------------------------------------------------------------------
/**
*/
vector3*
nSkinShadowCaster::GetCoords() const
{
    n_assert(0 != this->skinnedVerticies);
    n_assert(this->numSkinnedVerticies > 0);
    return this->skinnedVerticies;
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinShadowCaster::UpdateSkinnedVerticies()
{
    n_assert(0 != this->charSkeleton);
    n_assert(this->refCoordMesh.isvalid());
    n_assert(this->refCoordMesh->IsValid());
    n_assert(this->refCoordMesh->GetNumVertices() > 0);
    n_assert(0 != this->numWeightsAndJIndicies);
    n_assert(this->refCoordMesh->GetNumVertices() == this->numWeightsAndJIndicies);

    // data source
    nMesh2* sourceMesh = this->refCoordMesh.get();
    float* sourcePtr = sourceMesh->LockVertices();
    int stride = sourceMesh->GetVertexWidth();
    n_assert(sourcePtr);

    // allocate skinned vertex data buffer, if not happened yet 
    if (0 == this->skinnedVerticies)
    {
        this->numSkinnedVerticies = sourceMesh->GetNumVertices();
        this->skinnedVerticies = n_new_array(vector3, this->numSkinnedVerticies);
    }
    n_assert(this->numSkinnedVerticies == sourceMesh->GetNumVertices());
    
    // calculate new vertex positions for current skeleton
    int v;
    for (v = 0; v < this->numSkinnedVerticies; v++)
    {
        const WeightAndJIndex& waji = this->weightsAndJIndicies[v];
        const vector3& inPos = *((vector3*)(sourcePtr + (stride * v)));
        
        vector3 pos[4];
        int i;
        float weightSum = 0.0f;
        for(i = 0; i < 4; i++)
        {
            weightSum += waji.weight[i];
            const nCharJoint& charJoint = this->charSkeleton->GetJointAt(waji.index[i]);
            charJoint.GetSkinMatrix44().mult(inPos, pos[i]);
        }            
        n_assert(weightSum > 0.99f && weightSum < 1.01f);
        const float factor = 1.0f / weightSum;
        
        vector3& outPos = this->skinnedVerticies[v];
        outPos.set(0.0, 0.0, 0.0);
        for(i = 0; i < 4; i++)
        {
            outPos += (pos[i] * (waji.weight[i] * factor) );
        }
    }
    sourceMesh->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinShadowCaster::LoadWeightsAndJIndicies(nMesh2* sourceMesh)
{
    n_assert(sourceMesh);
    n_assert(sourceMesh->IsValid());
    n_assert2(sourceMesh->GetNumVertices() > 0 && (0 != (sourceMesh->GetVertexComponents() & (nMesh2::JIndices | nMesh2::Weights))),
        "The mesh used for skinned shadow casting must have weights and JIndicies!\n");
    
    n_assert(0 == this->weightsAndJIndicies);

    // allocate data
    this->numWeightsAndJIndicies = sourceMesh->GetNumVertices();
    this->weightsAndJIndicies = n_new_array(WeightAndJIndex, this->numWeightsAndJIndicies);
    
    // vertex strides
    const int stride = sourceMesh->GetVertexWidth();
    const int weightDst = sourceMesh->GetVertexComponentDistance(nMesh2::Weights);
    const int jindicesDst = sourceMesh->GetVertexComponentDistance(nMesh2::JIndices);

    // source data
    float* sourcePtr = sourceMesh->LockVertices();
    n_assert(sourcePtr);

    int v;
    for (v = 0; v < this->numWeightsAndJIndicies; v++)
    {
        WeightAndJIndex& waji = this->weightsAndJIndicies[v];
        float* weights = sourcePtr + (stride * v) + weightDst;
        float* jindicies = sourcePtr + (stride * v) + jindicesDst;
        
        int i;
        for (i = 0; i < 4; i++)
        {
            waji.weight[i] = *(weights + i);
            waji.index[i] = (int) *(jindicies + i);
        }
    }
    sourceMesh->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinShadowCaster::LoadVerticies(nMesh2* sourceMesh)
{
    n_assert(sourceMesh);
    n_assert(sourceMesh->IsValid());
    n_assert2(sourceMesh->GetNumVertices() > 0 && (0 != (sourceMesh->GetVertexComponents() & nMesh2::Coord)),
        "The mesh used for shadow casting must have coordinates!\n");
    
    n_assert(! this->refCoordMesh.isvalid());

    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // create resource name
    nString meshName = this->GetFilename();
    meshName += "_c";

    // get mesh
    nMesh2* destMesh = gfxServer->NewMesh(meshName.Get());
    this->refCoordMesh = destMesh;
    
    int numVertices = sourceMesh->GetNumVertices();
    if (! destMesh->IsValid() ) // is the mesh already loaded?
    {        
        destMesh->SetUsage(nMesh2::ReadOnly);
        destMesh->SetVertexComponents(nMesh2::Coord);
        destMesh->SetNumVertices(numVertices);
        
        destMesh->SetNumIndices(0);
        destMesh->SetNumEdges(0);
        destMesh->SetAsyncEnabled(false);
        destMesh->SetRefillBuffersMode(nMesh2::Enabled);
        
        bool success = destMesh->Load();
        n_assert(success);
    }

    // load source data
    if (nMesh2::NeededNow == destMesh->GetRefillBuffersMode())
    {
        // source ptr
        float* sourcePtr = sourceMesh->LockVertices();
        n_assert(sourcePtr);
        ushort stride = sourceMesh->GetVertexWidth();
        
        // dest ptr
        float* destPtr = destMesh->LockVertices();
        n_assert(destPtr);
        
        int i;
        for (i = 0; i < numVertices; i++)
        {
            *destPtr++ = *(sourcePtr + (i * stride));
            *destPtr++ = *(sourcePtr + (i * stride + 1));
            *destPtr++ = *(sourcePtr + (i * stride + 2));
        }
        sourceMesh->UnlockVertices();
        destMesh->UnlockVertices();
    }
}

//------------------------------------------------------------------------------
/**
*/
/*
void
nSkinShadowCaster::CreateRenderGeometry()
{
    n_assert(this->skinnedVerticies);
    n_assert(this->numSkinnedVerticies > 0);

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    
    if (this->refRenderMesh.isvalid())
    {
        // cleanup old data
        this->refRenderMesh->Release();
    }

    nMesh2* destMesh = 0;
    if (0 == destMesh)
    {
        // create a new mesh
        destMesh = gfxServer->NewMesh(0);
        this->refRenderMesh = destMesh;
    }

    n_assert(destMesh);

    if (! destMesh->IsValid())
    {
        // init mesh
        destMesh->SetVertexComponents(nMesh2::Color); // FIXME: the needed coord is vector4, but in nebula coord is vector3, so color is used as way around.
        destMesh->SetUsage(nMesh2::WriteOnce | nMesh2::NeedsVertexShader);
        destMesh->SetNumVertices(2 * this->numSkinnedVerticies);

        destMesh->SetNumIndices(0);
        destMesh->SetNumEdges(0);
        destMesh->SetAsyncEnabled(false);
        destMesh->SetRefillBuffersMode(nMesh2::Enabled);
        destMesh->Load();
    }
        
    if (nMesh2::NeededNow == destMesh->GetRefillBuffersMode())
    {
        float* basePtr = destMesh->LockVertices();
        // fill mesh with data
        int v;
        for (v = 0; v < this->numSkinnedVerticies; v++)
        {
            // vertex 1 - V.w = 1.0
            *basePtr++ = this->skinnedVerticies[v].x;
            *basePtr++ = this->skinnedVerticies[v].y;
            *basePtr++ = this->skinnedVerticies[v].z;
            *basePtr++ = 1.0f;

            // vertex 2 - V.w = 0.0
            *basePtr++ = this->skinnedVerticies[v].x;
            *basePtr++ = this->skinnedVerticies[v].y;
            *basePtr++ = this->skinnedVerticies[v].z;
            *basePtr++ = 0.0f;
        }
        destMesh->UnlockVertices();
        destMesh->SetRefillBuffersMode(nMesh2::Enabled);
    }
}*/

//------------------------------------------------------------------------------
/**
*/
void
nSkinShadowCaster::CreateFaceNormales()
{
    n_assert(this->skinnedVerticies);
    n_assert(this->numSkinnedVerticies > 0);
    n_assert(this->faces);
    n_assert(this->numFaces > 0);

    // allocate face normal buffer if not happened yet
    if (0 == this->faceNormales)
    {
        this->numFaceNormales = this->numFaces;
        this->faceNormales = n_new_array(vector3, this->numFaceNormales);
    }
    
    int i;
    for (i = 0; i < this->numFaces; i++)
    {
        const Face& face = this->faces[i];
        const vector3& vertex0 = this->skinnedVerticies[face.index[0]];
        const vector3& vertex1 = this->skinnedVerticies[face.index[1]];
        const vector3& vertex2 = this->skinnedVerticies[face.index[2]];

        // compute the face normal
        this->faceNormales[i] = (vertex1 - vertex0) * (vertex2 - vertex0);
        this->faceNormales[i].norm();
    }
}

//------------------------------------------------------------------------------
/**
*/
/*bool
nSkinShadowCaster::DebugSetupGeometry()
{
    if (this->dbgMesh.isvalid())
    {
        this->dbgMesh->Release();
    }

    if (this->numSkinnedVerticies > 0 && this->numFaces > 0)
    {
        nMesh2* mesh = nGfxServer2::Instance()->NewMesh(0);
        mesh->SetAsyncEnabled(false);
        mesh->SetNumVertices(this->numFaces * 3);
        mesh->SetVertexComponents(nMesh2::Coord);
        mesh->SetNumGroups(1);
        mesh->SetNumEdges(0);
        mesh->SetNumIndices(0);
        mesh->SetUsage(nMesh2::WriteOnce);
        mesh->Load();

        vector3* srcVtxPtr = this->skinnedVerticies;
        float* dstVtxPtr = mesh->LockVertices();

        int i;
        int numValidVtx = 0;
        for (i = 0; i < this->numFaces; i++)
        {
            const Face& face = this->faces[i];
            if (face.lightFacing)
            {
                int f;
                for(f = 0; f < 3; f++)
                {
                    *(dstVtxPtr++) = srcVtxPtr[face.index[f]].x;
                    *(dstVtxPtr++) = srcVtxPtr[face.index[f]].y;
                    *(dstVtxPtr++) = srcVtxPtr[face.index[f]].z;
                    numValidVtx++;
                }
            }
        }

        mesh->UnlockVertices();
        this->refCoordMesh->UnlockVertices();
        
        nGfxServer2::Instance()->SetMesh(mesh);
        nGfxServer2::Instance()->SetVertexRange(0, numValidVtx);
        nGfxServer2::Instance()->SetIndexRange(0, mesh->GetNumIndices());
        
        this->dbgMesh = mesh;
        return true;
    }
    return false;
}*/
