//--------------------------------------------------
// nvisiblespherevisitor.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nvisiblespherevisitor.h"

nVisibleSphereVisitor::nVisibleSphereVisitor(const sphere &viewsphere)
: nVisibilityVisitor(viewsphere.p)
{
   // initialize the view sphere
   m_viewspherestack.PushBack(viewsphere);
   m_sphereclipperstack.PushBack(nSphereClipper(viewsphere));
}

nVisibleSphereVisitor::~nVisibleSphereVisitor()
{
}

void nVisibleSphereVisitor::Reset()
{
  // the stacks should be size 1 at this point
  n_assert(m_viewspherestack.Size() == 1);
  n_assert(m_sphereclipperstack.Size() == 1);

  nVisibilityVisitor::Reset();
}

void nVisibleSphereVisitor::Reset(const sphere &viewsphere)
{
  nVisibilityVisitor::Reset(viewsphere.p);
  m_viewspherestack.Clear();
  m_sphereclipperstack.Clear();

  m_viewspherestack.PushBack(viewsphere);
  m_sphereclipperstack.PushBack(nSphereClipper(viewsphere));

}

void nVisibleSphereVisitor::StartVisualizeDebug(nGfxServer2 *gfx2)
{
    nVisibilityVisitor::StartVisualizeDebug(gfx2);
    vector4 spherecolor(0.0f,0.7f,0.9f,0.2f);
    m_sphereclipperstack.Back().VisualizeSphere(gfx2, spherecolor);
}

void nVisibleSphereVisitor::Visit(nSpatialElement *visitee)
{
    // empty -- typically overridden in subclasses
}

VisitorFlags nVisibleSphereVisitor::VisibilityTest(const bbox3 &testbox, VisitorFlags flags)
{
    // test against the current sphere clipper
    nSphereClipper &clipper = GetSphereClipper();

    VisitorFlags out;
    out = clipper.TestBBox( testbox, flags);

    return out;
}

VisitorFlags nVisibleSphereVisitor::VisibilityTest(const sphere &testsphere, VisitorFlags flags)
{
    // test against the current sphere clipper
    nSphereClipper &clipper = GetSphereClipper();

    VisitorFlags out;
    out = clipper.TestSphere( testsphere, flags);

    return out;
}

void nVisibleSphereVisitor::EnterLocalSpace(matrix44 &warp)
{
    // transform the sphere and generate a new clipper for the local space
    sphere oldsphere(m_viewspherestack.Back());
    // argh, must assume all scales are the same!
    float newradius( (warp*vector3(0,0,oldsphere.r)).len() );
    sphere newsphere(warp * oldsphere.p, newradius);

    nSphereClipper newclipper(newsphere);

    m_viewspherestack.PushBack(newsphere);
    m_sphereclipperstack.PushBack(newclipper);

    nVisibilityVisitor::EnterLocalSpace(warp);
}

void nVisibleSphereVisitor::LeaveLocalSpace()
{
    m_viewspherestack.Erase(m_viewspherestack.End()-1);
    m_sphereclipperstack.Erase(m_sphereclipperstack.End()-1);

    nVisibilityVisitor::LeaveLocalSpace();
}

