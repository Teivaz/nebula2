//--------------------------------------------------
// noccludingspherevisitor.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/noccludingspherevisitor.h"

nOccludingSphereVisitor::nOccludingSphereVisitor(const sphere &viewsphere)
: nVisibleSphereVisitor(viewsphere)
{
}

nOccludingSphereVisitor::~nOccludingSphereVisitor()
{
}

void nOccludingSphereVisitor::Reset()
{
  // clear out the occluders
  m_occluders.Clear();

  nVisibleSphereVisitor::Reset();
}

void nOccludingSphereVisitor::Visit(nSpatialSector *visitee, int recursedepth)
{
    // bug out if we're hit the bottom of our allowed recursion
    if (recursedepth < 1)
        return;

    nOctNode *rootnode = visitee->GetRoot();
    nSphereClipper::result_info clipinfo;
    nSphereClipper sphereclip = m_sphereclipperstack.Back();
    // record the size of the occluder array before doing this sector
    int previousoccludersize = m_occluders.Size();

    // get up to 3 occluders for this sector
    CollectOccluders(rootnode, sphereclip, clipinfo, 3);

    // if in debug mode, render the view frustum and occluders
    if (m_gfxdebug)
    {
        sphereclip.VisualizeSphere(m_gfxdebug, vector4(1.0f,1.0f,1.0f,0.5f));
        // draw the occluders
        for (nArray<nBBoxOccluder>::iterator bboxi = m_occluders.Begin(); bboxi != m_occluders.End(); bboxi++)
        {
            bboxi->VisualizeBBox(m_gfxdebug, vector4(0.0f,1.0f,1.0f,1.0f));
        }
    }


    // recursively descend the octree checking each node for clipping
    CheckOctNode(rootnode, sphereclip, clipinfo, recursedepth);

    // wipe out any occluders added to the stack upon entering this sector
    while (m_occluders.Size() > previousoccludersize)
    {
        m_occluders.Erase(m_occluders.End()-1);
    }
}

void nOccludingSphereVisitor::CheckOctNode(nOctNode *testnode, nSphereClipper &clipper, nSphereClipper::result_info clipstatus, int recursivedepth)
{
    // if the node is totally enclosed, trivially accept all the children nodes.  otherwise, do a frustum clip test
    bbox3 nodebbox ( (testnode->minCorner + testnode->maxCorner)*0.5, (testnode->maxCorner - testnode->minCorner)*0.5);
    if (clipstatus.active_flag != 0)
    {
        clipstatus = clipper.TestBBox(nodebbox, clipstatus);
    }

    // if the node is culled, then ignore this node and all nodes below it
    if (clipstatus.culled)
        return;

    // if the node is occluded, ignore this node and all nodes below it
    if (IsOccluded(sphere(nodebbox.center(), nodebbox.extents().len())))
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
            // is this element occluded? skip if it is
            if (!IsOccluded(sphere(se->pos, se->radius)))
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

// recursive collection of occluders in this octree
int nOccludingSphereVisitor::CollectOccluders(nOctNode *collectnode, nSphereClipper &clipper, nSphereClipper::result_info clipstatus, int maxoccluders)
{
    // if the node is totally enclosed, trivially accept all the children nodes.  otherwise, do a frustum clip test
    bbox3 nodebbox ( (collectnode->minCorner + collectnode->maxCorner)*0.5, (collectnode->maxCorner - collectnode->minCorner)*0.5);
    if (clipstatus.active_flag != 0)
    {
        clipstatus = clipper.TestBBox(nodebbox, clipstatus);
    }

    // if the node is culled, then ignore this node and all nodes below it
    if (clipstatus.culled)
        return maxoccluders;

    // if the node is occluded, ignore this node and all nodes below it
    if (IsOccluded(sphere(nodebbox.center(), nodebbox.extents().len())))
        return maxoccluders;

    // this node is not culled.  Test all the elements in this node, and then recurse to child nodes.
    nOctElement *oe;
    for (oe = (nOctElement *) collectnode->elm_list.GetHead();
         oe;
         oe = (nOctElement *) oe->GetSucc())
    {
        bbox3 thisbbox( (oe->minCorner + oe->maxCorner)*0.5, (oe->maxCorner - oe->minCorner)*0.5);
        nSphereClipper::result_info ri(clipper.TestBBox(thisbbox, clipstatus));
        if (!ri.culled)
        {
            nSpatialElement *se = (nSpatialElement *)oe;
            // if it's an occluder then add to the occluder set
            if (se->GetSpatialType() & nSpatialElement::N_SPATIAL_OCCLUDER)
            {
                // stuff a new sphereoccluder on the array
                m_occluders.PushBack(nBBoxOccluder(GetViewPoint(), thisbbox));
                maxoccluders -= 1;
            }
        }
    }

    // now check the children of this node
    if (collectnode->c[0])
    for (int childix=0; childix < 8; childix++)
    {
        maxoccluders = this->CollectOccluders(collectnode->c[childix], clipper, clipstatus, maxoccluders);
    }

    return maxoccluders;
}

bool nOccludingSphereVisitor::VisibilityTest(nSpatialElement *visitee)
{
    // test against the current sphere clipper
    nSphereClipper &clipper = GetSphereClipper();

    nSphereClipper::result_info in, out;
    bbox3 totest( (visitee->minCorner + visitee->maxCorner)*0.5, (visitee->maxCorner - visitee->minCorner)*0.5);
    out = clipper.TestBBox( totest, in);
    
    return !out.culled;
}

// checks that a given sphere is not blocked by any occluders
bool nOccludingSphereVisitor::IsOccluded(const sphere &testsphere)
{
    // now check against all occluders as well
    for (nArray<nBBoxOccluder>::iterator occluderiter = m_occluders.Begin();
                                            occluderiter != m_occluders.End();
                                            occluderiter++)
    {
        // does this one occlude it?
        nBBoxOccluder::result_info occludetest;
        occludetest = occluderiter->TestSphere(testsphere, occludetest);

        if (occludetest.culled)
            return true;
    }

    // not occluded
    return false;

}

void nOccludingSphereVisitor::EnterLocalSpace(matrix44 &warp)
{
    nVisibilityVisitor::EnterLocalSpace(warp);
}

void nOccludingSphereVisitor::LeaveLocalSpace()
{
    nVisibilityVisitor::LeaveLocalSpace();
}

