//-------------------------------------------------------------------
//  noctbox.cc
//  
//-------------------------------------------------------------------
#include "mathlib/bbox.h"
#include "octree/noctbbox.h"

//-------------------------------------------------------------------
/**
    Constructor.
*/
nOctBBox::nOctBBox()
{
}

//-------------------------------------------------------------------
/**
*/
void nOctBBox::DoCulling(nOctree* octree)
{
    this->recurse_collect_by_bbox(octree, octree->GetRoot());
}

//-------------------------------------------------------------------
/**
    - 17-Aug-00   floh    created
*/
void nOctBBox::recurse_collect_by_bbox(nOctree* octree, nOctNode *on)
{
    // clip status of current node
    int bbox_c = this->box_clip_box(on->p0,on->p1);
    if (bbox_c > 0) {
        // node is contained completely inside the
        // collection box, trivially accept all child nodes
        octree->recurse_collect_nodes_with_flags(on,nOctElement::N_COLLECT_BBOX);
    } else if (bbox_c == 0) {
        // node is partly contained in box, all elements in
        // the node need to be checked further, and recurse
        // to children
        this->collect_nodes_in_bbox(octree, on);
        if (on->c[0]) {
            int i;
            for (i=0; i<8; i++) this->recurse_collect_by_bbox(octree, on->c[i]);
        }
    }
}

//-------------------------------------------------------------------
/**
    - 17-Aug-00   floh    created
*/
void nOctBBox::collect_nodes_in_bbox(nOctree* octree, nOctNode *on)
{
    nOctElement *oe;
    for (oe = (nOctElement *) on->elm_list.GetHead();
         oe;
         oe = (nOctElement *) oe->GetSucc())
    {
        int c = this->box_clip_box(oe->p0,oe->p1);
        if (c >= 0) {
            oe->SetCollectFlags(nOctElement::N_COLLECT_BBOX);
            octree->collect(oe);
        }
    }
}

//-------------------------------------------------------------------
/**
    Get clipping status of node bounding box against global
    collection bounding box.

    Return:
      - +1 - node box completely containd in collection box
      - -1 - node box completely outside of collection box
      -  0 - node box partially inside collection box, or collection
             box is contained in node box

    - 02-Jun-00   floh    created
*/
int nOctBBox::box_clip_box(vector3& p0, vector3& p1)
{
    bbox3 b(p0,p1);
    int res = b.intersect(this->collect_bbox);
    if (bbox3::OUTSIDE == res) {
        return -1;
    } else if (bbox3::ISCONTAINED == res) {
        return +1;
    } else {
        return 0;
    }
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------