#ifndef N_SPHERECLIPPER_H
#define N_SPHERECLIPPER_H

/**
    @class nSphereClipper
    @ingroup NSpatialDBContribModule
    @brief Encapsulates the clipping region and flags of a sphere, with
    handling of 'active clipping plane' flags.

    This class is similar to the nFrustumClipper class but represents a
    clipping sphere.  It determines if an object is totally outside the
    clipping sphere.  Also, the @c active_flag of the return value will be
    @c 0 if an object is totally inside the sphere; typically you can use
    this for hierarchical objects to avoid doing cull checks on the children.
*/

#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"
#include "mathlib/plane.h"
#include "mathlib/sphere.h"
#include "mathlib/bbox.h"

class nSphereClipper {
public:
    nSphereClipper();
    nSphereClipper(const nSphereClipper &copyme);
    nSphereClipper(const vector3 &center, float radius);
    nSphereClipper(const sphere &clipsphere);

    // helper class for culling
    class result_info {
        public:
            bool    culled; ///< true when the volume is not visible
            unsigned char   active_flag;  ///< one bit tells if we need to do the clipping check
        
            result_info(bool c = false, unsigned char a = 1) : 
                culled(c), active_flag(a) 
            { }
    };

    result_info TestBBox(const bbox3 &boxtest, result_info in);
    result_info TestSphere(const sphere &spheretest, result_info in);
    result_info TestPoint(const vector3 &pointtest, result_info in);

    void VisualizeSphere(nGfxServer2 *gfx2, const vector4 &color);

protected:
    sphere m_sphere;
};

inline
nSphereClipper::nSphereClipper() : m_sphere(0,0,0,0)
{
}

inline
nSphereClipper::nSphereClipper(const nSphereClipper &copyme)
{
    m_sphere = copyme.m_sphere;
}


inline
nSphereClipper::nSphereClipper(const vector3 &center, float radius)
{
    m_sphere = sphere(center,radius);
}

inline
nSphereClipper::nSphereClipper(const sphere &clipsphere)
{
    m_sphere = clipsphere;
}

inline
nSphereClipper::result_info nSphereClipper::TestBBox(const bbox3 &boxtest, result_info in)
{
    // Convert to a bounding sphere and test it
    sphere virtsphere(boxtest.center(), boxtest.extents().len());

    return TestSphere(virtsphere, in);
}

inline
nSphereClipper::result_info nSphereClipper::TestSphere(const sphere &spheretest, nSphereClipper::result_info ri)
{
    // compare radius and distance
    vector3 d(m_sphere.p - spheretest.p);
    float rsum = m_sphere.r + spheretest.r;
    float dsquared = d.lensquared();

    // if the spheres are more distant than the sum of their two radii,
    // then cull it totally
    if (dsquared > (rsum*rsum))
        return nSphereClipper::result_info(true,0);

    // if the test sphere is of smaller radius than the clip sphere, and
    // the centers are closer than the differences of the two radii, the
    // test sphere is completely enclosed within the clip sphere and we
    // should turn the active_flag off, so that children within the test
    // sphere don't need to be clip tested at all
    if (spheretest.r < m_sphere.r)
    {
        float rdiff = m_sphere.r - spheretest.r;
        if ( dsquared < (rdiff*rdiff) )
        {
            ri.active_flag = 0;
            return ri;
        }
    }

    // non-conclusive test, just return the result_info as-is
    return ri;
}

inline
nSphereClipper::result_info nSphereClipper::TestPoint(const vector3 &pointtest, nSphereClipper::result_info ri)
{
    // build a bbox from the point and test that
    bbox3 mybbox(pointtest, vector3(0,0,0));
    return TestBBox(mybbox, ri);
}

inline
void nSphereClipper::VisualizeSphere(nGfxServer2 *gfx2, const vector4 &color)
{
    matrix44 m;
    m.scale( vector3( m_sphere.r, m_sphere.r, m_sphere.r ) );
    m.set_translation( m_sphere.p );
    gfx2->BeginShapes();
    gfx2->DrawShape( nGfxServer2::Sphere, m, color );
    gfx2->EndShapes();
}

#endif

