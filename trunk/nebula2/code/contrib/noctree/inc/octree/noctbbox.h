#ifndef N_OCTBBOX_H
#define N_OCTBBOX_H

#include "octree/noctvisitor.h"

//-----------------------------------------------------------------------------
/**
    @class nOctBBox
	@brief culling with given bounding box

*/
class nOctBBox : public nOctVisitor
{
public:
    nOctBBox();

    virtual void DoCulling (nOctree* oct);

    void SetBBox (bbox3& bbox);
    const bbox3& GetBBox ();

private:
    void recurse_collect_by_bbox (nOctree* octree, nOctNode *on);
    void collect_nodes_in_bbox(nOctree* octree, nOctNode *on);
    int  box_clip_box(vector3& p0, vector3& p1);

private:
    bbox3    collect_bbox;

};

//-----------------------------------------------------------------------------
/**
*/
void nOctBBox::SetBBox(bbox3& bbox)
{
    this->collect_bbox = bbox;
}

//-----------------------------------------------------------------------------
/**
*/
const bbox3& nOctBBox::GetBBox()
{
    return this->collect_bbox;
}

#endif /*N_OCTBBOX_H*/
