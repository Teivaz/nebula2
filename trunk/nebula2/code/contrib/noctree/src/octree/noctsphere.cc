//-------------------------------------------------------------------
//  noctsphere.cc
//  
//-------------------------------------------------------------------
#include "mathlib/bbox.h"
#include "mathlib/sphere.h"
#include "octree/noctsphere.h"

//-----------------------------------------------------------------------------
/**
*/
void nOctSphere::DoCulling(nOctree* octree)
{
    recurse_collect_by_sphere (octree, octree->GetRoot(), this->clip, false);
}

//-------------------------------------------------------------------
/**
    @brief Recursively collect within the clip planes.
*/
void nOctSphere::recurse_collect_by_sphere(nOctree* octree, 
                                        nOctNode* on,
                                        const sphere& clip,
                                        bool full_intersect)
{
    bbox3 box(on->p0, on->p1);
    if (false == clip.intersects(box))
        return;

    // Check for full intersect
    {
        float s, d = 0.0f;

        if (clip.p.x > box.vmin.x) { s = clip.p.x - box.vmin.x; d += s * s; }
        else if (clip.p.x < box.vmax.x) { s = clip.p.x - box.vmax.x; d += s * s; }
        if (clip.p.y > box.vmin.y) { s = clip.p.y - box.vmin.y; d += s * s; }
        else if (clip.p.y < box.vmax.y) { s = clip.p.y - box.vmax.y; d += s * s; }
        if (clip.p.z > box.vmin.z) { s = clip.p.z - box.vmin.z; d += s * s; }
        else if (clip.p.z < box.vmax.z) { s = clip.p.z - box.vmax.z; d += s * s; }

        full_intersect = d <= clip.r * clip.r;
    }

    // Intersects with clipping sphere, go collect
    this->collect_nodes_in_sphere(octree, on, clip, full_intersect);

    // Recurse if possible
    if (on->c[0])
    {
        for (int i = 0; i < 8; ++i)
            this->recurse_collect_by_sphere(octree, on->c[i], clip, full_intersect);
    }
}

//------------------------------------------------------------------- 
/**
    @brief Collect nodes in this node only.
*/
void nOctSphere::collect_nodes_in_sphere(nOctree* octree, 
                                      nOctNode* on,
                                      const sphere& clip,
                                      bool full_intersect)
{
    nOctElement *oe;
    for (oe = (nOctElement *)on->elm_list.GetHead();
         oe;
         oe = (nOctElement *)oe->GetSucc())
    {
        if (true == full_intersect)
            octree->collect(oe);
        else
        {
            sphere sphere(oe->pos, oe->radius);
            if (true == clip.intersects(sphere))
            {
                // oe->SetCollectFlags(nOctElement::N_COLLECT_SPHERE);
                octree->collect(oe);
            }
        }
    }
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------