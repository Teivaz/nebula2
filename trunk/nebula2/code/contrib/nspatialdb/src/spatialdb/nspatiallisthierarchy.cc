//--------------------------------------------------------
// nspatiallisthierarchy.cc
// (C) 2004 Gary Haussmann
//
// implementation for spatial list element
//--------------------------------------------------------

#include "spatialdb/nspatiallisthierarchy.h"

nSpatialListHierarchy::nSpatialListHierarchy()
: nSpatialHierarchyElement()
{
}

nSpatialListHierarchy::~nSpatialListHierarchy()
{
    ClearList();
}

void nSpatialListHierarchy::ClearList()
{
    while (!m_ElementList.IsEmpty())
        m_ElementList.RemHead();
}

void nSpatialListHierarchy::AppendElement(nSpatialElement *appendme)
{
    nNode *newnode = new nNode();
    newnode->SetPtr(appendme);
    m_ElementList.AddTail(newnode);
}

void nSpatialListHierarchy::RemoveElement(nSpatialElement *removeme)
{
    // find this node in the list
    nNode *walknodes = m_ElementList.GetHead();
    while (walknodes)
    {
        nSpatialElement *x = (nSpatialElement *)walknodes->GetPtr();
        if (x == removeme)
        {
            walknodes->Remove();
            return;
        }
    }

    // no match! give up
    return;
}

void nSpatialListHierarchy::Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    // for an accept, we just pass on the accept() to each element in our list
    if (m_ElementList.IsEmpty()) return;

    nNode *walknodes = m_ElementList.GetHead();
    while (walknodes)
    {
        nSpatialElement *thiselement = (nSpatialElement *)walknodes->GetPtr();
        thiselement->Accept(visitor, recursiondepth, flags);
        walknodes = walknodes->GetSucc();
    }
}

void nSpatialListHierarchy::Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    // for an accept, we just pass on the accept() to each element in our list
    if (m_ElementList.IsEmpty()) return;

    nNode *walknodes = m_ElementList.GetHead();
    while (walknodes !=NULL)
    {
        nSpatialElement *thiselement = (nSpatialElement *)walknodes->GetPtr();
        thiselement->Accept(visitor, recursiondepth, flags);
        walknodes = walknodes->GetSucc();
    }
}

void nSpatialListHierarchy::Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    // for an accept, we just pass on the accept() to each element in our list
    if (m_ElementList.IsEmpty()) return;

    nNode *walknodes = m_ElementList.GetHead();
    while (walknodes !=NULL)
    {
        nSpatialElement *thiselement = (nSpatialElement *)walknodes->GetPtr();
        thiselement->Accept(visitor, recursiondepth, flags);
        walknodes = walknodes->GetSucc();
    }
}

