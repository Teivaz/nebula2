//--------------------------------------------------
// noccludingfrustumgenarray.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/noccludingfrustumgenarray.h"

nOccludingFrustumGenArray::nOccludingFrustumGenArray(nCamera2 &cam, matrix44 &camxform, nOccludingFrustumGenArray::VisibleElements &foundarray)
: nOccludingFrustumVisitor(cam, camxform), m_visarray(foundarray)
{
}

nOccludingFrustumGenArray::~nOccludingFrustumGenArray()
{
    ClearArray();
}

void nOccludingFrustumGenArray::Reset()
{
    ClearArray();
    nOccludingFrustumVisitor::Reset();
}

void nOccludingFrustumGenArray::ClearArray()
{
    // clear out the list of stuff
    m_visarray.Clear();
}

void nOccludingFrustumGenArray::Visit(nSpatialElement *visitee, int recursedepth)
{
    // tack this element onto the list
    m_visarray.Append(visitee);
}

