#ifndef N_FRUSTUMCLIPPER_H
#define N_FRUSTUMCLIPPER_H

/**
    @class nFrustumClipper
    @ingroup NSpatialDBContribModule
    @brief Encapsulates the clipping planes of a frustum, with handling
    of 'active clipping plane' flags.

    This is just a simple frustum class, allowing you to construct a
    frustum and test object against it.  Also included is the concept
    of 'active planes' for recursive object testing (i.e., octrees and
    quadtrees).  When a given tree node is determine to be fully on the
    inside of a given plane, you know all the child objects will be fully
    inside that plane as well so there is no need to test any child objects
    against that plane.  When there are no active planes, you know the
    object and all its children are fully inside the frustum and so can
    trivially add them all without doing any more clipping checks.
*/

#include "spatialdb/nplaneclipper.h"
#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"

class nFrustumClipper : public nPlaneClipper {
public:
    nFrustumClipper();
    nFrustumClipper(const nFrustumClipper &copyme);
    nFrustumClipper(nCamera2 &camera, const matrix44 &viewtransform);
    nFrustumClipper(const plane clipplanes[6]);

    void VisualizeFrustum(nGfxServer2 *gfx2, const vector4 &color);

protected:

    // for debug visualizing
    vector3 m_frustumcorners[8];
};

inline
nFrustumClipper::nFrustumClipper() : nPlaneClipper()
{
}

inline
nFrustumClipper::nFrustumClipper(const nFrustumClipper &copyme)
    : nPlaneClipper(copyme)
{

    for (int i2=0; i2 < 8; i2++)
        m_frustumcorners[i2] = copyme.m_frustumcorners[i2];
}


inline
nFrustumClipper::nFrustumClipper(nCamera2 &camera, const matrix44 &viewtransform)
: nPlaneClipper()
{

    m_numplanes = 6;

    // initialize clip planes from the projection and transform info
    // stolen from nOctFrustum code, 
    matrix44 proj(camera.GetProjection());
    matrix44 m(viewtransform);
    m.invert();
    m *= proj;

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
    matrix44 projectback(viewtransform);
    float minx, maxx, miny, maxy, minz, maxz;
    camera.GetViewVolume(minx,maxx,miny,maxy,minz,maxz);
    float nfscale = maxz/minz;
    m_frustumcorners[0] = projectback * (vector3(minx,miny,-minz) * 1.0f);
    m_frustumcorners[1] = projectback * (vector3(minx,miny,-minz) * nfscale);
    m_frustumcorners[2] = projectback * (vector3(minx,maxy,-minz) * 1.0f);
    m_frustumcorners[3] = projectback * (vector3(minx,maxy,-minz) * nfscale);
    m_frustumcorners[4] = projectback * (vector3(maxx,miny,-minz) * 1.0f);
    m_frustumcorners[5] = projectback * (vector3(maxx,miny,-minz) * nfscale);
    m_frustumcorners[6] = projectback * (vector3(maxx,maxy,-minz) * 1.0f);
    m_frustumcorners[7] = projectback * (vector3(maxx,maxy,-minz) * nfscale);
}

inline
nFrustumClipper::nFrustumClipper(const plane clipplanes[6]) : nPlaneClipper(clipplanes, 6)
{
}


inline
void nFrustumClipper::VisualizeFrustum(nGfxServer2 *gfx2, const vector4 &color)
{
    // build two line strips of 8 points each; each line strip draws two faces of the frustum
    vector3 renderpoints[16] ={
        m_frustumcorners[0], m_frustumcorners[1], m_frustumcorners[5], m_frustumcorners[4],
        m_frustumcorners[0], m_frustumcorners[2], m_frustumcorners[6], m_frustumcorners[4],
        m_frustumcorners[3], m_frustumcorners[1], m_frustumcorners[5], m_frustumcorners[7],
        m_frustumcorners[3], m_frustumcorners[2], m_frustumcorners[6], m_frustumcorners[7],
    };

    gfx2->BeginLines();
    gfx2->DrawLines3d(renderpoints, 8, color);
    gfx2->DrawLines3d(renderpoints+8, 8, color);
    gfx2->EndLines();
}

#endif

