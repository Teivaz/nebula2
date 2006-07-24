//--------------------------------------------------
// nOccludedFrustumVisitor.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nOccludedFrustumVisitor.h"
#include "gfx2/ngfxserver2.h"

nOccludedFrustumVisitor::nOccludedFrustumVisitor(const nCamera2 &cameraprojection,
                                                 const matrix44 &cameratransform,
                                                 nOcclusionVisitor &occlusionvisitor)
: nVisibleFrustumVisitor(cameraprojection, cameratransform), m_occlusionvisitor(occlusionvisitor),
  m_frustumvisitor(cameraprojection, cameratransform)
{
    // If we don't provide some sort of clipping to the occlusion visitor, it will try to
    // collect all the occluders it finds, instead of ignoring the occluders outside of the
    // view frustum.  So we provide a copy of our local view frustum clipper and thus restrict
    // the region from which occluders can be gathered.
    m_occlusionvisitor.SetRestrictingVisitor(&m_frustumvisitor);
}

nOccludedFrustumVisitor::~nOccludedFrustumVisitor()
{
}

void nOccludedFrustumVisitor::Reset()
{
    nVisibleFrustumVisitor::Reset();
    m_frustumvisitor.Reset();
}

void nOccludedFrustumVisitor::Reset(const nCamera2 &newcamera, const matrix44 &newxform)
{
    nVisibleFrustumVisitor::Reset();
    m_frustumvisitor.Reset(newcamera, newxform);
}

void nOccludedFrustumVisitor::StartVisualizeDebug(nGfxServer2 *gfx2)
{
    nVisibilityVisitor::StartVisualizeDebug(gfx2);
    m_viewfrustumstack.Back().VisualizeFrustum(gfx2, vector4(1.0,0.0,1.0,0.5));
    m_occlusionvisitor.StartVisualizeDebug(gfx2);
}

void nOccludedFrustumVisitor::EndVisualizeDebug()
{
    nVisibilityVisitor::EndVisualizeDebug();
    m_occlusionvisitor.EndVisualizeDebug();
}

VisitorFlags nOccludedFrustumVisitor::VisibilityTest(const bbox3 &testbox, VisitorFlags flags)
{
    return m_occlusionvisitor.VisibilityTest(testbox, flags);
}

VisitorFlags nOccludedFrustumVisitor::VisibilityTest(const sphere &testsphere, VisitorFlags flags)
{
    return m_occlusionvisitor.VisibilityTest(testsphere, flags);
}

void nOccludedFrustumVisitor::EnterLocalSpace(matrix44 &warp)
{
    // transform the camera and generate a new frustum for the local space
    matrix44 newtransform = warp * GetCameraTransform();
    m_viewertransformstack.PushBack(newtransform);

    nFrustumClipper newfrustum(m_cameraprojection, newtransform);
    m_viewfrustumstack.PushBack(newfrustum);

    nVisibilityVisitor::EnterLocalSpace(warp);

    m_occlusionvisitor.EnterLocalSpace(warp);
}

void nOccludedFrustumVisitor::LeaveLocalSpace()
{
    m_viewertransformstack.Erase(m_viewertransformstack.End()-1);
    m_viewfrustumstack.Erase(m_viewfrustumstack.End()-1);

    nVisibilityVisitor::LeaveLocalSpace();

    m_occlusionvisitor.LeaveLocalSpace();
}

