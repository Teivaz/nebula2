#ifndef N_SKINSHADOWCASTER_H
#define N_SKINSHADOWCASTER_H
//------------------------------------------------------------------------------
/**
    @class nSkinShadowCaster
    @ingroup Shadow

    @brief A shadow caster for skinned geometry.

    (C) 2004 RadonLabs GmbH
*/
#include "shadow/nshadowcaster.h"

class nCharSkeleton;

//------------------------------------------------------------------------------
class nSkinShadowCaster : public nShadowCaster
{
public:
    /// constructor
    nSkinShadowCaster();
    /// destructor
    virtual ~nSkinShadowCaster();
    /// render shadow
    virtual void RenderShadow(const matrix44& modelMatrix, int groupIndex);
    /// set the updated char skeleton
    void SetCharSkeleton(const nCharSkeleton* charSkeleton);

protected:
    /// get the face normal of the triangle at index
    virtual vector3* GetFaceNormals() const;
    /// get the coordiantes array
    virtual vector3* GetCoords() const;
    /// get num coordiantes
    virtual int GetNumCoords() const;

    /// perform actual resource unloading
    virtual void UnloadResource();
    /// perform data load from source mesh
    virtual void LoadShadowData(nMesh2* sourceMesh);

private:
    /// read vertex data from source mesh
    void LoadVertices(nMesh2* sourceMesh);
    /// read wights and JIndices data from source mesh
    void LoadWeightsAndJIndices(nMesh2* sourceMesh);

    /// update the skinned vertices with the current set skeleton
    void UpdateSkinnedVertices();
    /// generate face normals
    void CreateFaceNormals();

    /// array of the face normals of the current loaded triangles
    vector3* faceNormals;
    int numFaceNormals;

    /// calcuated vertex data for the current skeleton
    vector3* skinnedVertices;
    int numSkinnedVertices;

    /// source coordiantes of the all groups of the shadow geometry, is shared via resource system
    nRef<nMesh2> refCoordMesh;

    bool charSkeletonDirty;
    const nCharSkeleton* charSkeleton;

    struct WeightAndJIndex
    {
        float weight[4];
        ushort index[4];
    };

    WeightAndJIndex* weightsAndJIndices;
    int numWeightsAndJIndices;
};

//------------------------------------------------------------------------------
/**
*/
inline
vector3*
nSkinShadowCaster::GetFaceNormals() const
{
    n_assert(0 != this->faceNormals)
    return this->faceNormals;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSkinShadowCaster::SetCharSkeleton(const nCharSkeleton* charSkeleton)
{
    n_assert(charSkeleton);
    this->charSkeletonDirty = true;
    this->charSkeleton = charSkeleton;
}

//------------------------------------------------------------------------------
#endif

