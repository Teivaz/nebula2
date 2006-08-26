#ifndef N_MESHBUNDLE_H
#define N_MESHBUNDLE_H
//------------------------------------------------------------------------------
/**
    @class nMeshBundle
    @ingroup Tools

    A mesh bundle splits a single input nMeshBuilder object into several
    internal mesh builder objects based on common attributes and maximum
    number of vertices in a single meshbuilder object.
    nMeshBundle is useful when a single nMeshBuilder object is bigger then
    the recommended or possible vertex buffer size of a target system
    (e.g. 16 bit indices restrict vertex buffer to 65536 unique vertices).

    (C) 2003 RadonLabs GmbH
*/
#include "tools/nmeshbuilder.h"

//------------------------------------------------------------------------------
class nMeshBundle
{
public:
    /// constructor
    nMeshBundle();
    /// destructor
    ~nMeshBundle();
    /// clear array
    void Clear();
    /// constructs the mesh bundle from a single input mesh and a max number of vertices
    void BuildMeshBundle(const nMeshBuilder& srcMesh, int maxVertices);
    /// return number of created meshes
    int GetNumMeshes() const;
    /// return pointer to mesh at index
    nMeshBuilder* GetMeshAt(int index);

private:
    nArray<nMeshBuilder*> meshBundleArray;
};

//------------------------------------------------------------------------------
#endif

