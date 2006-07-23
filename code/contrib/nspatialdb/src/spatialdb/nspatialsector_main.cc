//--------------------------------------------------
// nspatialsector.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------
#include "kernel/nkernelserver.h"
#include "spatialdb/nspatialsector.h"

nNebulaScriptClass(nSpatialSector, "kernel::nroot");

nSpatialSector::nSpatialSector()
{
}

nSpatialSector::~nSpatialSector()
{
}

void nSpatialSector::Configure(const matrix44 &transformtoglobalspace)
{
}

nSpatialSector::ElementHandle nSpatialSector::AddElement(nSpatialElement *addme)
{
    m_Nodeset.AddElement(addme);
    return addme;
}

void nSpatialSector::UpdateElement(nSpatialSector::ElementHandle elementid,
                                const bbox3 &newbbox)
{
    // just update the element.  We don't need to do anything else
		//elementid->SetAABB(newbbox);
	m_Nodeset.MoveElement(elementid, newbbox);
}

void nSpatialSector::RemoveElement(nSpatialSector::ElementHandle elementid)
{
    m_Nodeset.RemoveElement(elementid);
}

void nSpatialSector::Accept(nVisibilityVisitor &v, int recursiondepth, VisitorFlags flags)
{
    // just pass it onto the node set
    m_Nodeset.Accept(v, recursiondepth, flags);
}

void nSpatialSector::Accept(nSpatialVisitor &v, int recursiondepth, VisitorFlags flags)
{
    // just pass it onto the node set
    m_Nodeset.Accept(v, recursiondepth, flags);
}

void nSpatialSector::Accept(nOcclusionVisitor &v, int recursiondepth, VisitorFlags flags)
{
    // just pass it onto the node set
    m_Nodeset.Accept(v, recursiondepth, flags);
}

