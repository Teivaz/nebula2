//------------------------------------------------------------
// nspatialelements.cc
// (C) 2004 Gary Haussmann
//------------------------------------------------------------

#include "spatialdb/nspatialelements.h"
#include "spatialdb/nvisibilityvisitor.h"
#include "spatialdb/nspatialvisitor.h"


// visitor processing for a basic spatial element.  We just call accept()...
void nSpatialElement::Accept(nVisibilityVisitor &visitor, int recursiondepth)
{
    visitor.Visit(this, recursiondepth);
}

void nSpatialElement::Accept(nSpatialVisitor &visitor, int recursiondepth)
{
    visitor.Visit(this, recursiondepth);
}


// visitor processing for a basic portal.  We call accept on the underlying sector,
// unless we've hit the bottom of the allow recursion, in which case the portal is accepted instead
void nSpatialPortal::Accept(nVisibilityVisitor &visitor, int recursiondepth)
{
    if ( (recursiondepth > 0) && (m_otherside.isvalid()))
    {
        visitor.Visit(m_otherside.get(), recursiondepth-1);
    }
    else
    {
        visitor.Visit(this, recursiondepth);
    }
}

// actually, portals, are ignored for spatial queries
void nSpatialPortal::Accept(nSpatialVisitor &visitor, int recursiondepth)
{
}
