#ifndef N_GEOMETRYOCCLUSIONVISITOR_H
#define N_GEOMETRYOCCLUSIONVISITOR_H

// header file for the geometry-vased occlusion visitor
// (C) 2004 Gary Haussmann
// This code is licensed under the Nebula License

#include "nvisitorbase.h"
#include "nplaneclipper.h"

/**
 * @class nGeometryOcclusionVisitor
 * @ingroup NSpatialDBContribModule
 *
 * @brief A simple occlusion manager using clip planes to clip out occluded objects.
 *
 * The geometry-based occluder does everything using geometry operations
 * in world space.  Basically, any occluder is represented as a set of clipping planes
 * that clip out any objects behind the occluder.  A visibility test is done by
 * checking that the object or region to be tested is not occluded by any
 * of the currently stored occluders.
 *
 * The visitor can be configured to track all occluders if you want, but unless you
 * are stingy with your occluders you will probably want to keep the N most "important"
 * occluders, where importance is measured by potential occlusion area A,
 * where A = (occluder size)/(occluder distance to viewpoint).  Thus, bigger and closer
 * occluders are preferred.  Also, a non-visible occluder is ignored.  No
 * occluder fusion is performed.
 *
 * Since the occlusion tests are all done in world space, not image space, you can do
 * occlusion tests for non-image based views, such as finding LOS of a sphere (i.e.,
 * everything visible within 100m).  It also doesn't need to figure out a good resolution
 * to use for an occlusion maps, hierarchical z-buffer, or any such thing.
 *
 */
class nGeometryOcclusionVisitor : public nOcclusionVisitor {
public:
    nGeometryOcclusionVisitor(const vector3 &vp, nVisibilityVisitor *v = NULL);
    ~nGeometryOcclusionVisitor();

    void Reset();
    void Reset(const vector3 &newvp);

    void StartVisualizeDebug(nGfxServer2 *gfx2);

    void EndVisualizeDebug();

    void Visit(nSpatialElement *visitee);

    /// Add an occluder shaped like a AABB to the occluding set
    void AddBBoxOccluder(const bbox3 &boxoccluder);
    /// Add an occluder shaped like a sphere to the occluding set
    void AddSphereOccluder(const sphere &sphereoccluder);
    /// Add an occluder shaped like a convex hull (represented by a set of halfspaces) to the occluding set
    void AddHullOccluder(const nPlaneClipper &hulloccluder);

    /// Test of a given object AABB is visible or occluded
    VisitorFlags VisibilityTest(const bbox3 &testbox, VisitorFlags flags);
    /// Test of a given object sphere is visible or occluded
    VisitorFlags VisibilityTest(const sphere &testsphere, VisitorFlags flags);

    /// enter a local space; typically used for entering other cells/portals
    void EnterLocalSpace(matrix44 &warp);

    /// leave a local space
    void LeaveLocalSpace();

protected:
    // the occluders, stored as plane clippers
    nArray<nPlaneClipper> m_occluderset;

    // vertex endpoints of lines to draw for debugging purposes
    nArray<vector3> m_debuglines;
};

#endif