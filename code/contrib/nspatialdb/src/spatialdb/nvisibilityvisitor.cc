//--------------------------------------------------
// nvisibilityvisitor.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nvisibilityvisitor.h"
#include "gfx2/ngfxserver2.h"

nVisibilityVisitor::nVisibilityVisitor(const vector3 &viewpoint)
: m_gfxdebug(NULL)
{
    m_viewpointstack.PushBack(viewpoint);
}

nVisibilityVisitor::~nVisibilityVisitor()
{
}

void nVisibilityVisitor::Reset()
{
}

void nVisibilityVisitor::Reset(const vector3 &newviewpoint)
{
    m_viewpointstack.clear();
    m_viewpointstack.PushBack(newviewpoint);
}


void nVisibilityVisitor::Visit(nSpatialElement *visitee, int recursedepth)
{
    // do nothing.  Typically a subclass will override this to provide processing of some sort, like
    // adding the element to a list or rendering it
}

vector3 &nVisibilityVisitor::GetViewPoint() const
{
    // there must be at least one viewpoint on the stack!
    n_assert(m_viewpointstack.Size() > 0);

    return m_viewpointstack.Back();
}

void nVisibilityVisitor::EnterLocalSpace(matrix44 &warp)
{
    // transform our viewpoint into the new local space and put it on the stack
    vector3 newviewpoint = warp * GetViewPoint();
    m_viewpointstack.PushBack(newviewpoint);
}

void nVisibilityVisitor::LeaveLocalSpace()
{
    // there always must be at least 1 viewpoint on the stack
    n_assert(m_viewpointstack.Size() > 0);

    // pop the most recent viewpoint we put on the stack
    m_viewpointstack.Erase(m_viewpointstack.End()-1);

    // there always must be at least 1 viewpoint on the stack
    n_assert(m_viewpointstack.Size() > 0);
}

