#include "octree/noctfrustum.h"

nNebulaClass(nOctFrustum, "noctvisitor");

//-------------------------------------------------------------------
/**
    Constructor.
*/
nOctFrustum::nOctFrustum() :
    clipPlanes()
{
}

//-------------------------------------------------------------------
/**
    Do frustum culling for given octree.

    @param octree octree spatial database which we do culling
*/
void nOctFrustum::DoCulling(nOctree* octree)
{
    nOctVisitor::DoCulling(octree);

    init_clip_planes_for_frustum();

    uint active_clips = 0x003F;
    recurse_collect_within_clip_planes(octree->GetRoot(), active_clips);
}

//-------------------------------------------------------------------
/**
    Initialize clip planes for view frustum with view-projection matrix.

    this code from at:
    http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf
*/
void nOctFrustum::init_clip_planes_for_frustum()
{
    nGfxServer2* gfx_server = octree->GetGfxServer2();

    const matrix44& viewer = gfx_server->GetTransform(nGfxServer2::View);
    const matrix44& projection = gfx_server->GetTransform(nGfxServer2::Projection);

    matrix44 m = viewer * projection;

    // front
    clipPlanes[0].x = m.M13;
    clipPlanes[0].y = m.M23;
    clipPlanes[0].z = m.M33;
    clipPlanes[0].w = m.M43;

    // back
    clipPlanes[1].x = (m.M14 - m.M13);
    clipPlanes[1].y = (m.M24 - m.M23);
    clipPlanes[1].z = (m.M34 - m.M33);
    clipPlanes[1].w = (m.M44 - m.M43);

    // left
    clipPlanes[2].x = (m.M14 + m.M11);
    clipPlanes[2].y = (m.M24 + m.M21);
    clipPlanes[2].z = (m.M34 + m.M31);
    clipPlanes[2].w = (m.M44 + m.M41);

    // right
    clipPlanes[3].x = (m.M14 - m.M11);
    clipPlanes[3].y = (m.M24 - m.M21);
    clipPlanes[3].z = (m.M34 - m.M31);
    clipPlanes[3].w = (m.M44 - m.M41);

    // top
    clipPlanes[4].x = (m.M14 - m.M12);
    clipPlanes[4].y = (m.M24 - m.M22);
    clipPlanes[4].z = (m.M34 - m.M32);
    clipPlanes[4].w = (m.M44 - m.M42);

    // bottom
    clipPlanes[5].x = (m.M14 + m.M12);
    clipPlanes[5].y = (m.M24 + m.M22);
    clipPlanes[5].z = (m.M34 + m.M32);
    clipPlanes[5].w = (m.M44 + m.M42);

    //normalize planes.
    float denom;
    vector3 tmp;

    for (int i=0; i<6; i++)
    {
        tmp.set (clipPlanes[i].x, clipPlanes[i].y, clipPlanes[i].z);
        denom = 1.0f / tmp.len();
        clipPlanes[i].x *= denom;
        clipPlanes[i].y *= denom;
        clipPlanes[i].z *= denom;
        clipPlanes[i].w *= denom;
    }
}

//-------------------------------------------------------------------
/**
    @brief Recursively collect within the clip planes.
*/
void nOctFrustum::recurse_collect_within_clip_planes(nOctNode* on,
                                                     uint clip_mask)
{
    uint out_clip_mask;

    // Clip-Status der aktuellen Node...
    if (false == box_clip_against_clip_planes(on->minCorner,
                                              on->maxCorner, clipPlanes,
                                              out_clip_mask, clip_mask))
        return;

    // Node completely contained, gotta catch them all!
    if (0 == out_clip_mask)
    {
        octree->recurse_collect_nodes_with_flags(on, nOctElement::N_COLLECT_CLIP_PLANES);
    }
    // Partially clipped by clip planes, future doodah could do
    // with passing in clip mask to reduce number of checks recursively
    else
    {
        this->collect_nodes_within_clip_planes( on, out_clip_mask);
        if (on->c[0])
        {
            for (int i = 0; i < 8; ++i)
                this->recurse_collect_within_clip_planes( on->c[i], out_clip_mask);
        }
    }
}

//-------------------------------------------------------------------
/**
    Clip AABB defined by minCorner, maxCorner to supplied planes.

    This code is derived from code provided by Ville Miettinen of Hybrid.fi
    on the Algorithms mailing list.  It is similar to the code that
    Hybrid uses in their product dPVS (http://www.hybrid.fi/dpvs.html).

    @param minCorner Lower corner of AABB
    @param maxCorner Upper corner of AABB
    @param planes the array of planes to clip against (up to 32)
    @param out_clip_mask returned clip plane overlap
    @param in_clip_mask planes to clip against
    @return true if the AABB is at least partially inside clip
*/
bool nOctFrustum::box_clip_against_clip_planes(vector3& minCorner, vector3& maxCorner,
                                               vector4* planes,
                                               uint& out_clip_mask,
                                               uint in_clip_mask)
{
    // Figure out centre and half diagonal of AABB
    vector3 centre = (minCorner + maxCorner) / 2.0f;
    vector3 half_diagonal = maxCorner - centre;

    uint mk = 1;
    out_clip_mask = 0;

    // loop while there are active planes..
    while (mk <= in_clip_mask)
    {
        if (in_clip_mask & mk)
        {
            float NP = float(half_diagonal.x * n_abs(planes->x) +
                             half_diagonal.y * n_abs(planes->y) +
                             half_diagonal.z * n_abs(planes->z));
            float MP = centre.x * planes->x +
                       centre.y * planes->y +
                       centre.z * planes->z +
                       planes->w;

            // Behind clip plane
            if ((MP + NP) < 0.0f)
                return false;

            // Cross this plane
            if ((MP - NP) < 0.0f)
                out_clip_mask |= mk;
        }

        mk <<= 1;
        ++planes;
    }

    // Contained in some way
    return true;
}

//-------------------------------------------------------------------
/**
    @brief Collect nodes in this node only.
*/
void nOctFrustum::collect_nodes_within_clip_planes(nOctNode* on,
                                                   uint clip_mask)
{
    nOctElement *oe;
    for (oe = (nOctElement *) on->elm_list.GetHead();
         oe;
         oe = (nOctElement *) oe->GetSucc())
    {
        uint out_clip_mask = 0;
        if (true == box_clip_against_clip_planes(oe->minCorner,oe->maxCorner, clipPlanes,
                                                 out_clip_mask, clip_mask))
        {
            oe->SetCollectFlags(nOctElement::N_COLLECT_CLIP_PLANES);
            this->Collect(oe);
        }
    }
}
