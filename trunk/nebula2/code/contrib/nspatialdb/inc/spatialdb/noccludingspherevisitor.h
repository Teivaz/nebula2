#ifndef N_OCCLUDINGSPHEREVISITOR_H
#define N_OCCLUDINGSPHEREVISITOR_H

/**
    @class nOccludingSphereVisitor
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view sphere, useful for
    determining a character's LOS.

    This visitor finds all the elements possible visible from a given
    viewpoint within a view sphere.  This is useful for determining
    objects visible by a character in all directions.  This version also
    takes into account bounding box occluders

    The visitor will call Visit() for every element determined possibly
    visible.
*/

#include "spatialdb/nvisiblespherevisitor.h"
#include "spatialdb/nsphereclipper.h"
#include "spatialdb/nspatialelements.h"
#include "spatialdb/nspatialsector.h"
#include "spatialdb/nbboxoccluder.h"
#include "util/narray.h"

class nOccludingSphereVisitor : public nVisibleSphereVisitor {
public:
    nOccludingSphereVisitor(const sphere &viewsphere);
    ~nOccludingSphereVisitor();

    virtual void Reset();

    virtual void Visit(nSpatialSector *visitee, int recursedepth);

    virtual bool VisibilityTest(nSpatialElement *visitee);

protected:
    /// holds the current occluders we are using; currently we only support sphere occluders
    nArray<nBBoxOccluder> m_occluders;

    // entering a new local space; the matrix given will transform from the current local system into
    // the new space local coordinate system.  This is used to possibly update a transform matrix
    // or to transform the spatial region to a new coordinate system.
    virtual void EnterLocalSpace(matrix44 &warp);

    /// leave a local space
    virtual void LeaveLocalSpace();

    /// recursive descent of the octree embedded inside a sector
    void CheckOctNode(nOctNode *testnode, nSphereClipper &clipper, nSphereClipper::result_info clipstatus, int recursivedepth);

    /// recursive collection of occluders in this octree
    int CollectOccluders(nOctNode *collectnode, nSphereClipper &clipper, nSphereClipper::result_info clipstatus, int maxoccluders);

    /// checks if a given sphere is blocked by any occluders
    bool IsOccluded(const sphere &testsphere);
};




#endif
