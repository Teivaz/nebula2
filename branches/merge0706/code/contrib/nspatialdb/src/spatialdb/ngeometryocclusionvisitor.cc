//-----------------------------------------------
// ngeometryocclusionvisitor.cc
// (C) 2004 Gary Haussmann
//
// This code is licensed under the Nebula License
//-----------------------------------------------

#include "spatialdb/ngeometryocclusionvisitor.h"
#include "gfx2/ngfxserver2.h"

nGeometryOcclusionVisitor::nGeometryOcclusionVisitor(const vector3 &vp, nVisibilityVisitor *v)
: nOcclusionVisitor(vp)
{
}

nGeometryOcclusionVisitor::~nGeometryOcclusionVisitor()
{
}

void nGeometryOcclusionVisitor::Reset()
{
    nVisibilityVisitor::Reset();

    // wipe out all occluders
    m_occluderset.Clear();
}

void nGeometryOcclusionVisitor::Reset(const vector3 &vp)
{
    nVisibilityVisitor::Reset(vp);

    // wipe out all occluders
    m_occluderset.Clear();
}

void nGeometryOcclusionVisitor::StartVisualizeDebug(nGfxServer2 *gfx2)
{
    nVisibilityVisitor::StartVisualizeDebug(gfx2);
    m_debuglines.Clear();
}

void nGeometryOcclusionVisitor::EndVisualizeDebug()
{
    nVisibilityVisitor::EndVisualizeDebug();

    if (m_gfxdebug == NULL)
        return;

    // draw all of the occluder lines we have accumulated
    vector4 color(1,0,0,1);

    if (m_debuglines.Size() > 0)
    {
        m_gfxdebug->BeginLines();
        for (int vix=0; vix < m_debuglines.Size(); vix += 2)
            m_gfxdebug->DrawLines3d(m_debuglines.Begin() + vix, 2, color);
        m_gfxdebug->EndLines();
    }
}

void nGeometryOcclusionVisitor::Visit(nSpatialElement *visitee)
{
}


