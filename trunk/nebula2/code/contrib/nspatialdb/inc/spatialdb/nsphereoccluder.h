#ifndef N_SPHEREOCCLUDER_H
#define N_SPHEREOCCLUDER_H

/**
   @class nSphereOccluder
   @brief Encapsulates the occlusion region and flags of an occluding sphere, with handling of 'active occluding' flags.

   This class is similar to the nSphereOccluder class but represents an occluding sphere, which obscures objects
   as seen from a specific viewpoint.  It determines if an object is occluded by the sphere; that is, there is
   no line segment connecting the viewpoint and the test object that does not intersect the sphere.

   The return value of an occlusion test is a two-member result_info, which is designed for efficient use 
   by heirarchical objects.  In many spatial heirarchies it holds true that if a specific tree node is
   fully occluded/not occluded then all the children of that node are also occluded/not occluded, and so
   we can avoid doing additional tests on the nodes children when we already know what the answer will be.

   The result_info has three possible return values:
   "occluded" "active_flag"
      true     0          this node is fully occluded, so all children are occluded
     false     0          this node is fully visible, so all children will be fully visible
     false     !0         this node is partially visible, so children will have to be tested individually
*/

#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"
#include "mathlib/plane.h"
#include "mathlib/sphere.h"
#include "mathlib/bbox.h"

class nSphereOccluder {
public:
    nSphereOccluder();
    nSphereOccluder(const nSphereOccluder &copyme);
    nSphereOccluder(const vector3 &viewpoint, const vector3 &center, float radius);
    nSphereOccluder(const vector3 &viewpoint, const sphere &occludingsphere);

    // helper class for occlusion
    class result_info {
        public:
            bool    occluded; // true when the volume is not visible
            unsigned char   active_flag;  // one bit tells if we need to do the clipping check
        
            result_info(bool c = false, unsigned char a = 1) : 
                occluded(c), active_flag(a) 
            { }
    };

    result_info TestBBox(const bbox3 &boxtest, result_info in);
    result_info TestSphere(const sphere &spheretest, result_info in);
    result_info TestPoint(const vector3 &pointtest, result_info in);

    void VisualizeSphere(nGfxServer2 *gfx2, const vector4 &color);

protected:
    vector3 m_viewpoint;
    sphere m_sphere;
};

inline
nSphereOccluder::nSphereOccluder() : m_viewpoint(0,0,0), m_sphere(0,0,0,1)
{
}

inline
nSphereOccluder::nSphereOccluder(const nSphereOccluder &copyme)
{
    m_viewpoint = copyme.m_viewpoint;
    m_sphere = copyme.m_sphere;
}


inline
nSphereOccluder::nSphereOccluder(const vector3 &viewpoint, const vector3 &center, float radius)
{
    m_viewpoint = viewpoint;
    m_sphere = sphere(center,radius);
}

inline
nSphereOccluder::nSphereOccluder(const vector3 &viewpoint, const sphere &occludingsphere)
{
    m_viewpoint = viewpoint;
    m_sphere = occludingsphere;
}

inline
nSphereOccluder::result_info nSphereOccluder::TestBBox(const bbox3 &boxtest, result_info in)
{
    // Convert to a bounding sphere and test it
    sphere virtsphere(boxtest.center(), boxtest.extents().len());

    return TestSphere(virtsphere, in);
}

inline
nSphereOccluder::result_info nSphereOccluder::TestSphere(const sphere &spheretest, nSphereOccluder::result_info ri)
{
    line3 occluderline(m_viewpoint, m_sphere.p);
    vector3 testviewvector = spheretest.p - occluderline.b;

    // trivial culling criteria: distance from occluder to viewpoint is less than the occluder radius
    if (occluderline.len() < m_sphere.r)
        return nSphereOccluder::result_info(true,0);

    // if the test sphere is completely in front of the occluder, we have a trivial non-occlusion
    if ( (testviewvector.len() + spheretest.r) < (occluderline.m.len() - m_sphere.r) )
        return nSphereOccluder::result_info(false,0);

    // figure out how big the occluder looks at the test sphere's distance
    n_assert(occluderline.m.len() > 0.0f);
    float occludertestradius = m_sphere.r * ( testviewvector.len() / occluderline.m.len() );

    // find the distance between the center of the test sphere and the line projected from
    // the viewpoint to the occluder's center
    float testdistance = occluderline.distance(spheretest.p);

    // at this point, you pretend that the occluder has been moved out to the same
    // distance from the viewpoint as the test sphere along its occluder line, with an
    // appropriate increase in radius.
    // now, does the occluder totally swallow up the sphere?  we have to check the test sphere's
    // radius and the distance of the test sphere from the line.
    if ( occludertestradius > testdistance + spheretest.r )
        return nSphereOccluder::result_info(true,0);

    // ok, maybe the test sphere is so far from the line that the occluder doesn't enclosed it at all.
    // in this case, we have definite non-occlusion
    if ( occludertestradius < testdistance - spheretest.r )
        return nSphereOccluder::result_info(false,0);
     
    // can't determine visibility, it's a partial occlusion.  just return the result_info that was passed in
    return ri;
}

inline
nSphereOccluder::result_info nSphereOccluder::TestPoint(const vector3 &pointtest, nSphereOccluder::result_info ri)
{
    // build a bbox from the point and test that
    bbox3 mybbox(pointtest, vector3(0,0,0));
    return TestBBox(mybbox, ri);
}

inline
void nSphereOccluder::VisualizeSphere(nGfxServer2 *gfx2, const vector4 &color)
{
}

#endif

