//------------------------------------------------------------------------------
//  nstaticshadowcaster_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "shadow/nstaticshadowcaster.h"

nNebulaClass(nStaticShadowCaster, "nshadowcaster");

//------------------------------------------------------------------------------
/**
*/
nStaticShadowCaster::nStaticShadowCaster() :
    faceNormales(0), numFaceNormales(0)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nStaticShadowCaster::~nStaticShadowCaster()
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
nStaticShadowCaster::LoadShadowData(nMesh2* sourceMesh)
{
    n_assert(sourceMesh);

    nShadowCaster::LoadShadowData(sourceMesh);
    
    this->LoadVerticies(sourceMesh);
    this->CreateFaceNormales();
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticShadowCaster::UnloadResource()
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
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticShadowCaster::LoadVerticies(nMesh2* sourceMesh)
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

    if (nMesh2::NeededNow == destMesh->GetRefillBuffersMode())
    {
        // source ptr
        float* sourcePtr = sourceMesh->LockVertices();
        n_assert(sourcePtr);
        ushort stride = sourceMesh->GetVertexWidth();
        
        // dest ptr
        vector3* destPtr = (vector3*) destMesh->LockVertices();
        n_assert(destPtr);
        
        int i;
        for (i = 0; i < numVertices; i++)
        {
            *(destPtr++) = *((vector3*) (sourcePtr + (i * stride)));
        }
        sourceMesh->UnlockVertices();
        destMesh->UnlockVertices();

        destMesh->SetRefillBuffersMode(nMesh2::Enabled);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticShadowCaster::CreateFaceNormales()
{
    n_assert(0 == this->faceNormales);
    
    // allocate memory
    this->numFaceNormales = this->numFaces;
    n_assert(this->numFaceNormales > 0);

    this->faceNormales = n_new_array(vector3, this->numFaceNormales);
    n_assert(0 != this->faceNormales);

    vector3* coords = (vector3*) this->refCoordMesh->LockVertices();
    
    int i;
    for (i = 0; i < this->numFaces; i++)
    {
        const Face& face = this->faces[i];
        const vector3& vertex0 = *(coords + face.index[0]);
        const vector3& vertex1 = *(coords + face.index[1]);
        const vector3& vertex2 = *(coords + face.index[2]);

        // compute the face normal
        this->faceNormales[i] = (vertex1 - vertex0) * (vertex2 - vertex0);
        this->faceNormales[i].norm();
    }
    this->refCoordMesh->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
*/
/*bool
nStaticShadowCaster::DebugSetupGeometry()
{
    if (this->dbgMesh.isvalid())
    {
        this->dbgMesh->Release();
    }

    if (this->refCoordMesh.isvalid() && this->refCoordMesh->IsValid() && this->refCoordMesh->GetNumVertices() > 0)
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

        vector3* srcVtxPtr = (vector3*) this->refCoordMesh->LockVertices();
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
}
*/
//------------------------------------------------------------------------------
/**
*/
vector3*
nStaticShadowCaster::GetCoords() const
{
    n_assert(this->refCoordMesh.isvalid() && this->refCoordMesh->IsValid());
    return (vector3*) this->refCoordMesh->LockVertices();
}