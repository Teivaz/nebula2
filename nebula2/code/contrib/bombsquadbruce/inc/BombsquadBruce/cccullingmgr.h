#ifndef N_CCULLINGMGR_H
#define N_CCULLINGMGR_H
//------------------------------------------------------------------------------
/**
    @class CCCullingMgr
    @ingroup BombsquadBruceContribModule
    @brief Determines which game objects are visible from a given perspective.

    (C)	2004 Rafael Van Daele-Hunt
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "spatialdb/nvisitorbase.h"
#include "spatialdb/nspatialsector.h"
class CCCamera;
class CCRoot;

class CCCullingMgr : public nRoot
{
public:
    CCCullingMgr();
    ~CCCullingMgr();

    // nonscript commands
    void StartElements();
    bool AddElement(CCRoot&);
    void EndElements();
    const nVisibilityVisitor::VisibleElements& GetVisibleElements(const CCCamera&);
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
