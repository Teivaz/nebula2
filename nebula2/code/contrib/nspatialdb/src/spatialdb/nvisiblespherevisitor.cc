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

void nVisibleSphereVisitor::Visit(nSpatialSector *visitee, int recursedepth)
{
    // bug out if we're hit the bottom of our allowed recursion
    if (recursedepth < 1)
        return;

    //n_assert(visitee->GetOctree() != NULL);
    nOctNode *rootnode = visitee->/*GetOctree()->*/GetRoot();
    nSphereClipper::result_info clipinfo;
    nSphereClipper sphereclip = m_sphereclipperstack.Back();
    if (m_gfxdebug)
    {
        sphereclip.VisualizeSphere(m_gfxdebug, vector4(1.0f,1.0f,0.0f,0.5f));
    }

    // recursively descend the octree checking each node for clipping
    CheckOctNode(rootnode, sphereclip, clipinfo, recursedepth);
}

void nVisibleSphereVisitor::CheckOctNode(nOctNode *testnode, nSphereClipper &clipper, nSphereClipper::result_info clipstatus, int recursivedepth)
{
    // if the node is totally enclosed, trivially accept all the children nodes.  otherwise, do a frustum clip test
    if (clipstatus.active_flag != 0)
    {
        bbox3 nodebbox ( (testnode->minCorner + testnode->maxCorner)*0.5, (testnode->maxCorner - testnode->minCorner)*0.5);
        clipstatus = clipper.TestBBox(nodebbox, clipstatus);
    }

    // if the node is culled, then ignore this node and all nodes below it
    if (clipstatus.culled)
        return;

    // this node is not culled.  Test all the elements in this node, and then recurse to child nodes.
    nOctElement *oe;
    for (oe = (nOctElement *) testnode->elm_list.GetHead();
         oe;
         oe = (nOctElement *) oe->GetSucc())
    {
        bbox3 thisbbox( (oe->minCorner + oe->maxCorner)*0.5, (oe->maxCorner - oe->minCorner)*0.5);
        nSphereClipper::result_info ri(clipper.TestBBox(thisbbox, clipstatus));
        if (!ri.culled)
        {
            nSpatialElement *se = (nSpatialElement *)oe;
            se->Accept(*this, recursivedepth);
        }
    }

    // now check the children of this node
    if (testnode->c[0])
    for (int childix=0; childix < 8; childix++)
    {
        this->CheckOctNode(testnode->c[childix], clipper, clipstatus, recursivedepth);
    }
}

bool nVisibleSphereVisitor::VisibilityTest(nSpatialElement *visitee)
{
    // test against the current sphere clipper
    nSphereClipper &clipper = GetSphereClipper();

    nSphereClipper::result_info in, out;
    bbox3 totest( (visitee->minCorner + visitee->maxCorner)*0.5, (visitee->maxCorner - visitee->minCorner)*0.5);
    out = clipper.TestBBox( totest, in);
    
    return !out.culled;
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

