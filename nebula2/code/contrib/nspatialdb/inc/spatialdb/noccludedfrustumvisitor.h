#ifndef N_OCCLUDEDFRUSTUMVISITOR_H
#define N_OCCLUDEDFRUSTUMVISITOR_H

#include "nvisiblefrustumvisitor.h"

/**
    @class nOccludedFrustumVisitor
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view frustum, useful for
    rendering to a camera.

    This visitor finds all the visible elements within a view frustum,
    utilizing the occlusion visitor you provide to cull out occluded
    elements.  It is assumed that you have populated the occlusion
    visitor with occluders already.  This visitor won't modify the
    state of the occlusion visitor (i.e., by adding or removing occluers),
    it merely uses the occlusion visitor for visibility tests.

    The visitor will call Visit() for every element determined possibly
    visible.

    Since you have to populate the occlusion visitor yourself, you must basically
    do two visit passes, once with the occlusion visitor to collect occluders, and
    once with this visitor to collect visible elements:
    
    @code

    nSpatialElement *mysetofelements = GetElements();
    nMyCoolOcclusionVisitorType myocclusionvisitor(viewpoint);
    nOccludedFrustumVisitor ofvisitor(camera, transform, myocclusionvisitor);

    ofvisitor.Reset();    // you should also reset the occlusion visitor you provided...
    myocclusionvisitor.Reset();

    mysetofelements->Accept(myocclusionvisitor);   // gathers occluders
    mysetofelements->Accept(ofvisitor);            // finds visible, non-occluded elements

    @endcode
*/
class nOccludedFrustumVisitor : public nVisibleFrustumVisitor {
public:
    nOccludedFrustumVisitor(const nCamera2 &cameraprojection, const matrix44 &cameratransform, nOcclusionVisitor &occlusionvisitor); 
    ~nOccludedFrustumVisitor();

    /// Reset any accumulated state; the view frustum is unchanged.  You have to reset the occlusion visitor as well.
    void Reset();

    /// Reset any data and reposition the frustum.  You have to reset the occlusion visitor as well.
    void Reset(const nCamera2 &newcamera, const matrix44 &newxform);

    // visit() is not overridden, so this is still an abstract class!

    /** Check if an element should be culled.  If the returned VisitorFlags produces
    a TestResult() of true, the element is visible.  The returned VisitorFlags can be used
    for more efficient visibility tests of primitives enclosed in this bounding box */
    VisitorFlags VisibilityTest(const bbox3 &testbox, VisitorFlags flags);
    VisitorFlags VisibilityTest(const sphere &testsphere, VisitorFlags flags);

    void StartVisualizeDebug(nGfxServer2 *gfx2);
    void EndVisualizeDebug();

    // entering a new local space; the matrix given will transform from the current local system into
    // the new space local coordinate system.  This is used to possibly update a transform matrix
    // or to transform the spatial region to a new coordinate system.
    void EnterLocalSpace(matrix44 &warp);

    // leave a local space
    void LeaveLocalSpace();

protected:
    nOcclusionVisitor &m_occlusionvisitor;
    nVisibleFrustumVisitor m_frustumvisitor; // for use by the occlusion visitor
};


#endif
