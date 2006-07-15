//------------------------------------------------------------------------------
//  nmeshbundle.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nmeshbundle.h"

//------------------------------------------------------------------------------
/**
*/
nMeshBundle::nMeshBundle()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMeshBundle::~nMeshBundle()
{
    this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
nMeshBundle::Clear()
{
    int i;
    for (i = 0; i < this->meshBundleArray.Size(); i++)
    {
        n_delete(this->meshBundleArray[i]);
        this->meshBundleArray[i] = 0;
    }
    this->meshBundleArray.Clear();
}

//------------------------------------------------------------------------------
/**
    Take a source mesh and split it into several meshes based on
    vertex components, usage flags and maximum number of vertices.
*/
void
nMeshBundle::BuildMeshBundle(const nMeshBuilder& srcMesh, int maxVertices)
{
    n_assert(maxVertices > 0);

    // create an index remapping map
    nArray<int> indexMap(0, 0);
    indexMap.SetFixedSize(srcMesh.GetNumVertices());
    indexMap.Fill(0, indexMap.Size(), -1);

    nMeshBuilder* curMeshBuilder = n_new(nMeshBuilder);
    int triIndex;
    int numTris = srcMesh.GetNumTriangles();
    for (triIndex = 0; triIndex < numTris; triIndex++)
    {
        // current mesh full?
        if (curMeshBuilder->GetNumVertices() >= (maxVertices - 3))
        {
            this->meshBundleArray.Append(curMeshBuilder);
            curMeshBuilder = n_new(nMeshBuilder);
            indexMap.Fill(0, indexMap.Size(), -1);
        }

        // add triangle and vertices to current mesh
        curMeshBuilder->CopyTriangle(srcMesh, triIndex, indexMap);
    }

    // append the last mesh
    this->meshBundleArray.Append(curMeshBuilder);
}

//------------------------------------------------------------------------------
/**
*/
int
nMeshBundle::GetNumMeshes() const
{
    return this->meshBundleArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
nMeshBuilder*
nMeshBundle::GetMeshAt(int index)
{
    return this->meshBundleArray[index];
}
