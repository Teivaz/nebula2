//--------------------------------------------------
// nvisiblespheregenarray.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nvisiblespheregenarray.h"

nVisibleSphereGenArray::nVisibleSphereGenArray(const sphere &viewsphere, nVisibleSphereGenArray::VisibleElements &foundarray)
: nVisibleSphereVisitor(viewsphere), m_visarray(foundarray)
{
}

nVisibleSphereGenArray::~nVisibleSphereGenArray()
{
    ClearArray();
}

void nVisibleSphereGenArray::Reset()
{
    ClearArray();
    nVisibleSphereVisitor::Reset();
}

void nVisibleSphereGenArray::ClearArray()
{
    // clear out the list of stuff
    m_visarray.Clear();
}

void nVisibleSphereGenArray::Visit(nSpatialElement *visitee, int recursedepth)
{
    // tack this element onto the list!
    m_visarray.Append(visitee);
}

