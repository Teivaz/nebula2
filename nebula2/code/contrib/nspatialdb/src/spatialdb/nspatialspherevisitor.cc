//--------------------------------------------------
// nspatialspherevisitor.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nspatialspherevisitor.h"

nSpatialSphereVisitor::nSpatialSphereVisitor(const sphere &viewsphere)
: nSpatialVisitor()
{
   // initialize the view sphere
   m_sphereregionstack.PushBack(viewsphere);
   m_sphereclipperstack.PushBack(nSphereClipper(viewsphere));
}

nSpatialSphereVisitor::~nSpatialSphereVisitor()
{
}

void nSpatialSphereVisitor::Reset()
{
  // the stacks should be size 1 at this point
  n_assert(m_sphereregionstack.Size() == 1);
  n_assert(m_sphereclipperstack.Size() == 1);

  nSpatialVisitor::Reset();
}

void nSpatialSphereVisitor::Visit(nSpatialSector *visitee, int recursedepth)
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
        sphereclip.VisualizeSphere(m_gfxdebug, vector4(1.0f,0.0f,1.0f,0.5f));
    }

    // recursively descend the octree checking each node for clipping
    CheckOctNode(rootnode, sphereclip, clipinfo, recursedepth);

    // find any child sectors and descend to them as well
    nRoot *childnode = visitee->GetHead();
    nClass *sectorclass = visitee->GetClass();
    while (childnode != NULL)
    {
	    // if it's a sector, examine it
	    if (childnode->IsA(sectorclass))
	    {
	        nSpatialSector *childsector = (nSpatialSector *)childnode;
	        // currently the child sectors have the same coordinate system as their parents, so this
	        // enter/leave local space doesn't do anything
	        matrix44 idmatrix;
	        idmatrix.ident();
	        this->EnterLocalSpace(idmatrix);
	        this->Visit(childsector, recursedepth-1);
	        this->LeaveLocalSpace();
    	}
	
        childnode = childnode->GetSucc();
    }
}

void nSpatialSphereVisitor::CheckOctNode(nOctNode *testnode, nSphereClipper &clipper, nSphereClipper::result_info clipstatus, int recursivedepth)
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

bool nSpatialSphereVisitor::ContainmentTest(nSpatialElement *visitee)
{
    // test against the current sphere clipper
    nSphereClipper &clipper = GetSphereClipper();

    nSphereClipper::result_info in, out;
    bbox3 totest( (visitee->minCorner + visitee->maxCorner)*0.5, (visitee->maxCorner - visitee->minCorner)*0.5);
    out = clipper.TestBBox( totest, in);
    
    return !out.culled;
}

void nSpatialSphereVisitor::EnterLocalSpace(matrix44 &warp)
{
    // transform the sphere and generate a new clipper for the local space
    sphere oldsphere(m_sphereregionstack.Back());
    // argh, must assume all scales are the same!
    float newradius( (warp*vector3(0,0,oldsphere.r)).len() );
    sphere newsphere(warp * oldsphere.p, newradius);

    nSphereClipper newclipper(newsphere);

    m_sphereregionstack.PushBack(newsphere);
    m_sphereclipperstack.PushBack(newclipper);

    nSpatialVisitor::EnterLocalSpace(warp);
}

void nSpatialSphereVisitor::LeaveLocalSpace()
{
    m_sphereregionstack.Erase(m_sphereregionstack.End()-1);
    m_sphereclipperstack.Erase(m_sphereclipperstack.End()-1);

    nSpatialVisitor::LeaveLocalSpace();
}
