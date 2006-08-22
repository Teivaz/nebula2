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
#include "export2/nmaxcamera.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"
#include "pluginlibs/nmaxvieweroptions.h"

#include "tools/napplauncher.h"
#include "tools/nwinmaincmdlineargs.h"

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
    // it exports root node in the scene.
    return ExportScene(name, inf, 0, 0/*no use preview*/);
}

//-----------------------------------------------------------------------------
/**
    Launch preview application.  
*/
static
bool LaunchViewer(const char* sceneFile)
{
    // read viewer options.
    nMaxViewerOptions viewerOptions;

    if (nMaxOptions::Instance()->UseDefaultViewer())
        viewerOptions.SetViewerType(nMaxViewerOptions::ViewerType::Default);
    else
        viewerOptions.SetViewerType(nMaxViewerOptions::ViewerType::Custom);

    viewerOptions.SetSceneFileName(sceneFile);

    if (!viewerOptions.Read())
    {
        n_message("Failed to read viewer option.");
        return false;
    }

    // viewer arguments.
    nString appArgs;
    appArgs += viewerOptions.GetArguments();
    appArgs += " ";

// begin camera
    // extract camera info from the current viewport.
    nMaxCamera camera;
    //FIXME: 
    camera.ExtractFromViewport();

    vector3 eyepos = camera.GetEyePos();
    vector3 eyedir = camera.GetEyeDir();
    vector3 eyeup  = camera.GetEyeUp();

    nString strEyePosX, strEyePosY, strEyePosZ;
    strEyePosX.SetFloat(eyepos.x);
    strEyePosY.SetFloat(eyepos.y);
    strEyePosZ.SetFloat(eyepos.z);

    appArgs += "-eyeposx"; appArgs += " ";
    appArgs += strEyePosX; appArgs += " ";
    appArgs += "-eyeposy"; appArgs += " ";
    appArgs += strEyePosY; appArgs += " ";
    appArgs += "-eyeposz"; appArgs += " ";
    appArgs += strEyePosZ; appArgs += " ";

    nString strEyeDirX, strEyeDirY, strEyeDirZ;
    strEyeDirX.SetFloat(eyedir.x);
    strEyeDirY.SetFloat(eyedir.y);
    strEyeDirZ.SetFloat(eyedir.z);

    appArgs += "-eyecoix"; appArgs += " ";
    appArgs += strEyeDirX; appArgs += " ";
    appArgs += "-eyecoiy"; appArgs += " ";
    appArgs += strEyeDirY; appArgs += " ";
    appArgs += "-eyecoiz"; appArgs += " ";
    appArgs += strEyeDirZ; appArgs += " ";

    nString strEyeUpX, strEyeUpY, strEyeUpZ;
    strEyeUpX.SetFloat(eyeup.x);
    strEyeUpY.SetFloat(eyeup.y);
    strEyeUpZ.SetFloat(eyeup.z);

    appArgs += "-eyeupx"; appArgs += " ";
    appArgs += strEyeUpX; appArgs += " ";
    appArgs += "-eyeupy"; appArgs += " ";
    appArgs += strEyeUpY; appArgs += " ";
    appArgs += "-eyeupz"; appArgs += " ";
    appArgs += strEyeUpZ; appArgs += " ";
//end camera*/

    // launch the viewer application.
    nAppLauncher appLauncher;

    appLauncher.SetExecutable(viewerOptions.GetExecutable());

    nString cwd; 
    cwd += nMaxOptions::Instance()->GetHomePath(); 
#ifdef _DEBUG
    cwd += "\\bin\\win32d";
#else
    cwd += "\\bin\\win32";
#endif

    appLauncher.SetWorkingDirectory(cwd.Get());
    appLauncher.SetArguments(appArgs.Get());

    return appLauncher.Launch();
}

//-----------------------------------------------------------------------------
/**
    Delete any exist singleton class instances.

    - 20-Feb-05 kims Removed release of nMaxOption instance. The option can be
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
    //FIXME: 
    //n_delete(logDlg);
}

//-----------------------------------------------------------------------------
/**
    Export the given scene or node.

    @param name filename to save a exported scene(or node).
    @param inf  pointer to Interface class instance.
    @param node INode which to export. If Null is given, it exports scene root node.

    @return Return 0, if it failed to export.
*/
int ExportScene(const TCHAR* name, Interface* inf, INode* inode, int previewMode)
{
    n_assert(inf);

    // init the maxLogHandler to redirect error message of nebula to 3dsmax.
    nLogHandler* logHandler = nKernelServer::Instance()->GetLogHandler();
    static_cast<nMaxLogHandler*>(logHandler)->SetLogSys(inf->Log());

    // create log dialog.
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
        n_maxlog(Error, "Failed when the plugin reads '%s' file of '$3dsmax/plugcfg' directory.", 
            N_MAXEXPORT_INIFILE);

        ReleaseSingletons();

        return 0;
    }
    expOptions->SetSaveFileName(nString(name));

    // specifies preview mode.
    if (-1 == previewMode )
        expOptions->SetPreviewMode(true);
    else
        expOptions->SetPreviewMode(false);

    // export scene.
    nMaxScene scene;
    if (!scene.Export(inode))
    {
        n_maxlog(Error, "Failed to export scene.");

        ReleaseSingletons();

        return 0;
    }

    // run viewer app.
    if (expOptions->RunViewer())
    {
        nString saveFile = name;
        nString sceneFile = saveFile.ExtractFileName();

        if (LaunchViewer(sceneFile.Get()))
        {
            n_maxlog(Medium, "Launched viewer application.");
        }
        else
        {
            n_maxlog(Medium, "Failed to launch viewer application.");
        }
    }

    ReleaseSingletons();

    return 1;
}
