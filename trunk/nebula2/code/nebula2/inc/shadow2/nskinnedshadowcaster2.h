#ifndef N_SKINNEDSHADOWCASTER2_H
#define N_SKINNEDSHADOWCASTER2_H
//------------------------------------------------------------------------------
/**
    @class nSkinnedShadowCaster2
    @ingroup Shadow2

    A shadow caster for skinned geometry.

    (C) 2005 Radon Labs GmbH
*/
#include "shadow2/nshadowcaster2.h"
#include "character/ncharskeleton.h"

//------------------------------------------------------------------------------
class nSkinnedShadowCaster2 : public nShadowCaster2
{
public:
    /// constructor
    nSkinnedShadowCaster2();
    /// destructor
    virtual ~nSkinnedShadowCaster2();
    /// set the uptodate character skeleton
    void SetCharSkeleton(const nCharSkeleton* skel);
    /// setup the shadow volume for rendering
    virtual void SetupShadowVolume(const nLight& light, const matrix44& invModelLightMatrix);
    /// render the shadow volume
    virtual void RenderShadowVolume();

protected:
    /// override in subclasse to perform actual resource loading
    virtual bool LoadResource();
    /// override in subclass to perform actual resource unloading
    virtual void UnloadResource();
    /// update skinned mesh from character skeleton and bind pose mesh
    void UpdateSkinning();

    const nCharSkeleton* charSkeleton;  // the character skeleton
    bool charSkeletonDirty;             // only need to update skinned mesh when skeleton dirty
    nRef<nMesh2> refBindPoseMesh;       // the bind pose shadow mesh, only read by CPU
    nRef<nMesh2> refSkinnedMesh;        // the skinned shadow mesh, special extrusion layout, written and rendered
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSkinnedShadowCaster2::SetCharSkeleton(const nCharSkeleton* skel)
{
    n_assert(skel);
    this->charSkeleton = skel;
    this->charSkeletonDirty = true;
}

//------------------------------------------------------------------------------
#endif
