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

#include "spatialdb/nvisitorbase.h"
#include "spatialdb/nsphereclipper.h"
#include "spatialdb/nspatialelements.h"

class nVisibleSphereVisitor : public nVisibilityVisitor {
public:
    nVisibleSphereVisitor(const sphere &viewsphere);
    ~nVisibleSphereVisitor();

    virtual void Reset();

    /// Reset any data and reposition the sphere
    void Reset(const sphere &viewsphere);

    virtual void Visit(nSpatialElement *visitee);

    /** Check if an element should be culled.  If the returned VisitorFlags produces
    a TestResult() of true, the element is visible.  The returned VisitorFlags can be used
    for more efficient visibility tests of primitives enclosed in this bounding box */
    virtual VisitorFlags VisibilityTest(const bbox3 &testbox, VisitorFlags flags);

    /** Check if an element should be culled.  If the returned VisitorFlags produces
    a TestResult() of true, the element is visible.  The returned VisitorFlags can be used
    for more efficient visibility tests of primitives enclosed in this bounding sphere */
    virtual VisitorFlags VisibilityTest(const sphere &testsphere, VisitorFlags flags);

    void StartVisualizeDebug(nGfxServer2 *gfx2);

    // entering a new local space; the matrix given will transform from the current local system into
    // the new space local coordinate system.  This is used to possibly update a transform matrix
    // or to transform the spatial region to a new coordinate system.
    virtual void EnterLocalSpace(matrix44 &warp);

    /// leave a local space
    virtual void LeaveLocalSpace();

protected:
    // represents the view sphere
    // changes if we enter a sector with different local coordinate system
    nArray<sphere> m_viewspherestack;
    nArray<nSphereClipper> m_sphereclipperstack;

    /// gets the current sphere clipper used
    nSphereClipper &GetSphereClipper() const;

};

inline
nSphereClipper &nVisibleSphereVisitor::GetSphereClipper() const
{
    n_assert(m_sphereclipperstack.Size() > 0);
    return m_sphereclipperstack.Back();
}

#endif
