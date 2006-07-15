//--------------------------------------------------
// nvisitorbase.cc
//
// contains code for the two basic visitors for nspatialdb: visibilityvisitor and spatialvisitor
//
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nvisitorbase.h"
#include "gfx2/ngfxserver2.h"

//
// code for visibility visitor
//

nVisibilityVisitor::nVisibilityVisitor(const vector3 &viewpoint)
: m_gfxdebug(NULL)
{
    m_viewpointstack.PushBack(viewpoint);
    m_minimumdetailratiostack.PushBack(0.0f);
}

nVisibilityVisitor::~nVisibilityVisitor()
{
}

void nVisibilityVisitor::Reset()
{
}

void nVisibilityVisitor::Reset(const vector3 &newviewpoint)
{
    m_viewpointstack.Clear();
    m_viewpointstack.PushBack(newviewpoint);
}

void nVisibilityVisitor::StartVisualizeDebug(nGfxServer2 *gfx) 
{ 
    m_gfxdebug = gfx;
}

void nVisibilityVisitor::EndVisualizeDebug() 
{
}

vector3 &nVisibilityVisitor::GetViewPoint() const
{
    // there must be at least one viewpoint on the stack!
    n_assert(m_viewpointstack.Size() > 0);

    return m_viewpointstack.Back();
}

float nVisibilityVisitor::GetMinimumDetailRatio() const
{
    n_assert(m_minimumdetailratiostack.Size() > 0);

    return m_minimumdetailratiostack.Back();
}

void nVisibilityVisitor::EnterLocalSpace(matrix44 &warp)
{
    // transform our viewpoint into the new local space and put it on the stack
    vector3 newviewpoint = warp * GetViewPoint();
    m_viewpointstack.PushBack(newviewpoint);
    m_minimumdetailratiostack.PushBack(0.0f);
}

void nVisibilityVisitor::LeaveLocalSpace()
{
    // there always must be at least 1 viewpoint on the stack
    n_assert(m_viewpointstack.Size() > 0);

    // pop the most recent viewpoint we put on the stack
    m_viewpointstack.Erase(m_viewpointstack.End()-1);
    m_minimumdetailratiostack.Erase(m_minimumdetailratiostack.End()-1);

    // there always must be at least 1 viewpoint on the stack
    n_assert(m_viewpointstack.Size() > 0);
}

//
// code for spatial visitor
//

void nSpatialVisitor::Reset()
{
}

void nSpatialVisitor::StartVisualizeDebug(nGfxServer2 *gfx) 
{ 
    m_gfxdebug = gfx; 
}

void nSpatialVisitor::EndVisualizeDebug() 
{  
}

//
// code for occlusion visitor
//

nOcclusionVisitor::nOcclusionVisitor(const vector3 &vp)
: nVisibilityVisitor(vp)
{
}

nOcclusionVisitor::~nOcclusionVisitor()
{
}

void nOcclusionVisitor::StartVisualizeDebug(nGfxServer2 *gfx) 
{ 
    m_gfxdebug = gfx; 
}

void nOcclusionVisitor::EndVisualizeDebug() 
{ 

}
