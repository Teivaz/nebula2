//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwserver.h>
#include <lwsdk/lwhandler.h>
#include <lwsdk/lwpanel.h>
#include <lwsdk/lwhost.h>
}

#include "lwexporter/nlwexportermaster.h"
#include "lwexporter/nlwexportermasterpanel.h"
#include "lwwrapper/nlwglobals.h"
#include "kernel/ntypes.h"
#include <string.h> // for stricmp()
#include "util/nstring.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nwin32loghandler.h"
#include "lwexporter/nlwshaderpanelfactory.h"
#include "lwexporter/nlwsettingsregistry.h"
#include "lwexporter/nlwexportersettings.h"
#include "lwexporter/nlwviewerremote.h"

//----------------------------------------------------------------------------
nLWExporterMaster* nLWExporterMaster::singleton = 0;

nNebulaUsePackage(nnebula);

//----------------------------------------------------------------------------
/**
*/
nLWExporterMaster::nLWExporterMaster() :
    kernelServer(0),
    logHandler(0),
    viewerRemote(0)
{
    nLWExporterMaster::singleton = this;

    // startup Nebula 2
    this->kernelServer = n_new(nKernelServer);
#ifdef __WIN32__
    this->logHandler = n_new(nWin32LogHandler("n2lwexporter"));
#endif
    this->kernelServer->SetLogHandler(this->logHandler);
    this->kernelServer->AddPackage(nnebula);

    // load exporter settings
    nLWExporterSettings* exporterSettings = nLWExporterSettings::Instance();
    n_assert(exporterSettings);
    if (exporterSettings)
    {
        exporterSettings->Load();
        nFileServer2* fs = this->kernelServer->GetFileServer();
        if (fs)
        {
            fs->SetAssign("proj", exporterSettings->GetProjectDir());
            fs->SetAssign("textures", "proj:export/textures");
        }
    }

    // load shader panels
    nLWShaderPanelFactory* shaderPanelFactory = nLWShaderPanelFactory::Instance();
    n_assert(shaderPanelFactory);
    if (shaderPanelFactory && exporterSettings)
    {
        shaderPanelFactory->SetShadersFile("proj:data/shaders/shaders.xml");
        // this will fail if the user didn't get a chance to set the project dir
        shaderPanelFactory->Load();
    }

    this->viewerRemote = n_new(nLWViewerRemote);
}

//----------------------------------------------------------------------------
/**
*/
nLWExporterMaster::~nLWExporterMaster()
{
    if (this->viewerRemote)
    {
        n_delete(this->viewerRemote);
        this->viewerRemote = 0;
    }

    // kill the singletons
    nLWSettingsRegistry::FreeInstance();
    nLWShaderPanelFactory::FreeInstance();
    nLWExporterSettings::FreeInstance();

    // shutdown Nebula 2
    if (this->kernelServer)
    {
        n_delete(this->kernelServer);
        this->kernelServer = 0;
    }

    if (this->logHandler)
    {
        n_delete(this->logHandler);
        this->logHandler = 0;
    }

    nLWExporterMaster::singleton = 0;

    // assumption: the master handler outlives everyone else
    // that assumption is wrong!
    //nLWGlobals::SetGlobalFunc(0);
}

//----------------------------------------------------------------------------
/**
*/
nLWExporterMaster* 
nLWExporterMaster::Instance()
{
    // unlike the other singletons like nLWExporterSettings the master is
    // a plugin and can only be created by Lightwave... which is why we
    // don't try to create a new instance here
    return nLWExporterMaster::singleton;
}

//----------------------------------------------------------------------------
/**
    This MUST be called when a new plugin is created (with the possible 
    exception of generics if you sure they can't outlive the master plugin).
*/
void nLWExporterMaster::PluginCreated()
{
    // we have to ensure the master is killed after any other plugin with
    // create, so we use reference counting to accomplish that
    nLWExporterMaster* master = nLWExporterMaster::Instance();
    n_assert(master);
    if (master)
    {
        master->AddRef();
    }
}

//----------------------------------------------------------------------------
/**
    This MUST be called when a plugin is destroyed (with the possible 
    exception of generics if you sure they can't outlive the master plugin).
*/
void nLWExporterMaster::PluginDestroyed()
{
    // decrement the master reference count we incremented in PluginCreated()
    nLWExporterMaster* master = nLWExporterMaster::Instance();
    n_assert(master);
    if (master)
    {
        master->Release();
    }
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(LWInstance) 
nLWExporterMaster::OnCreate(void* priv, void* context, LWError* error)
{
    if (!nLWExporterMaster::singleton)
    {
        return n_new(nLWExporterMaster);
    }
    
    *error = "Only one Nebula 2 Exporter Master Handler allowed per scene.";
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(void) 
nLWExporterMaster::OnDestroy(LWInstance instance)
{
    if (instance)
    {
        // we use reference counting to ensure the master is killed after
        // everyone else that may rely on it being around - because Lightwave
        // doesn't kill things in LIFO order
        ((nLWExporterMaster*)instance)->Release();
    }
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(LWError) 
nLWExporterMaster::OnCopy(LWInstance dest, LWInstance source)
{
    // don't support copy, should only be one instance of the master anyway
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWExporterMaster::OnLoad(const LWLoadState* loadState)
{
    // TODO: Load all plug-in data.
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWExporterMaster::OnSave(const LWSaveState* saveState)
{
    // TODO: Save all plug-in data.
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
const char*
nLWExporterMaster::OnGetDescription()
{
    return "Nebula 2 Exporter Master Handler";
}

//----------------------------------------------------------------------------
/**
*/
const LWItemID*
nLWExporterMaster::OnUseItems()
{
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterMaster::OnChangeID(const LWItemID* idList)
{

}

//----------------------------------------------------------------------------
/**
*/
double
nLWExporterMaster::OnEvent(const LWMasterAccess* access)
{
    return 0.0;
}

//----------------------------------------------------------------------------
/**
*/
unsigned int
nLWExporterMaster::OnFlags()
{
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWExporterMaster::OnDisplayUI()
{
    HostDisplayInfo* hdi = nLWGlobals::GetHostDisplayInfo();
    if (!hdi)
        return "Failed to obtain HostDisplayInfo";

    wxWindow parent;
    parent.SetHWND(hdi->window);
    parent.Enable(false);
    nLWExporterMasterPanel panel(&parent);
    panel.ShowModal();
    parent.Enable(true);
    parent.SetHWND(0);

    // no errors
    return 0;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
