//------------------------------------------------------------
// nspatialelements.cc
// (C) 2004 Gary Haussmann
//------------------------------------------------------------

#include "spatialdb/nspatialelements.h"
#include "spatialdb/nvisitorbase.h"

// visitor processing for a basic spatial element.  We just call accept()...
void nSpatialElement::Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    // the default behavior is to always have the visitor process this node
    visitor.Visit(this);
}

void nSpatialElement::Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    // the default behavior is to always have the visitor process this node
    visitor.Visit(this);
}

void nSpatialElement::Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    // default is to do nothing for an occlusion visitor
}

nBBoxSpatialElement::~nBBoxSpatialElement()
{
}

void nBBoxSpatialElement::Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    if (visitor.VisibilityTest(GetAABB(), flags).TestResult())
        visitor.Visit(this);
}

void nBBoxSpatialElement::Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    if (visitor.ContainmentTest(GetAABB(), flags).TestResult())
        visitor.Visit(this);
}

void nBBoxSpatialElement::Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    // default behavior is to not occlude stuff, since we're not an occluder
}

// visitor processing for a basic portal.  We call accept on the underlying sector,
// unless we've hit the bottom of the allow recursion, in which case the portal is accepted instead
void nSpatialPortalElement::Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    if (
        (visitor.VisibilityTest(GetAABB(), flags).TestResult()) &&
        (recursiondepth > 0) &&
        (m_otherside.isvalid())
        )
    {
        // add the sector on the other side, not this portal
        //visitor.Visit(m_otherside.get());
    }
}

// actually, portals, are ignored for spatial queries
void nSpatialPortalElement::Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
}

void nSpatialOccluderElement::Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    // default behavior for an occluder is to use our bounding box as an occluder
    if (visitor.VisibilityTest(GetAABB(), flags).TestResult())
        visitor.AddBBoxOccluder(GetAABB());
}