void nGeometryOcclusionVisitor::AddBBoxOccluder(const bbox3 &occludingbox)
{
    // here we construct a plane clipper that enclosed the shadowed area behind the bbox;
    // this involves finding the bbox silhouette from the current viewpoint

    vector3 viewpoint(GetViewPoint());
    vector3 occludervector(occludingbox.center() - viewpoint);
    vector3 occextents(occludingbox.extents());

    // degenerate case when the viewpoint is inside the occluder, maybe
    // we'll handle this smartly someday
    if (occludingbox.contains(viewpoint))
        return;

    // we'll have to build the clipping planes manually be examing the
    // bounding box silhouette as seen from the viewpoint
    // here's some data arrays describing the cube faces and edges
    // array of normals for the cube faces
    vector3 facenormals[6] = { vector3(0,0,-1), vector3(1,0,0), vector3(0,1,0), vector3(0,0,1), vector3(-1,0,0), vector3(0,-1,0) };

    // array telling which vertices are contained for each face, in ccw order.
    // used for generating face clipping planes
    int facevertexmap[6][4] = { {0,1,5,4}, {1,3,7,5}, {4,5,7,6}, {3,2,6,7}, {4,6,2,0}, {1,0,2,3} };

    // array telling what two faces each edge is part of--edges are
    // considered directional vectors from one vertex to another, and
    // the faces are specified with the left side face first.  In this
    // way we can determine which orientation of the edge to use
    // for the silhouette
    int edgefacemap[12][2] = { {0,5}, {0,1}, {0,2}, {0,4}, {1,5}, {2,1}, {4,2}, {5,4}, {3,5}, {3,1}, {3,2}, {3,4} };

    // array telling which vertices a given edge is connecting
    int edgevertexmap[12][2] = { {0,1}, {1,5}, {5,4}, {4,0}, {1,3}, {5,7}, {4,6}, {0,2}, {3,2}, {7,3}, {6,7}, {2,6} };


    // array of vertex locations, specifying the relative point locations
    // from the bbox center
    vector3 vertexvp[8] = { 
        vector3(-1,-1,-1), vector3(1,-1,-1), vector3(-1,-1,1), vector3(1,-1,1),
        vector3(-1, 1,-1), vector3(1, 1,-1), vector3(-1, 1,1), vector3(1, 1,1)
    };

    // compute actual locations of the bounding box 
    vector3 vertexv[8];
    for (int vix=0; vix<8; vix++)
    {
        vector3 vp(vertexvp[vix]);
        vertexv[vix] = occludingbox.center() + vector3(occextents.x * vp.x, occextents.y * vp.y, occextents.z * vp.z);
    }

    // there can only be 9 clipplanes max; three for the planes made from the front faces
    // and six planes outlining the silhouette
    plane clipplanes[9];
    int clipplaneindex=0, silhouettevertexindex=0;

    bool backfaces[6];

    // find back faces.  We compare the face normal with a vector from
    // the viewpoint to the face
    for (int faceix=0; faceix < 6; faceix++)
    {
        // project out from the boxx center to the center of the face
        vector3 n1(facenormals[faceix]);
        vector3 n2(occextents.x * n1.x, occextents.y * n1.y, occextents.z * n1.z);
        vector3 facepos(occludingbox.center() + n2);
        vector3 vp2face(facepos - viewpoint);

        // the dotproduct of the face normal with  the (viewpoint -> face) vector tells us if the face is front or back facing
        backfaces[faceix] = ((vp2face % n2) > 0.0);

        // add back faces to the clipplanes, such that objects in front of the box are
        // not occluded.  Push the face back a little so the object doesn't occlude itself...
        if (!backfaces[faceix])
        {
            int i1=facevertexmap[faceix][0], i2 = facevertexmap[faceix][1], i3=facevertexmap[faceix][2], i4=facevertexmap[faceix][3];
            vector3 n3(n2*0.1f);
            clipplanes[clipplaneindex++] = plane( vertexv[i1] - n3, vertexv[i3] - n3, vertexv[i2] - n3 );
            /*m_debuglines.PushBack(vertexv[i1]);
            m_debuglines.PushBack(vertexv[i2]);
            m_debuglines.PushBack(vertexv[i2]);
            m_debuglines.PushBack(vertexv[i3]);
            m_debuglines.PushBack(vertexv[i3]);
            m_debuglines.PushBack(vertexv[i4]);
            m_debuglines.PushBack(vertexv[i4]);
            m_debuglines.PushBack(vertexv[i1]);*/
        }
    }

    // back faces are marked; find the silhouette edges.  These are edges
    // bordering one front and one back face when we find and edge, build
    // a clip plane for it
    for (int edgeix=0; edgeix < 12; edgeix++)
    {
        // this is a silhouette edge if one face is a front face and one
    	// face is a back face. it is important to know which face is the
    	// back face, so that we orient our edge (and thus the clipping plane)
        // correctly
        bool b1 = backfaces[edgefacemap[edgeix][0]], b2 = backfaces[edgefacemap[edgeix][1]];
        if ( (b1 & !b2) || (!b1& b2) ) // try replacing w/ xor... GJH
        {
            int i1=edgevertexmap[edgeix][0], i2 = edgevertexmap[edgeix][1];
            // swap the vertices if the left face is the back face
            if (b2)
            {
                int ix=i1; i1=i2; i2=ix;
            }
            // find the two vertex coordinates for this edge
            vector3 v1(vertexv[i1]), v2(vertexv[i2]);

            // we can construct the clipping plane using the viewpoint and the two vertices on the silhouette edge.
            clipplanes[clipplaneindex++] = plane(viewpoint, v1, v2);

            // add the vertices to the silhouette points if they're not already in there
            m_debuglines.PushBack(v1);
            m_debuglines.PushBack(v2);
            // add "streaks" away from the silhouette edge
            m_debuglines.PushBack(v1);
            m_debuglines.PushBack( v1 * 2 - viewpoint );
            m_debuglines.PushBack(v2);
            m_debuglines.PushBack( v2 * 2 - viewpoint );
            //m_debugverts[silhouettevertexindex++] = v1;
            //m_debugverts[silhouettevertexindex++] = v2;
        }
    }

    // there are only so many silhouette types you can make from a cube
    n_assert( (clipplaneindex > 4) && (clipplaneindex < 10) );
    // there should the same # of silhouette vertices  as the number of silhouette edges
    //n_assert( silhouettevertexindex == (2 * m_numplanes) );

    nPlaneClipper newoccluder(clipplanes, clipplaneindex);

    m_occluderset.PushBack(newoccluder);
}

