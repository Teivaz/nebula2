#ifndef N_PLANECLIPPER_H
#define N_PLANECLIPPER_H

/**
    @class nPlaneClipper
    @ingroup NSpatialDBContribModule
    @brief Encapsulates a set of clipping planes, handling
    of 'active clipping plane' flags for recursive traversal of
    spatial data structures.

    This is just a simple plane clipping class, allowing you to represent various
    primitives such as view frustums, shadow silhouettes, and convex hulls.  Objects
    are tested against the positive side of the plane; if the object is on the negative
    side of any plane, it is definitively outside the region and rejected.  
    Thus the class implements a test where all the
    planes face inward; if you want to tell if the object is outside of a region you'll
    have to reverse the sense of the flags returned.

    Objects inside the region always return a true test result, and objects outside the
    region always return a false test result.  Objects straddling a boundary will
    return the test result of the flags that you passed in.  In this manner, you get two
    sorts of tests depending on what you pass in:
    -If you pass in @c VisitorFlags(false) you will only get back a true TestResult if the
       object is completely within the region.
    -If you pass in a @c VisitorFlags(true) you will only get back a false TestResult if the
       object is completely outside the region.
    These two possibilities have different uses depending on whether or not you're
    determining potentially visible objects or completely obscured objects.

    Also included is the concept of 'active planes' for recursive object testing 
    (i.e., octrees and quadtrees).  When a given tree node is determined to be fully on the
    positive side of a given plane X, you know all the child objects will be fully
    on the positive side of plane X as well, so there is no need to test any child objects
    against plane X since you already know the test will pass.  
    When there are no active planes left you know the
    object and all its children are fully determined, so you can
    trivially add them all without doing any more clipping checks.
*/
#include "mathlib/plane.h"
#include "mathlib/sphere.h"
#include "mathlib/bbox.h"

// this is currently limited to leave some bitflags available in the VisitorFlags struct
// NOTE: keep this up-to-date, so visitors can add flags on the higher bits
#define N_PLANECLIPPER_MAXPLANES 16

class nPlaneClipper {
public:
    /// A default constructed clipper will always return true for it clipping test.
    nPlaneClipper();
    /// copy constructor
    nPlaneClipper(const nPlaneClipper &copyme);
    /// Construct the clipper with a set of planes.
    nPlaneClipper(const plane *clipplanes, int numplanes);

    /// Test a bounding box against the planes.  Returns true if the geometry is 
    /// completely contained within the region surrounded by the planes.  Returns
    /// false if the geometry is definitively outside the region.  Returns whatever
    /// TestResult() state you passed in if the geometry is not definitively inside or outside of
    /// the region, possibly with some of the active_flags turned off.
    VisitorFlags TestBBox(const bbox3 &boxtest, VisitorFlags in);

    /// Test a bounding sphere against the planes.  Returns true if the geometry is 
    /// completely contained within the region surrounded by the planes.  Returns
    /// false if the geometry is definitively outside the region.  Returns whatever
    /// TestResult() state you passed in if the geometry is not definitively inside or outside of
    /// the region, possibly with some of the active_flags turned off.
    VisitorFlags TestSphere(const sphere &spheretest, VisitorFlags in);

    /// Test a point against the planes.  Returns true if the geometry is 
    /// completely or partially contained within the region surrounded by the planes.
    VisitorFlags TestPoint(const vector3 &pointtest, VisitorFlags in);

protected:
    plane m_planes[N_PLANECLIPPER_MAXPLANES];
    int m_numplanes;
};

inline
nPlaneClipper::nPlaneClipper()
{
}

inline
nPlaneClipper::nPlaneClipper(const nPlaneClipper &copyme)
{
    m_numplanes = copyme.m_numplanes;

    for (int i=0; i < copyme.m_numplanes; i++)
        m_planes[i] = copyme.m_planes[i];
}


inline
nPlaneClipper::nPlaneClipper(const plane *clipplanes, int numplanes)
: m_numplanes(numplanes)
{
    // initialize from user-supplied planes
    for (int i=0; i < numplanes; i++)
    {
        m_planes[i] = clipplanes[i];
    }
}

inline
VisitorFlags nPlaneClipper::TestBBox(const bbox3 &boxtest, VisitorFlags in)
{
    // Returns a visibility code indicating the culling status of the
    // given axis-aligned box.  The result_info passed in should indicate
    // which planes might cull the box, by setting the corresponding
    // bit in in.active_planes.
    vector3 center = boxtest.center();
    vector3 extent = boxtest.extents();

    // Check the box against each active frustum plane.
    int bit = 1;
    int allplanebits( (1<<m_numplanes) - 1 );
    for (int i = 0; i < m_numplanes; i++, bit <<= 1)
    {
        // do the test only if the specified plane is active
        if ((bit & in.m_activeflags) != 0)
        {
            const plane&    p       = m_planes[i];
            const vector3 normal    = p.normal();
            
            // Check box against this plane.
            float   d = normal % center + p.d; //Calculate closest distance from center point to plane.
            float   extent_toward_plane = n_abs(extent.x * normal.x)
                + n_abs(extent.y * normal.y)
                + n_abs(extent.z * normal.z);
            if (d < 0)
            {  
                if (-d > extent_toward_plane)
                {
                    // Box is definitively on the negative side of the plane, so it's culled
                    return VisitorFlags(false,true);
                } // else this plane is ambiguous so leave it active.
            } else {
                if (d > extent_toward_plane)
                {
                    // Box is accepted by this plane, so
                    // deactivate it, since neither this
                    // box or any contained part of it can
                    // ever be culled by this plane.
                    in.m_activeflags &= ~bit;
                    if ( (in.m_activeflags & allplanebits) == 0)
                    {
                        // This box is definitively inside all the culling
                        // planes, so there's no need to continue.
                        return VisitorFlags(true,false);
                    }
                } // else this plane is ambigious so leave it active.
            }
        }
    }

    return in;  // Box not definitively culled.  Return updated active plane flags.
}

inline
VisitorFlags nPlaneClipper::TestSphere(const sphere &spheretest, VisitorFlags ri)
{
    // build a bbox from the sphere and test that
    bbox3 mybbox(spheretest.p, vector3(spheretest.r, spheretest.r, spheretest.r));
    return TestBBox(mybbox, ri);
}

inline
VisitorFlags nPlaneClipper::TestPoint(const vector3 &pointtest, VisitorFlags ri)
{
    // build a bbox from the point and test that
    bbox3 mybbox(pointtest, vector3(0,0,0));
    return TestBBox(mybbox, ri);
}

#endif
