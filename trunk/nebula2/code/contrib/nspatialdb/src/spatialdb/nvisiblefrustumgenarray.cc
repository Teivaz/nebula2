//--------------------------------------------------
// nvisiblefrustumgenarray.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nvisiblefrustumgenarray.h"

nVisibleFrustumGenArray::nVisibleFrustumGenArray(const nCamera2 &cam, const matrix44 &camxform, nVisibilityVisitor::VisibleElements &foundarray)
: nVisibleFrustumVisitor(cam, camxform), m_visarray(foundarray)
{
}

nVisibleFrustumGenArray::~nVisibleFrustumGenArray()
{
    ClearArray();
}

void nVisibleFrustumGenArray::Reset()
{
    ClearArray();
    nVisibleFrustumVisitor::Reset();
}

void nVisibleFrustumGenArray::ClearArray()
{
    // clear out the list of stuff
    m_visarray.Clear();
}

void nVisibleFrustumGenArray::Visit(nSpatialElement *visitee, int recursedepth)
{
    // tack this element onto the list
    m_visarray.Append(visitee);
}

