#ifndef N_CCULLINGMGR_H
#define N_CCULLINGMGR_H
//------------------------------------------------------------------------------
/**
    @class CCCullingMgr

    @brief Determines which game objects are visible from a given perspective.

    (C)	2004 Rafael Van Daele-Hunt
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "spatialdb/nvisibilityvisitor.h"
class nSpatialSector;
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
    nVisibilityVisitor::VisibleElements m_ObjectsToRender;
    nVisibilityVisitor* m_pCuller;

    nRef<nSpatialSector> m_rRootSector;    
};

#endif N_CCULLINGMGR_H