//--------------------------------------------------
// nvisiblefrustumvisitor.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nvisiblefrustumvisitor.h"
#include "gfx2/ngfxserver2.h"

nVisibleFrustumVisitor::nVisibleFrustumVisitor(const nCamera2 &cameraprojection, const matrix44 &cameratransform)
: nVisibilityVisitor(cameratransform.pos_component()), m_cameraprojection(cameraprojection)
{
     // initialize the view frustum, matrix, etc.

     m_viewertransformstack.PushBack(cameratransform);

     nFrustumClipper clipper(m_cameraprojection, cameratransform);
     m_viewfrustumstack.PushBack(clipper);
}

nVisibleFrustumVisitor::~nVisibleFrustumVisitor()
{
}

void nVisibleFrustumVisitor::Reset()
{
    // the stacks should be size 1 at this point
    n_assert(m_viewertransformstack.Size() == 1);
    n_assert(m_viewfrustumstack.Size() == 1);

    nVisibilityVisitor::Reset();
}

void nVisibleFrustumVisitor::Reset(const nCamera2 &newcamera, const matrix44 &newxform)
{
    m_cameraprojection = newcamera;
    nVisibilityVisitor::Reset(newxform.pos_component());

    m_viewertransformstack.Clear();
    m_viewfrustumstack.Clear();

    m_viewertransformstack.PushBack(newxform);

    nFrustumClipper clipper(m_cameraprojection, newxform);
    m_viewfrustumstack.PushBack(clipper);
}

void nVisibleFrustumVisitor::StartVisualizeDebug(nGfxServer2 *gfx2)
{
    nVisibilityVisitor::StartVisualizeDebug(gfx2);
    m_viewfrustumstack.Back().VisualizeFrustum(gfx2, vector4(1.0,0.0,1.0,0.5));
}

void nVisibleFrustumVisitor::Visit(nSpatialElement *visitee)
{
}

VisitorFlags nVisibleFrustumVisitor::VisibilityTest(const bbox3 &testbox, VisitorFlags flags)
{
    nFrustumClipper &clipper = GetFrustumClipper();
    return clipper.TestBBox(testbox, flags);
}

VisitorFlags nVisibleFrustumVisitor::VisibilityTest(const sphere &testsphere, VisitorFlags flags)
{
    // test against the current frustum clipper
    nFrustumClipper &clipper = GetFrustumClipper();
    return clipper.TestSphere(testsphere, flags);
}

void nVisibleFrustumVisitor::EnterLocalSpace(matrix44 &warp)
{
    // transform the camera and generate a new frustum for the local space
    matrix44 newtransform = warp * GetCameraTransform();
    m_viewertransformstack.PushBack(newtransform);

    nFrustumClipper newfrustum(m_cameraprojection, newtransform);
    m_viewfrustumstack.PushBack(newfrustum);

    nVisibilityVisitor::EnterLocalSpace(warp);
}

void nVisibleFrustumVisitor::LeaveLocalSpace()
{
    m_viewertransformstack.Erase(m_viewertransformstack.End()-1);
    m_viewfrustumstack.Erase(m_viewfrustumstack.End()-1);

    nVisibilityVisitor::LeaveLocalSpace();
}

