#ifndef N_VISIBLEFRUSTUMVISITOR_H
#define N_VISIBLEFRUSTUMVISITOR_H

/**
    @class nVisibleFrustumVisitor
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view frustum, useful for
    rendering to a camera.

    This visitor finds all the elements possible visible from a given
    viewpoint within a view frustum.  This is useful for determining
    objects that appear on screen with a given projective camera.

    The visitor will call Visit() for every element determined possibly
    visible.
*/

#include "spatialdb/nvisitorbase.h"
#include "spatialdb/nfrustumclipper.h"
#include "spatialdb/nspatialelements.h"

class nGfxServer2;

class nVisibleFrustumVisitor : public nVisibilityVisitor {
public:
    nVisibleFrustumVisitor(const nCamera2 &cameraprojection, const matrix44 &cameratransform); 
    ~nVisibleFrustumVisitor();

    /// reset any accumulated state.  The view frustum is unchanged
    void Reset();

    /// reset any data and reposition the frustum.
    void Reset(const nCamera2 &newcamera, const matrix44 &newxform);

    void Visit(nSpatialElement *visitee);

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

    // leave a local space
    virtual void LeaveLocalSpace();

protected:
    // represents the camera position/rotation in the current space.
    // changes if we enter a sector with different local coordinate system
    nArray<matrix44> m_viewertransformstack;

    // represents the clipping planes of the view frustum
    // changes if we enter a sector with different local coordinate system
    nArray<nFrustumClipper> m_viewfrustumstack;

    // represents the camera projection
    // this typically does not change as we wander through coordinate spaces, so we have a member, not a stack
    nCamera2 m_cameraprojection;

    matrix44 &GetCameraTransform() const;
    nFrustumClipper &GetFrustumClipper() const;
};

inline
matrix44 &nVisibleFrustumVisitor::GetCameraTransform() const
{
    return m_viewertransformstack.Back();
}

inline
nFrustumClipper &nVisibleFrustumVisitor::GetFrustumClipper() const
{
    return m_viewfrustumstack.Back();
}

#endif
