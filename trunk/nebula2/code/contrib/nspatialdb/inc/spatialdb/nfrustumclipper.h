#ifndef N_FRUSTUMCLIPPER_H
#define N_FRUSTUMCLIPPER_H

/**
   @class nFrustumClipper
   @brief Encapsulates the clipping planes of a frustum, with handling of 'active clipping plane' flags.

   This is just a simple frustum class, allowing you to construct a frustum and test object against it.
   Also included is the concept of 'active planes' for recursive object testing (i.e., octrees and quadtrees).
   When a given tree node is determine to be fully on the inside of a given plane, you know all the child
   objects will be fully inside that plane as well so there is no need to test any child objects against
   that plane.  When there are no active planes, you know the object and all its children are fully inside the frustum
   and so can trivially add them all without doing any more clipping checks.
*/

#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"
#include "mathlib/plane.h"
#include "mathlib/sphere.h"
#include "mathlib/bbox.h"

class nFrustumClipper {
public:
    nFrustumClipper();
    nFrustumClipper(const nFrustumClipper &copyme);
    nFrustumClipper(nCamera2 &camera, const matrix44 &viewtransform);
    nFrustumClipper(const plane clipplanes[6]);

    // helper class for culling
    class result_info {
        public:
            bool    culled; // true when the volume is not visible
            unsigned char   active_planes;  // one bit per frustum plane
        
            //0x3F=b111111 => 6 planes for view frustum. Front, back and 4 side planes. (Pyramid with top cut off).     
            result_info(bool c = false, unsigned char a = 0x3f) : 
                culled(c), active_planes(a) 
            { }
    };

    result_info TestBBox(const bbox3 &boxtest, result_info in);
    result_info TestSphere(const sphere &spheretest, result_info in);
    result_info TestPoint(const vector3 &pointtest, result_info in);

    void VisualizeFrustum(nGfxServer2 *gfx2, const vector4 &color);

protected:
    plane m_planes[6];

    // for debug visualizing
    vector3 m_frustumcorners[8];
};

inline
nFrustumClipper::nFrustumClipper()
{
}

inline
nFrustumClipper::nFrustumClipper(const nFrustumClipper &copyme)
{
    for (int i=0; i < 6; i++)
        m_planes[i] = copyme.m_planes[i];
    for (int i2=0; i2 < 8; i2++)
        m_frustumcorners[i2] = copyme.m_frustumcorners[i2];
}


inline
nFrustumClipper::nFrustumClipper(nCamera2 &camera, const matrix44 &viewtransform)
{
    // initialize clip planes from the projection and transform info
    // stolen from nOctFrustum code, 
    matrix44 proj = camera.GetProjection();
    matrix44 m = viewtransform * proj;

    // front
    m_planes[0].a = m.M13;
    m_planes[0].b = m.M23;
    m_planes[0].c = m.M33;
    m_planes[0].d = m.M43;

    // back
    m_planes[1].a = (m.M14 - m.M13);
    m_planes[1].b = (m.M24 - m.M23);
    m_planes[1].c = (m.M34 - m.M33);
    m_planes[1].d = (m.M44 - m.M43);

    // left
    m_planes[2].a = (m.M14 + m.M11);
    m_planes[2].b = (m.M24 + m.M21);
    m_planes[2].c = (m.M34 + m.M31);
    m_planes[2].d = (m.M44 + m.M41);

    // right
    m_planes[3].a = (m.M14 - m.M11);
    m_planes[3].b = (m.M24 - m.M21);
    m_planes[3].c = (m.M34 - m.M31);
    m_planes[3].d = (m.M44 - m.M41);

    // top
    m_planes[4].a = (m.M14 - m.M12);
    m_planes[4].b = (m.M24 - m.M22);
    m_planes[4].c = (m.M34 - m.M32);
    m_planes[4].d = (m.M44 - m.M42);

    // bottom
    m_planes[5].a = (m.M14 + m.M12);
    m_planes[5].b = (m.M24 + m.M22);
    m_planes[5].c = (m.M34 + m.M32);
    m_planes[5].d = (m.M44 + m.M42);

    //normalize planes.
    float denom;
    vector3 tmp;
    
    for (int i=0; i<6; i++)
    {
        tmp.set (m_planes[i].a, m_planes[i].b, m_planes[i].c);
        denom = 1.0f / tmp.len();
        m_planes[i].a *= denom;
        m_planes[i].b *= denom;
        m_planes[i].c *= denom;
        m_planes[i].d *= denom;
    }

    // generate frustum corners, for debugging
    matrix44 projectback(m);
    projectback.invert();
    m_frustumcorners[0] = projectback * vector3(1,1,0);
    m_frustumcorners[1] = projectback * vector3(1,1,1);
    m_frustumcorners[2] = projectback * vector3(-1,1,0);
    m_frustumcorners[3] = projectback * vector3(-1,1,1);
    m_frustumcorners[4] = projectback * vector3(1,-1,0);
    m_frustumcorners[5] = projectback * vector3(1,-1,1);
    m_frustumcorners[6] = projectback * vector3(-1,-1,0);
    m_frustumcorners[7] = projectback * vector3(-1,-1,1);
}

inline
nFrustumClipper::nFrustumClipper(const plane clipplanes[6])
{
    // initialize from user-supplied planes
    for (int i=0; i < 6; i++)
    {
        m_planes[i] = clipplanes[i];
    }
}

inline
nFrustumClipper::result_info nFrustumClipper::TestBBox(const bbox3 &boxtest, result_info in)
{
    // Returns a visibility code indicating the culling status of the
    // given axis-aligned box.  The result_info passed in should indicate
    // which planes might cull the box, by setting the corresponding
    // bit in in.active_planes.
    vector3 center = boxtest.center();
    vector3 extent = boxtest.extents();

    // Check the box against each active frustum plane.
    int bit = 1;
    for (int i = 0; i < 6; i++, bit <<= 1)
    {
        // do the test only if the plane is still not definately containing/clipping the box
        if ((bit & in.active_planes) != 0)
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
                    // Box is culled by plane; it's not visible.
                    return result_info(true, 0);
                } // else this plane is ambiguous so leave it active.
            } else {
                if (d > extent_toward_plane)
                {
                    // Box is accepted by this plane, so
                    // deactivate it, since neither this
                    // box or any contained part of it can
                    // ever be culled by this plane.
                    in.active_planes &= ~bit;
                    if (in.active_planes == 0)
                    {
                        // This box is definitively inside all the culling
                        // planes, so there's no need to continue.
                        return in;
                    }
                } // else this plane is ambigious so leave it active.
            }
        }
    }

    return in;  // Box not definitively culled.  Return updated active plane flags.
}

inline
nFrustumClipper::result_info nFrustumClipper::TestSphere(const sphere &spheretest, nFrustumClipper::result_info ri)
{
    // build a bbox from the sphere and test that
    bbox3 mybbox(spheretest.p, vector3(spheretest.r, spheretest.r, spheretest.r));
    return TestBBox(mybbox, ri);
}

inline
nFrustumClipper::result_info nFrustumClipper::TestPoint(const vector3 &pointtest, nFrustumClipper::result_info ri)
{
    // build a bbox from the point and test that
    bbox3 mybbox(pointtest, vector3(0,0,0));
    return TestBBox(mybbox, ri);
}

inline
void nFrustumClipper::VisualizeFrustum(nGfxServer2 *gfx2, const vector4 &color)
{
    gfx2->BeginLines();
    gfx2->DrawLines3d(m_frustumcorners, 8, color);
    gfx2->EndLines();
}

#endif

