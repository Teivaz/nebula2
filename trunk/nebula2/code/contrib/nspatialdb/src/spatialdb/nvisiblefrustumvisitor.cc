//--------------------------------------------------
// nvisiblefrustumvisitor.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nvisiblefrustumvisitor.h"
#include "gfx2/ngfxserver2.h"

nVisibleFrustumVisitor::nVisibleFrustumVisitor(nCamera2 &cameraprojection, matrix44 &cameratransform)
: nVisibilityVisitor(cameratransform.pos_component()), m_cameraprojection(cameraprojection)
{
   // initialize the view frustum, matrix, etc.

   m_viewertransformstack.PushBack(cameratransform);

   nFrustumClipper clipper(cameraprojection, cameratransform);
   m_viewfrustumstack.PushBack(clipper);
}

nVisibleFrustumVisitor::~nVisibleFrustumVisitor()
{
}

void nVisibleFrustumVisitor::VisualizeDebug(nGfxServer2 *gfx2)
{
    // draw the frustum clipper
    nFrustumClipper &clipper(m_viewfrustumstack.Front());
    clipper.VisualizeFrustum(gfx2, vector4(1.0f,1.0f,0.0f,1.0f));

}


void nVisibleFrustumVisitor::Reset()
{
  // the stacks should be size 1 at this point
  n_assert(m_viewertransformstack.Size() == 1);
  n_assert(m_viewfrustumstack.Size() == 1);

  nVisibilityVisitor::Reset();
}

void nVisibleFrustumVisitor::Visit(nSpatialSector *visitee, int recursedepth)
{
    // bug out if we're hit the bottom of our allowed recursion
    if (recursedepth < 1)
        return;

    n_assert(visitee->GetOctree() != NULL);
    nOctNode *rootnode = visitee->GetOctree()->GetRoot();
    nFrustumClipper::result_info clipinfo;
    nFrustumClipper frustum = m_viewfrustumstack.Back();

    // recursively descend the octree checking each node for clipping
    CheckOctNode(rootnode, frustum, clipinfo, recursedepth);
}

void nVisibleFrustumVisitor::CheckOctNode(nOctNode *testnode, nFrustumClipper &clipper, nFrustumClipper::result_info clipstatus, int recursivedepth)
{
    // if the node is totally enclosed, trivially accept all the children nodes.  otherwise, do a frustum clip test
    if (clipstatus.active_planes != 0)
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
        nFrustumClipper::result_info ri(clipper.TestBBox(thisbbox, clipstatus));
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

bool nVisibleFrustumVisitor::VisibilityTest(nSpatialElement *visitee)
{
    // test against the current frustum clipper
    nFrustumClipper &clipper = GetFrustumClipper();

    nFrustumClipper::result_info in, out;
    bbox3 totest( (visitee->minCorner + visitee->maxCorner)*0.5, (visitee->maxCorner - visitee->minCorner)*0.5);
    out = clipper.TestBBox(totest, in);
    
    return !out.culled;
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

