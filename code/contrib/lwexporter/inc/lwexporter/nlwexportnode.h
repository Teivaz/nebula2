#ifndef N_LW_EXPORT_NODE_H
#define N_LW_EXPORT_NODE_H
//----------------------------------------------------------------------------
#include "lwwrapper/nlwglobals.h"
#include "tools/nmeshbuilder.h"

class nLWExportNodeType;
class nLWObjectExportSettings;
class nLWItemInfoGlobal;
class nLWSettingsRegistry;
class nSceneNode;
class nLWLayoutMonitor;
class nLWCmdExec;

//----------------------------------------------------------------------------
/**
    This will almost certainly be re-factored into various subclasses for
    mesh, skin, and what not.
*/
class nLWExportNode
{
public:
    nLWExportNode(nLWObjectExportSettings*, nLWCmdExec*);
    ~nLWExportNode();

    bool GetExportHierarchy();
    void CollectDescendants(const nLWGlobals::ItemInfo*, const nLWSettingsRegistry*);
    int GetNumChildren();
    int GetNumDescendants();
    bool IsAncestorOf(nLWExportNode*);
    void Export(nLWLayoutMonitor*);

private:
    void ClearChildren();
    nSceneNode* ExportShapeNode(const nString&, nLWLayoutMonitor*);
    nSceneNode* ExportSkinShapeNode(const nString&, nLWLayoutMonitor*);
    void ExportResourcesOnly(const nString&, nLWLayoutMonitor*);

    nLWExportNodeType* nodeType;
    nLWObjectExportSettings* settings;
    nArray<nLWExportNode*> children;
    nLWCmdExec* cmdExec;
};

//----------------------------------------------------------------------------
#endif // N_LW_EXPORT_NODE_H
