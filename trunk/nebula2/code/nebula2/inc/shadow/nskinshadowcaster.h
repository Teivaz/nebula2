#ifndef N_SKINSHADOWCASTER_H
#define N_SKINSHADOWCASTER_H
//------------------------------------------------------------------------------
/**
    @class nSkinShadowCaster
    @ingroup Shadow

    A shadow caster for skinned geometry.

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
    void LoadVerticies(nMesh2* sourceMesh);
    /// read wights and JIndicies data from source mesh
    void LoadWeightsAndJIndicies(nMesh2* sourceMesh);

    /// update the skinned verticies with the current set skeleton
    void UpdateSkinnedVerticies();
    /// generate face normals
    void CreateFaceNormales();

    /// array of the face normals of the current loaded triangels
    vector3* faceNormales;
    int numFaceNormales;

    /// calcuated vertex data for the current skeleton
    vector3* skinnedVerticies;
    int numSkinnedVerticies;

    /// source coordiantes of the all groups of the shadow geometry, is shared via resource system
    nRef<nMesh2> refCoordMesh;

    bool charSkeletonDirty;
    const nCharSkeleton* charSkeleton;

    struct WeightAndJIndex
    {
        float weight[4];
        ushort index[4];
    };

    WeightAndJIndex* weightsAndJIndicies;
    int numWeightsAndJIndicies;
};

//------------------------------------------------------------------------------
/**
*/
inline
vector3*
nSkinShadowCaster::GetFaceNormals() const
{
    n_assert(0 != this->faceNormales)
    return this->faceNormales;
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