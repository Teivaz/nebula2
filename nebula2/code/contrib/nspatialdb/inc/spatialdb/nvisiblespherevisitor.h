#ifndef N_VISIBLESPHEREVISITOR_H
#define N_VISIBLESPHEREVISITOR_H

/**
    @class nVisibleSphereVisitor
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view sphere, useful for
    determining a character's LOS.

    This visitor finds all the elements possible visible from a given
    viewpoint within a view sphere.  This is useful for determining
    objects visible by a character in all directions.

    The visitor will call Visit() for every element determined possibly
    visible.
*/

#include "spatialdb/nvisibilityvisitor.h"
#include "spatialdb/nsphereclipper.h"
#include "spatialdb/nspatialelements.h"
#include "spatialdb/nspatialsector.h"

class nVisibleSphereVisitor : public nVisibilityVisitor {
public:
    nVisibleSphereVisitor(const sphere &viewsphere);
    ~nVisibleSphereVisitor();

    virtual void Reset();

    virtual void Visit(nSpatialSector *visitee, int recursedepth);

    virtual bool VisibilityTest(nSpatialElement *visitee);

protected:
    // represents the view sphere
    // changes if we enter a sector with different local coordinate system
    nArray<sphere> m_viewspherestack;
    nArray<nSphereClipper> m_sphereclipperstack;

    /// gets the current sphere clipper used
    nSphereClipper &GetSphereClipper() const;

    // entering a new local space; the matrix given will transform from the current local system into
    // the new space local coordinate system.  This is used to possibly update a transform matrix
    // or to transform the spatial region to a new coordinate system.
    virtual void EnterLocalSpace(matrix44 &warp);

    /// leave a local space
    virtual void LeaveLocalSpace();

    /// recursive descent of the octree embedded inside a sector
    void CheckOctNode(nOctNode *testnode, nSphereClipper &clipper, nSphereClipper::result_info clipstatus, int recursivedepth);
};

inline
nSphereClipper &nVisibleSphereVisitor::GetSphereClipper() const
{
    n_assert(m_sphereclipperstack.Size() > 0);
    return m_sphereclipperstack.Back();
}

#endif
