//--------------------------------------------------
// nspatialsector.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nspatialsector.h"
#include "octree/noctree.h"

nNebulaScriptClass(nSpatialSector, "nroot");

nSpatialSector::nSpatialSector()
{
}

nSpatialSector::~nSpatialSector()
{
    // the octree should be automaticall removed, being a child nRoot node
}

void nSpatialSector::Configure(const matrix44 &transformtoglobalspace)
{
    // auto-create a child octree
    nString octreepath = this->GetFullName() + "/octree";
    m_octree = (nOctree *)this->kernelServer->New("noctree", octreepath.Get());
}

void nSpatialSector::AddElement(nSpatialElement *addme)
{
    m_octree->AddElement(addme);
}

void nSpatialSector::RemoveElement(nSpatialElement *removeme)
{
    m_octree->RemoveElement(removeme);
}

void nSpatialSector::UpdateElement(nSpatialElement *updateme, const vector3 &pos, float radius)
{
    m_octree->UpdateElement(updateme, pos, radius);
}

void nSpatialSector::UpdateElement(nSpatialElement *updateme, const vector3 &pos, const bbox3 &bbox)
{
    m_octree->UpdateElement(updateme, pos, bbox);
}


nOctree const *nSpatialSector::GetOctree() const
{
    if (m_octree.isvalid())
        return m_octree.get();
    else
        return NULL;
}


