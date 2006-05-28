#ifndef N_LW_EXPORTER_H
#define N_LW_EXPORTER_H
//----------------------------------------------------------------------------
#include "lwwrapper/nlwglobals.h"
#include "util/nstring.h"
#include "util/narray.h"

class nLWObjectExportSettings;
class nTransformNode;
class nLWExportNode;
class nLWLayoutMonitor;
class nLWCmdExec;

//----------------------------------------------------------------------------
/**
*/
class nLWExporter
{
public:
    enum ExportMode
    {
        EXPORT_SELECTION = 0,
        EXPORT_SCENE     = 1,
    };

    nLWExporter(nLWCmdExec*);
    ~nLWExporter();

    void SetExportMode(ExportMode);
    bool ExecuteLWCmd(const nString& cmdStr);

    void Preview();
    void Export();

private:
    void CollectExportNodes();
    void ClearExportNodes();
    int GetTotalExportNodeCount();
    void DoExport();
    void ConstructN2FileName(nLWLayoutMonitor*);
    bool LaunchNebulaViewer(const nString& n2FileName);

    ExportMode exportMode;
    nArray<nLWExportNode*> topLevelExportNodes;
    nTransformNode* exportRoot;
    bool previewMode;
    nString n2FileName;
    nLWCmdExec* cmdExec;
};

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWExporter::SetExportMode(ExportMode mode)
{
    this->exportMode = mode;
}

//----------------------------------------------------------------------------
#endif // N_LW_EXPORTER_H
