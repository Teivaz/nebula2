#ifndef N_CCULLINGMGR_H
#define N_CCULLINGMGR_H
//------------------------------------------------------------------------------
/**
    @class BBCullingMgr
    @ingroup BombsquadBruceContribModule
    @brief Determines which game objects are visible from a given perspective.

    (C)	2004 Rafael Van Daele-Hunt
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "spatialdb/nvisitorbase.h"
#include "spatialdb/nspatialsector.h"
class BBCamera;
class BBRoot;

class BBCullingMgr : public nRoot
{
public:
    BBCullingMgr();
    ~BBCullingMgr();

    // nonscript commands
    void StartElements();
    bool AddElement(BBRoot&);
    void EndElements();
    const nVisibilityVisitor::VisibleElements& GetVisibleElements(const BBCamera&);
    void Visualize();

private:
    void DestroySpatialDB(nSpatialSector *cleanme);

    nVisibilityVisitor::VisibleElements m_ObjectsToRender;
    nVisibilityVisitor* m_pCuller;

    nArray<nSpatialElement *> m_currentelements;
    nArray<nSpatialSector::ElementHandle> m_currentelementhandles;
    nRef<nSpatialSector> m_rRootSector;    

};

#endif N_CCULLINGMGR_H
