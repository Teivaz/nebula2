#ifndef N_OCTBBOX_H
#define N_OCTBBOX_H

#include "mathlib/bbox.h"
#include "octree/noctvisitor.h"

//-----------------------------------------------------------------------------
/**
    @class nOctBBox
    @ingroup NOctreeContribModule
    @brief culling with given bounding box
*/
class nOctBBox : public nOctVisitor
{
public:
    virtual void DoCulling(nOctree* oct);

    void SetBBox(const bbox3& bbox);
    const bbox3& GetBBox();
private:
    void recurse_collect_by_bbox(nOctNode *on);
    void collect_nodes_in_bbox(nOctNode *on);
    int  box_clip_box(vector3& minCorner, vector3& maxCorner);

private:
    bbox3    collect_bbox;
};

//-----------------------------------------------------------------------------
/**
*/
inline
void nOctBBox::SetBBox(const bbox3& bbox)
{
    this->collect_bbox = bbox;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const bbox3& nOctBBox::GetBBox()
{
    return this->collect_bbox;
}

#endif /*N_OCTBBOX_H*/
