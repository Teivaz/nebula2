//-------------------------------------------------------------------
//  mapoctree_collect.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "map/mapquadtree.h"
#include "gfx2/ngfxserver2.h"

//-------------------------------------------------------------------
/**
    Sammelt ein Element, stellt sicher, dass das collect_array
    nicht ueberlaeuft...

    - 02-Jun-99   floh    created
*/
//-------------------------------------------------------------------
void MapQuadtree::collect(MapQuadElement *oe)
{
    if (this->num_collected < this->array_size) {
        this->collect_array[this->num_collected++] = oe;
    } else {
        n_printf("MapQuadtree::collect(): Overflow in collect array!\n");
    }
}

//-------------------------------------------------------------------
/**
    Sammelt unbesehen alle Elemente der uebergebenen Node
    und geht rekursiv alle Childnodes runter.

    - 02-Jun-99   floh    created
*/
//-------------------------------------------------------------------
void MapQuadtree::recurse_collect_nodes_with_flags(MapQuadNode *on, int c_flags)
{
    MapQuadElement *oe;
    for (oe = (MapQuadElement *) on->elm_list.GetHead();
         oe;
         oe = (MapQuadElement *) oe->GetSucc())
    {
        oe->SetCollectFlags(c_flags);
        this->collect(oe);
    }
    if (on->c[0])
    {
        for (int i = 0; i < 4; ++i)
            this->recurse_collect_nodes_with_flags(on->c[i],c_flags);
    }
}

int MapQuadtree::GetCollect(MapQuadElement**& array)
{
    array = collect_array;
    return this->num_collected;
}

/**
    @brief Collect octree by frustrum.
    Assume symmetrical frustum.

    Uses a clip mask to minimise checks.  Hardcoded to only derive clip
    planes from frustum but no reason why it cannot be extended to use
    additional clip planes, up to a total max of 32.
*/
int MapQuadtree::CollectByFrustum(nGfxServer2* gfx_server,
                                  MapQuadElement**& array)
{
    InitClipPlanes(gfx_server);

    num_collected = 0;

    unsigned int active_clips = 0x003F;
    recurse_collect_by_frustum(this->tree_root, active_clips);

    array = collect_array;
    return num_collected;
}

/**
    Initialise the clip planes for frustum collection.
*/
void MapQuadtree::InitClipPlanes(nGfxServer2 * gfx_server)
{
    // The setup of the planes here are a bit mucky:
    //      0 - front
    //      1 - back
    //      2 - left
    //      3 - right
    //      4 - top
    //      5 - bottom
	float minz = gfx_server->GetCamera().GetNearPlane();
	float maxz = gfx_server->GetCamera().GetFarPlane();
	float angle_h = static_cast<float>( gfx_server->GetCamera().GetAngleOfView() * N_PI / 180 );
    float cos_h = cosf(angle_h);
    float sin_h = sinf(angle_h);
    float angle_v = angle_h *  gfx_server->GetCamera().GetAspectRatio();
    float cos_v = cosf(angle_v);
    float sin_v = sinf(angle_v);

    // Normals point into plane
    // D = - dot(p.normal, point_in_plane)
    clipPlanes[0].set(0.0f, 0.0f, -1.0f, 0.0f);
    clipPlanes[0].w = -( -1.0f * -minz);
    clipPlanes[1].set(0.0f, 0.0f, 1.0f, 0.0f);
    clipPlanes[1].w = -( 1.0f * -maxz);
    clipPlanes[2].set(cos_h, 0.0f, -sin_h, 0.0f);
    clipPlanes[3].set(-cos_h, 0.0f, -sin_h, 0.0f);
    clipPlanes[4].set(0.0f, -cos_v, -sin_v, 0.0f);
    clipPlanes[5].set(0.0f, cos_v, -sin_v, 0.0f);

/*
    NOTE: to fix the culling problem with nmap not at origin, model_view
    shall be used instead of inv_viewer -- Paul H. Liu

    // Transform clip planes by current camera angle
    // Plane normals are transformed by the transpose of the inverse
    // matrix for transforming points.  Just transpose since mv is
    // already inverted
    matrix44 inv_viewer;
    gfx_server->GetMatrix(N_MXM_INVVIEWER, inv_viewer);
    inv_viewer.transpose();
*/
    matrix44 model_view;
    //gfx_server->GetMatrix(N_MXM_MODELVIEW, model_view);
	model_view = gfx_server->GetTransform( nGfxServer2::ModelView );
    model_view.transpose();

    // Would have used plane but vector4 can be transformed
    for (int i = 0; i < 6; ++i)
        clipPlanes[i] = model_view * clipPlanes[i];
}

/**
    @brief Recursively collect by frustum.
*/
void MapQuadtree::recurse_collect_by_frustum(MapQuadNode* on,
                                           unsigned int clip_mask)
{
    unsigned int out_clip_mask;

    // Clip-Status der aktuellen Node...
    if (false == box_clip_frustum(on->pos, on->p1, clipPlanes, out_clip_mask, clip_mask))
        return;

    // Node completely contained, gotta catch them all!
    if (0 == out_clip_mask)
    {
        recurse_collect_nodes_with_flags(on,MapQuadElement::N_COLLECT_VIEWVOL);
    }
    // Partially clipped by frustum, future doodah could do
    // with passing in clip mask to reduce number of checks recursively
    else
    {
        this->collect_nodes_in_frustum(on, out_clip_mask);
        if (on->c[0])
        {
            for (int i = 0; i < 4; ++i)
                this->recurse_collect_by_frustum(on->c[i], out_clip_mask);
        }
    }
}

/**
    @brief Collect nodes in this node only.
*/
void MapQuadtree::collect_nodes_in_frustum(MapQuadNode* on, unsigned int clip_mask)
{
    MapQuadElement *oe;
    for (oe = (MapQuadElement *) on->elm_list.GetHead();
         oe;
         oe = (MapQuadElement *) oe->GetSucc())
    {
        unsigned int out_clip_mask = 0;
        if (true == box_clip_frustum(oe->pos,oe->p1, clipPlanes, out_clip_mask, clip_mask))
        {
            oe->SetCollectFlags(MapQuadElement::N_COLLECT_VIEWVOL);
            this->collect(oe);
        }
    }
}

/**
    Clip AABB defined by p0, p1 to supplied planes.
    @param centre Centre of AABB
    @param p1 Upper corner of AABB
    @param planes the array of planes to clip against
    @param out_clip_mask returned clip plane overlap
    @param in_clip_mask planes to clip against
    @return true if the AABB is at least partially inside clip
*/
bool MapQuadtree::box_clip_frustum(const vector3& centre, const vector3& p1,
                                   vector4* planes,
                                   unsigned int& out_clip_mask,
                                   unsigned int in_clip_mask)
{
    // Figure out centre and half diagonal of AABB
    vector3 half_diagonal = p1 - centre;

    unsigned int mk = 1;
    out_clip_mask = 0;

    // loop while there are active planes..
    while (mk <= in_clip_mask)
    {
        if (in_clip_mask & mk)
        {
            float NP = float(half_diagonal.x * fabsf(planes->x) +
                             half_diagonal.y * fabsf(planes->y) +
                             half_diagonal.z * fabsf(planes->z));
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

