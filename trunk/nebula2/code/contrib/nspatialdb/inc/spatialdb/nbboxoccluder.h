#ifndef N_BBOXOCCLUDER_H
#define N_BBOXOCCLUDER_H


// warning:: bboxoccluder is not finished yet.  -GJH 2004-24-07

/**
   @class nBBoxOccluder
   @brief Encapsulates the occlusion region and flags of an occluding bounding box, with handling of 'active occluding' flags.

   This class is similar to the nBBoxOccluder class but represents an occluding bbox, which obscures objects behind a bounding box
   as seen from a specific viewpoint.  It determines if an object is occluded by the box; that is, there is
   no line segment connecting the viewpoint and the test object that does not intersect the box.

   The return value of an occlusion test is a two-member result_info, which is designed for efficient use 
   by heirarchical objects.  In many spatial heirarchies it holds true that if a specific tree node is
   fully occluded/not occluded then all the children of that node are also occluded/not occluded, and so
   we can avoid doing additional tests on the nodes children when we already know what the answer will be.

   The result_info has three possible return values:
   "culled" "active_flag"
      true     0          this node is fully occluded, so all children are occluded
     false     0          this node is fully visible, so all children will be fully visible
     false     !0         this node is partially visible, so children will have to be tested individually
*/

#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"
#include "mathlib/plane.h"
#include "mathlib/sphere.h"
#include "mathlib/bbox.h"
#include "spatialdb/nfrustumclipper.h"


// in fact, for a bounding box occluder we are really just checking objects against six clipping planes
// representing the silhouette, which is remarkably like the frustum clipper.  So we'll re-use that and
// basically invert the culling flag, plus check that the occluder is closer to the viewpoint than the object
// under test

class nBBoxOccluder : public nFrustumClipper {
public:
    nBBoxOccluder();
    nBBoxOccluder(const nBBoxOccluder &copyme);
    nBBoxOccluder(const vector3 &viewpoint, const bbox3 &occludingbox);

    // we'll reuse the frustum clipper result_info datatype
    typedef nFrustumClipper::result_info result_info;

    result_info TestBBox(const bbox3 &boxtest, result_info in);
    result_info TestSphere(const sphere &spheretest, result_info in);
    result_info TestPoint(const vector3 &pointtest, result_info in);

    void VisualizeBBox(nGfxServer2 *gfx2, const vector4 &color);

protected:
    vector3 m_viewpoint; // we need a viewpoint for checking if objects are in front of the occluder
    float m_occluderdistance; // distance of occluder from the viewpoint
    int m_numplanes; // there may be less than 6 planes, so we need to track however many planes there are
    int m_planemask; // we also need to mask off the extra bits in the result_info if there are less than 6 planes
};

inline
nBBoxOccluder::nBBoxOccluder() : nFrustumClipper(), m_viewpoint(), m_occluderdistance(0.0f), m_numplanes(0), m_planemask(0)
{
}

inline
nBBoxOccluder::nBBoxOccluder(const nBBoxOccluder &copyme) : nFrustumClipper(copyme)

{
    m_viewpoint = copyme.m_viewpoint;
    m_occluderdistance = copyme.m_occluderdistance;
    m_numplanes = copyme.m_numplanes;
    m_planemask = copyme.m_planemask;
}


