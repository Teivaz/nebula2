#ifndef N_OCTFRUSTUM_H
#define N_OCTFRUSTUM_H

#include "noctvisitor.h"

//-----------------------------------------------------------------------------
/**
    @class nOctFrustum

	@brief 
*/
class nOctFrustum : public nOctVisitor
{
public:
    nOctFrustum();

    virtual void DoCulling (nOctree* octree);

protected:
    void init_clip_planes_for_frustum (nOctree* octree);
    void recurse_collect_within_clip_planes (nOctree* octree, 
                                             nOctNode* on, uint clip_mask);
    bool box_clip_against_clip_planes(vector3& p0, vector3& p1, vector4* planes,
                                      uint& out_clip_mask,
                                      uint in_clip_mask);
    void collect_nodes_within_clip_planes (nOctree* octree, 
                                           nOctNode* on, uint clip_mask);

private:
    /// Clip planes for frustum collection
    vector4 clipPlanes[32];

};

#endif /*N_OCTFRUSTUM_H*/