void nGeometryOcclusionVisitor::AddSphereOccluder(const sphere &sphereoccluder)
{
}

void nGeometryOcclusionVisitor::AddHullOccluder(const nPlaneClipper &hulloccluder)
{
}

VisitorFlags nGeometryOcclusionVisitor::VisibilityTest(const bbox3 &testbox, VisitorFlags flags)
{
    // note that the occlusionvisitor uses all the bitflags in the VisitorFlags structure to
    // determine which of its occluders can be safely ignored.  This means the restricting 
    // visibilityvisitor gets a default set of bitflags and always has to do all its tests!
    // Perhaps someday we'll 'reallocate' the first 6 bitflags or so for the restricting
    // visibilityvisitor...

    // test this bbox against the provided visibility visitor, if any
    if (m_restrictingvisitor)
    {
        VisitorFlags ff=m_restrictingvisitor->VisibilityTest(testbox, VisitorFlags());
        if (ff.AntiTestResult())
            return ff;
    }

    // test this bbox against all our planeclipper occluders
    for (int occix=0; occix < m_occluderset.Size(); occix++)
    {
        int occluderbit = (1<<occix);

        // if this occluder is turned off, skip it
//        if ( (occix < VisitorFlags::MAXVISITORFLAGS) && ((occluderbit & flags.m_activeflags) == 0) )
//            continue;

        nPlaneClipper &thisoccluder= m_occluderset[occix];

        // by passing in a 'false' initial flag, we'll only get a true back if the object
        // is entirely inside the occluder shadow
        VisitorFlags occludetest(thisoccluder.TestBBox(testbox, VisitorFlags(false,false)));

        // if the test result is true, this bbox is completely behind the object and so is not visible
        if (occludetest.TestResult())
            return VisitorFlags(false);

/*        // if the antitest results is true, this bbox is completely not behind the object and so we
        // can ignore this occluder for enclosed elements
        if (occludetest.AntiTestResult())
            flags.m_activeflags &= ~occluderbit;*/
    }

    // it's not totally occluded according to the occluders, although it may still be occluding something
    return flags;
}

VisitorFlags nGeometryOcclusionVisitor::VisibilityTest(const sphere &testsphere, VisitorFlags flags)
{
    // build a bounding box inside the sphere, and test that
    vector3 boxcenter(testsphere.p);
    vector3 boxextents(testsphere.r, testsphere.r, testsphere.r);

    // the bounding box must be scaled so it fits within the sphere
    bbox3 testbox(boxcenter, boxextents * 0.7071f);
    return VisibilityTest(testbox, flags);
}

void nGeometryOcclusionVisitor::EnterLocalSpace(matrix44 &warp)
{
    // we should really transform all the occluders into the new space, or at
    // least ditch them while in the new space
    
    // transform the restricting visitor at least!
    if (m_restrictingvisitor)
        m_restrictingvisitor->EnterLocalSpace(warp);
}

void nGeometryOcclusionVisitor::LeaveLocalSpace()
{
    // we should really transform all the occluders into the old space, or at
    // least get back the ones in the old space and ditch the new ones
    if (m_restrictingvisitor)
        m_restrictingvisitor->LeaveLocalSpace();
}


