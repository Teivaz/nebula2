#ifndef N_OCCLUDINGFRUSTUMVISITOR_H
#define N_OCCLUDINGFRUSTUMVISITOR_H

/**
    @class nOccludingFrustumVisitor
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view frustum, factoring in
    occluder objects.

    This visitor finds all the elements possible visible from a given
    viewpoint within a view frustum.  This is useful for determining
    objects that appear on screen with a given projective camera.  The
    difference between this class and its superclass (nVisibleFrustumVisitor)
    is that it will grab occluders and use them to cull away objects.

    The visitor will call Visit() for every element determined possibly visible.
*/

#include "spatialdb/nvisiblefrustumvisitor.h"
#include "spatialdb/nbboxoccluder.h"

class nOccludingFrustumVisitor : public nVisibleFrustumVisitor {
public:
    nOccludingFrustumVisitor(nCamera2 &cameraprojection, matrix44 &cameratransform); 
    ~nOccludingFrustumVisitor();

    virtual void Reset();

    virtual void Visit(nSpatialSector *visitee, int recursedepth);

    virtual bool VisibilityTest(nSpatialElement *visitee);

protected:
    /// holds the current occluders we are using; currently we only support sphere occluders
    nArray<nBBoxOccluder> m_occluders;

    /**
      entering a new local space; the matrix given will transform
      from the current local system into the new space local
      coordinate system.  This is used to possibly update a transform matrix
      or to transform the spatial region to a new coordinate system.
    */
    virtual void EnterLocalSpace(matrix44 &warp);

    /// leave a local space
    virtual void LeaveLocalSpace();

    /// recursive descent of the octree embedded inside a sector
    void CheckOctNode(nOctNode *testnode, nFrustumClipper &clipper, nFrustumClipper::result_info clipstatus, int recursivedepth);

    /// recursive collection of occluders in this octree
    int CollectOccluders(nOctNode *collectnode, nFrustumClipper &clipper, nFrustumClipper::result_info clipstatus, int maxoccluders);

    /// checks if a given sphere is blocked by any occluders
    bool IsOccluded(const sphere &testsphere);
};

#endif
