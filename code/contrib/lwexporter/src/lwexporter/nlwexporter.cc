//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwexporter.h"
#include "lwexporter/nlwexportersettings.h"
#include "lwexporter/nlwsettingsregistry.h"
#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "scene/ntransformnode.h"
#include "lwexporter/nlwexportnode.h"
#include "util/nstring.h"
#include "kernel/nfileserver2.h"
#include "tools/napplauncher.h"
#include "lwwrapper/nlwlayoutmonitor.h"

//----------------------------------------------------------------------------
/**
*/
nLWExporter::nLWExporter(nLWCmdExec* cmdExec) :
    exportMode(nLWExporter::EXPORT_SCENE),
    exportRoot(0),
    previewMode(false),
    cmdExec(cmdExec)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWExporter::~nLWExporter()
{
    this->ClearExportNodes();
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWExporter::Preview()
{
    this->previewMode = true;
    this->DoExport();
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWExporter::Export()
{
    this->previewMode = false;
    this->DoExport();
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWExporter::DoExport()
{
    nLWExporterSettings* exporterSettings = nLWExporterSettings::Instance();
    n_assert(exporterSettings);
    if (!exporterSettings)
        return;

    this->CollectExportNodes();

    nString monitorMsg;
    if (this->previewMode)
    {
        monitorMsg = "Exporting Assets for Preview";
    }
    else
    {
        monitorMsg = "Exporting Assets for Real Yo!";
    }

    nLWLayoutMonitor monitor(monitorMsg, LMO_REVIEW);
    int numNodesToExport = this->GetTotalExportNodeCount();
    
    monitorMsg.Format("[INFO] Exporting %d node(s).\n", numNodesToExport);
    monitor.Open(numNodesToExport, monitorMsg);
    n_printf(monitorMsg.Get());

    nKernelServer* ks = nKernelServer::Instance();

    // need a script server for saving n2 files
    ks->New("ntclserver", "/sys/servers/script");

    // need a variable server to create nSkinAnimator
    ks->New("nvariableserver", "/sys/servers/variable");

    // need this for character debugging
    //ks->New("nchardebugserver", "/sys/servers/chardebug");

    // set the script server that should be used for saving scene nodes
    //ks->GetPersistServer()->SetSaverClass(exporterSettings->GetScriptServer());

    /*
    // use the ram file server for preview
    if (this->previewMode)
    {
        ks->ReplaceFileServer("nramfileserver");
    }
    else
    {
        ks->ReplaceFileServer("nfileserver2");
    }
    */
    
    nFileServer2* fs = ks->GetFileServer();
    fs->SetAssign("proj", exporterSettings->GetProjectDir());
    fs->SetAssign("gfxlib", "proj:export/gfxlib");
    fs->SetAssign("meshes", "proj:export/meshes");
    fs->SetAssign("textures", "proj:export/textures");
    fs->SetAssign("anims", "proj:export/anims");

    // .n2 file name
    this->n2FileName.Clear();

    // all scene nodes will be created under this root node
    this->exportRoot = static_cast<nTransformNode*>(ks->NewNoFail("ntransformnode", 
                                                                  "/export"));

    if (this->exportRoot)
    {
        ks->PushCwd(this->exportRoot);

        // create the scene nodes, export the mesh and animations
        for (int i = 0; i < this->topLevelExportNodes.Size(); i++)
        {
            this->topLevelExportNodes[i]->Export(&monitor);
            if (monitor.UserAborted())
                break;
        }

        if (!monitor.UserAborted())
        {
            // TODO: compute the bounding box for the export node

            this->ConstructN2FileName(&monitor);

            // now write out the whole export scene node tree
            if (this->exportRoot->SaveAs(this->n2FileName.Get()))
            {
                monitorMsg.Format("[INFO] Saved scene node file as %s\n", 
                                  this->n2FileName.Get());
            }
            else
            {
                monitorMsg.Format("[ERROR] Failed to save scene node file as %s\n",
                                  this->n2FileName.Get());
                if (this->n2FileName.IsEmpty())
                {
                    monitorMsg += "<unknown file>";
                }
                else
                {
                    monitorMsg += this->n2FileName;
                }
            }
            monitor.Step(0, monitorMsg);
            n_printf(monitorMsg.Get());
        }
    }

    // cleanup

    if (this->exportRoot)
    {
        this->exportRoot->Release();
        this->exportRoot = 0;
    }

    this->ClearExportNodes();

    if (this->previewMode)
    {
        if (!monitor.UserAborted())
        {
            // launch preview app
            monitorMsg = "[INFO] Launching nViewer.\n";
            monitor.Step(0, monitorMsg);
            n_printf(monitorMsg.Get());
            if (!this->n2FileName.IsEmpty())
                this->LaunchNebulaViewer(n2FileName);
        }
    }

    monitor.Close();
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWExporter::ConstructN2FileName(nLWLayoutMonitor* monitor)
{
    nLWGlobals::SceneInfo sceneInfo;
    n_assert(sceneInfo.IsValid());
    if (sceneInfo.IsValid())
    {
        // the absolute path to the scene file
        nString sceneFileName = sceneInfo.Get()->filename;
        nString sceneFileDir = sceneFileName.ExtractToLastSlash();
        sceneFileDir.ToLower();
        sceneFileDir.ConvertBackslashes();
        sceneFileDir.StripTrailingSlash();
        sceneFileDir += "/";

        // the absolute path to the current Lightwave Scenes directory
        nLWGlobals::DirInfoFunc dirInfo;
        nString lwScenesDir = dirInfo.GetScenesDir();
        lwScenesDir.ToLower();
        lwScenesDir.ConvertBackslashes();
        lwScenesDir.StripTrailingSlash();
        lwScenesDir += "/";

        this->n2FileName = "gfxlib:";
        // figure out what the category dir is and append it to the filename
        if (sceneFileDir.Length() > lwScenesDir.Length())
        {
            if (strncmp(sceneFileDir.Get(), lwScenesDir.Get(), lwScenesDir.Length()) == 0)
            {
                this->n2FileName += &(sceneFileDir.Get()[lwScenesDir.Length()]);
            }
            else
            {
                nString msg;
                msg.Format("[ERROR] Scene file %s was not found in the current "
                           "Lightwave Content directory.\n", sceneFileDir.Get());
                if (monitor)
                {
                    monitor->Step(0, msg);
                }
                n_printf(msg.Get());
            }
        }
        this->n2FileName += sceneFileName.ExtractFileName();
        this->n2FileName.StripExtension();
        this->n2FileName += ".n2";
    }
}

//----------------------------------------------------------------------------
/**
    @brief Get the total number of nodes to be exported.
    @note CollectExportNodes() must have been called for this to work.
*/
int 
nLWExporter::GetTotalExportNodeCount()
{
    int count = 0;
    for (int i = 0; i < this->topLevelExportNodes.Size(); i++)
    {
        count += this->topLevelExportNodes[i]->GetNumDescendants() + 1;
    }
    return count;
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWExporter::ClearExportNodes()
{
    for (int i = 0; i < this->topLevelExportNodes.Size(); i++)
    {
        n_delete(this->topLevelExportNodes[i]);
    }
    this->topLevelExportNodes.Reset();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporter::CollectExportNodes()
{
    // clear the current list (should've already been cleared earlier, but just in case...)
    this->ClearExportNodes();

    nLWGlobals::ItemInfo itemInfo;
    n_assert(itemInfo.IsValid());
    if (!itemInfo.IsValid())
        return;

    nLWSettingsRegistry* registry = nLWSettingsRegistry::Instance();
    n_assert(registry);
    if (!registry)
        return;

    // collect all export nodes
    if (nLWExporter::EXPORT_SELECTION == this->exportMode)
    {
        nLWGlobals::InterfaceInfo interfaceInfo;
        n_assert(interfaceInfo.IsValid());
        if (!interfaceInfo.IsValid())
            return;

        // collect the export nodes for all selected items and
        // their children (if the "export hierarchy" feature is
        // enabled)
        int numSelectedItems = 0;
        while (interfaceInfo.Get()->selItems[numSelectedItems++]);
        nArray<nLWExportNode*> selectedExportNodes(numSelectedItems, 0);

        for (int i = 0; i < numSelectedItems; i++)
        {
            LWItemID curItemId = interfaceInfo.Get()->selItems[i];
            switch (itemInfo.Get()->type(curItemId))
            {
                case LWI_OBJECT:
                {
                    nLWObjectExportSettings* settings = registry->Get(curItemId);
                    if (settings)
                    {
                        nLWExportNode* expNode = n_new(nLWExportNode(settings, this->cmdExec));
                        expNode->CollectDescendants(&itemInfo, registry);
                        selectedExportNodes.PushBack(expNode);
                    }
                    break;
                }

                default:
                    break;
            }
        }

        // exclude any export nodes that are parented and will be exported by 
        // a parent (it may not be exported if the "export hierarchy" feature 
        // is disabled for the parent)
        for (int i = 0; i < selectedExportNodes.Size(); i++)
        {
            bool exportedByAncestor = false;
            for (int j = 0; j < selectedExportNodes.Size(); j++)
            {
                if ((i != j) && selectedExportNodes[j]->IsAncestorOf(selectedExportNodes[i]))
                {
                    exportedByAncestor = true;
                    break;
                }
            }

            if (!exportedByAncestor)
            {
                this->topLevelExportNodes.PushBack(selectedExportNodes[i]);
            }
        }

        // free up any export nodes that are exported by a parent
        // (because the parent will have it's own copy)
        int numNodesToKill = selectedExportNodes.Size() - this->topLevelExportNodes.Size();
        if (numNodesToKill > 0)
        {
            for (int i = 0; i < selectedExportNodes.Size(); i++)
            {
                if (-1 == this->topLevelExportNodes.FindIndex(selectedExportNodes[i]))
                {
                    n_delete(selectedExportNodes[i]);
                    selectedExportNodes[i] = 0;
                    if (--numNodesToKill == 0)
                        break;
                }
            }
        }

        // don't want to delete any nodes it still contains, 
        // so just reset the element count to zero
        selectedExportNodes.Reset();
    }
    else if (nLWExporter::EXPORT_SCENE == this->exportMode)
    {
        LWItemID curItemId = itemInfo.Get()->first(LWI_OBJECT, LWITEM_NULL);
        while (curItemId)
        {
            if (LWITEM_NULL == itemInfo.Get()->parent(curItemId))
            {
                nLWObjectExportSettings* settings = registry->Get(curItemId);
                if (settings)
                {
                    nLWExportNode* expNode = n_new(nLWExportNode(settings, this->cmdExec));
                    expNode->CollectDescendants(&itemInfo, registry);
                    this->topLevelExportNodes.PushBack(expNode);
                }
            }
            curItemId = itemInfo.Get()->next(curItemId);
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWExporter::LaunchNebulaViewer(const nString& n2FileName)
{
    nLWExporterSettings* settings = nLWExporterSettings::Instance();
    n_assert(settings);
    if (!settings)
        return false;

    nAppLauncher appLauncher;
    appLauncher.SetExecutable("nviewer.exe");

    nString projDir(settings->GetProjectDir());

    nString viewerDir(projDir);
    viewerDir.StripTrailingSlash();
#ifdef _DEBUG
    viewerDir += "/bin/win32d";
#else
    viewerDir += "/bin/win32";
#endif
    appLauncher.SetWorkingDirectory(viewerDir.Get());
    
    nString appArgs;
    appArgs.Format("-projdir %s -w %d -h %d -view %s",
                   projDir.Get(),
                   settings->GetPreviewWidth(),
                   settings->GetPreviewHeight(),
                   n2FileName.Get());
    if (settings->GetPreviewFullScreen())
        appArgs += " -fullscreen ";
    
    nString renderer(settings->GetPreviewRenderer());
    nString featureSet(settings->GetPreviewRendererFeatureSet(renderer));
    if (!featureSet.IsEmpty())
    {
        appArgs += " -featureset ";
        appArgs += featureSet;
    }
    
    // get camera info
    nLWGlobals::SceneInfo sceneInfo;
    n_assert(sceneInfo.IsValid());
    if (!sceneInfo.IsValid())
        return false;

    nLWGlobals::InterfaceInfo interfaceInfo;
    n_assert(interfaceInfo.IsValid());
    if (!interfaceInfo.IsValid())
        return false;

    nLWGlobals::ItemInfo itemInfo;
    n_assert(itemInfo.IsValid());
    if (!itemInfo.IsValid())
        return false;

    LWTime curTime = interfaceInfo.Get()->curTime;
    LWItemID curCamId = sceneInfo.Get()->renderCamera(curTime);
    if (curCamId)
    {
        LWDVector lwVec;
        vector3 vec;
        nString vecStr;
        // eye position
        itemInfo.Get()->param(curCamId, LWIP_W_POSITION, curTime, lwVec);
        vec.set((float)lwVec[0], (float)lwVec[1], (float)lwVec[2]);
        vecStr.Format(" -eyeposx %f -eyeposy %f -eyeposz %f", vec.x, vec.y, -vec.z);
        appArgs += vecStr;
        // eye direction
        itemInfo.Get()->param(curCamId, LWIP_FORWARD, curTime, lwVec);
        vec.set((float)lwVec[0], (float)lwVec[1], (float)lwVec[2]);
        vec.norm();
        vecStr.Format(" -eyecoix %f -eyecoiy %f -eyecoiz %f", vec.x, vec.y, -vec.z);
        appArgs += vecStr;
        // eye up
        itemInfo.Get()->param(curCamId, LWIP_UP, curTime, lwVec);
        vec.set((float)lwVec[0], (float)lwVec[1], (float)lwVec[2]);
        vec.norm();
        vecStr.Format(" -eyeupx %f -eyeupy %f -eyeupz %f", vec.x, vec.y, vec.z);
        appArgs += vecStr;
    }

    appLauncher.SetArguments(appArgs.Get());
    nString logAppArgs(appArgs + "\n");
    n_printf(logAppArgs.Get());

    return appLauncher.Launch();
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
