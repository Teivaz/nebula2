 //-----------------------------------------------------------------------------
//  nmaxexport2_main.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "base/nmaxlistener.h"
#include "export2/nmaxexport2.h"
#include "tools/nmeshbuilder.h"
#include "export2/nmaxscene.h"
#include "export2/nmaxoptions.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"
#include "tools/napplauncher.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxExport2::nMaxExport2()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxExport2::~nMaxExport2()
{
}

//-----------------------------------------------------------------------------
/**
    Export a scene.
*/
int	nMaxExport2::DoExport(const TCHAR *name, 
    ExpInterface *ei, 
    Interface *inf, 
    BOOL suppressPrompts, 
    DWORD options)
{
    return ExportScene(name, inf);
}

//-----------------------------------------------------------------------------
/**
    Launch preview application.  
*/
static
bool LaunchViewer(const char* sceneFile)
{
    //nString appPath;
    nString appArgs;

    //appPath = nMaxOptions::Instance()->GetViewerDir();
    //appPath += "nmaxpreviewer.exe";

    appArgs += "-projdir ";
    appArgs += nMaxOptions::Instance()->GetHomePath();
    appArgs += " ";
    appArgs += "-view ";
    appArgs += nMaxOptions::Instance()->GetGfxLibAssign();
    appArgs += sceneFile;

    nAppLauncher appLauncher(nKernelServer::Instance());

    appLauncher.SetExecutable("nviewer.exe");

    nString cwd; 
    cwd += nMaxOptions::Instance()->GetHomePath(); 
#ifdef _DEBUG
    cwd += "\\bin\\win32d";
#else
    cwd += "\\bin\\win32";
#endif

    //appLauncher.SetWorkingDirectory("home:");
    appLauncher.SetWorkingDirectory(cwd.Get());
    appLauncher.SetArguments(appArgs.Get());

    return appLauncher.Launch();
}

//-----------------------------------------------------------------------------
/**
    Delete any exist singleton class instances.

    - 20-Feb-05 kim Removed release of nMaxOption instance. The option can be
                    globally available not just only for export.
*/
static
void ReleaseSingletons()
{
    nMaxInterface* intf    = nMaxInterface::Instance();
    nMaxLogDlg*    logDlg  = nMaxLogDlg::Instance();

    n_delete(intf);

    // idle until user click 'ok' button of log dialog.
    logDlg->Wait();
    n_delete(logDlg);
}

//-----------------------------------------------------------------------------
/**

*/
int ExportScene(const TCHAR* name, Interface* inf)
{
    n_assert(inf);

    //init the maxLogHandler to redirect error message of nebula to 3dsmax.
    nLogHandler* logHandler = nKernelServer::Instance()->GetLogHandler();
    static_cast<nMaxLogHandler*>(logHandler)->SetLogSys(inf->Log());

    nMaxLogDlg* logDlg = nMaxLogDlg::Instance();
    logDlg->Create();

    // create max interface.
    nMaxInterface* intf = nMaxInterface::Instance();

    if (!intf->Create(inf))
    {
        n_maxlog(Error, "Failed to create nMaxInterface class instance.");

        ReleaseSingletons();

        return 0;
    }

    // read options from .ini file.
    nMaxOptions* expOptions = nMaxOptions::Instance();
    if (!expOptions->Initialize())
    {
        n_maxlog(Error, "Faile to read '%s' file from 'plugcfg' directory.", 
            N_MAXEXPORT_INIFILE);

        ReleaseSingletons();

        return 0;
    }
    expOptions->SetSaveFileName(nString(name));

    // export scene.
    nMaxScene scene;
    //scene.SetSaveFileName(name);
    if (!scene.Export())
    {
        n_maxlog(Error, "Failed to export scene.");

        ReleaseSingletons();

        return 0;
    }

    // run nmaxpreviewer.
    if (expOptions->RunViewer())
    {
        nString saveFile = name;
        nString sceneFile = saveFile.ExtractFileName();

        if (LaunchViewer(sceneFile.Get()))
        {
            n_maxlog(Midium, "Launched preview application.");
        }
        else
        {
            n_maxlog(Midium, "Failed to launch preview application.");
        }
    }

    ReleaseSingletons();

    return 1;
}