inline
nBBoxOccluder::nBBoxOccluder(const vector3 &viewpoint, const bbox3 &occludingbox) : nFrustumClipper(), m_viewpoint(viewpoint)
{
    vector3 occludervector(occludingbox.center() - viewpoint);
    vector3 occextents(occludingbox.extents());
    m_occluderdistance = occludervector.len() - occextents.len();

    // degenerate case when the viewpoint is inside the occluder, maybe we'll handle this smartly someday
    n_assert(!occludingbox.contains(viewpoint));

    // we'll have to build the clipping planes manually be examing the bounding box silhouette as seen from the
    // viewpoint
    // here's some data arrays describing the cube faces and edges
    // array of normals for the cube faces
    vector3 facenormals[6] = { vector3(0,-1,0), vector3(1,0,0), vector3(0,0,1), vector3(0,1,0), vector3(-1,0,0), vector3(0,0,-1) };

    // array telling what two faces each edge is part of--edges are considered directional vectors from one vertex to another, and
    // the faces are specified with the left side face first.  In this way we can determine which orientation of the edge to use
    // for the silhouette
    int edgefacemap[12][2] = { {0,5}, {0,1}, {0,2}, {0,4}, {1,5}, {1,2}, {4,2}, {5,4}, {3,5}, {3,1}, {3,2}, {3,4} };

    // array telling which vertices a given edge is connecting
    int edgevertexmap[12][2] = { {0,1}, {1,5}, {5,4}, {4,0}, {1,3}, {5,7}, {4,6}, {0,2}, {3,2}, {7,3}, {6,7}, {2,6} };

    // array of vertex locations, specifying the relative point locations from the bbox center
    vector3 vertexv[8] = { 
        vector3(-1,-1,-1), vector3(1,-1,-1), vector3(-1,1,-1), vector3(1,1,-1),
        vector3(-1,-1, 1), vector3(1,-1, 1), vector3(-1,1, 1), vector3(1,1, 1)
    };

    bool backfaces[6];

    // find back faces.  We compare the face normal with a vector from the viewpoint to the face
    for (int faceix=0; faceix < 6; faceix++)
    {
        // project out from the boxx center to the center of the face
        vector3 n(facenormals[faceix]);
        vector3 facepos(occludingbox.center() + vector3(occextents.x * n.x, occextents.y * n.y, occextents.z * n.z));
        vector3 vp2face(facepos - viewpoint);

        // the dotproduct of the face normal with  the (viewpoint -> face) vector tells us if the face is front or back facing
        backfaces[faceix] = ((vp2face % n) > 0.0);
    }

    // back faces are marked; find the silhouette edges.  These are edges bordering one front and one back face
    // when we find and edge, build a clip plane for it
    m_numplanes = 0;
    for (int edgeix=0; edgeix < 12; edgeix++)
    {
        // this is a silhouette edge if one face is a front face and one face is a back face.
        // it is important to know which face is the back face, so that we orient our edge (and thus the clipping plane)
        // correctly
        bool b1 = backfaces[edgefacemap[edgeix][0]], b2 = backfaces[edgefacemap[edgeix][1]];
        if ( (b1 & !b2) || (!b1& b2) ) // try replacing w/ xor... GJH
        {
            int i1=edgevertexmap[edgeix][0], i2 = edgevertexmap[edgeix][1];
            // swap the vertices if the left face is the back face
            if (b2)
            {
                int ix=i1; i1=i2; i2=ix;
            }
            // find the two vertex coordinates for this edge
            vector3 vp1(vertexv[i1]), vp2(vertexv[i2]);
            vector3 v1(occludingbox.center() + vector3(occextents.x * vp1.x, occextents.y * vp1.y, occextents.z * vp1.z));
            vector3 v2(occludingbox.center() + vector3(occextents.x * vp2.x, occextents.y * vp2.y, occextents.z * vp2.z));

            // we can construct the clipping plane using the viewpoint and the two vertices on the edge.
            m_planes[m_numplanes++] = plane(viewpoint, v1, v2);
        }
    }

    // there are only so many silhouette types you can make from a cube
    n_assert( (m_numplanes > 3) && (m_numplanes < 7) );

    int planemasks[3] = {0x0f, 0x1f, 0x3f};
    m_planemask = planemasks[m_numplanes-4];
}

inline
nBBoxOccluder::result_info nBBoxOccluder::TestBBox(const bbox3 &boxtest, result_info in)
{
    // first, mask off the extra planes.  This is needed if there are less than 6 clipping planes
    in.active_planes &= m_planemask;

    // if the bbox is closer to the viewpoint than the occluder, it is obviously not occluded
    vector3 testobjectvector(boxtest.center() - m_viewpoint);
    if (testobjectvector.len() - boxtest.extents().len() < m_occluderdistance)
        return result_info(false,0);

    // process the bbox using the view frustum stuff
    in = nFrustumClipper::TestBBox(boxtest, in);

    // we have to the reverse the result, since the frustum clipper says 'yes' when an object is not inside all the clipping
    // planes.  However, for an occluder a 'yes' result means the test object is not behind the occluder.  A similar situation
    // occurs when the object is actually occluded.  So we have to reverse the sense of the culling.
    // so we do some logic swizzling here to compute that
    if (in.culled)
        return result_info(false,0);
    else if (in.active_planes == 0)
        return result_info(true, 0);
    else
        return in;
}

inline
nBBoxOccluder::result_info nBBoxOccluder::TestSphere(const sphere &spheretest, nBBoxOccluder::result_info ri)
{
    // build a bbox from the sphere and test that
    bbox3 mybbox(spheretest.p, vector3(spheretest.r, spheretest.r, spheretest.r));
    return TestBBox(mybbox, ri);
}

inline
nBBoxOccluder::result_info nBBoxOccluder::TestPoint(const vector3 &pointtest, nBBoxOccluder::result_info ri)
{
    // build a bbox from the point and test that
    bbox3 mybbox(pointtest, vector3(0,0,0));
    return TestBBox(mybbox, ri);
}

inline
void nBBoxOccluder::VisualizeBBox(nGfxServer2 *gfx2, const vector4 &color)
{
}

#endif

