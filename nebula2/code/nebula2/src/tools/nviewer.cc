//------------------------------------------------------------------------------
/**
    @page NebulaToolsnviewer nviewer

    nviewer

    Selfcontained viewer application for Nebula.

    The Nebula2 viewer displays Nebula2 3D objects in realtime. It can be invoked
    from the command line (nviewer.exe) or from the start menu.

    The viewer offers an overlay 2D user interface which is activated by pressing
    the Escape key.
    The GUI offers builtin command consoles, a texture browser window, a graphics
    object browser window, and 2 windows with runtime and debug information.

    <dl>
     <dt>-startup</dt>
       <dd>startup script to run, default is: home:bin/startup.tcl</dd>
     <dt>-sceneserver</dt>
       <dd>scene server to use</dd>
     <dt>-scriptserver</dt>
       <dd>script server to use</dd>
     <dt>-view</dt>
       <dd>data to load and view with the default lighting setup</dd>
     <dt>-stage</dt>
       <dd>the light stage to load, default is: home:export/gfxlib/stdlight.n2</dd>
     <dt>-fullscreen</dt>
       <dd>if present, then nviewer will go fullscreen</dd>
     <dt>-alwaysontop</dt>
       <dd>if present the window will be allways on top</dd>
     <dt>-w</dt>
       <dd>width of window to open (default: 640)</dd>
     <dt>-h</dt>
       <dd>height of window to open (default: 480)</dd>
     <dt>-x</dt>
       <dd>the x position of the window (default: 0)</dd>
     <dt>-y</dt>
       <dd>the y position of the window (default: 0)</dd>
     <dt>-projdir</dt>
       <dd>the optional project directory (assigns it to the projdir: alias, for use in the user's scripts)</dd>
     <dt>-useram</dt>
       <dd>if present, then nviewer will use ram file</dd>
     <dt>-eyeposx</dt>
       <dd>position vector of camera matrix</dd>
     <dt>-eyecoix</dt>
       <dd>look vector of camera matrix</dd>
     <dt>-eyeupx</dt>
       <dd>up vector of camera matrix</dd>
    </dl>

    nviewer also defines some default input handling:

    @todo Document default inputhandling

    (C) 2002 RadonLabs GmbH
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "tools/nviewerapp.h"

#ifdef __WIN32__
#include "kernel/nwin32loghandler.h"
#include "tools/nwinmaincmdlineargs.h"
#else
#include "tools/ncmdlineargs.h"
#endif

nNebulaUsePackage(nnebula);
#ifdef __WIN32__
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndshow);
#endif
nNebulaUsePackage(ngui);
nNebulaUsePackage(nnetwork);
#ifdef __WIN32__
nNebulaUsePackage(ndsound);
#endif

//------------------------------------------------------------------------------
/*
    Win32 specific method which checks the registry for the Nebula2
    Toolkit's project path. If the reg keys are not found, the
    routine just returns 0.
*/
#ifdef __WIN32__
const char*
ReadProjRegistryKey()
{
    // read the project directory from the registry
    HKEY hKey;
    LONG err;
    err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\RadonLabs\\Toolkit", 0, KEY_QUERY_VALUE, &hKey);
    if (ERROR_SUCCESS != err)
    {
        return 0;
    }
    DWORD keyType;
    static char projectDir[512];
    DWORD projectDirSize = sizeof(projectDir);
    err = RegQueryValueEx(hKey, "project", 0, &keyType, (LPBYTE) &projectDir, &projectDirSize);
    if (ERROR_SUCCESS != err)
    {
        return 0;
    }
    return projectDir;
}
#endif

//------------------------------------------------------------------------------
/*
*/
#ifdef __WIN32__
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    nWinMainCmdLineArgs args(lpCmdLine);
#else
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);
#endif

    nString scriptServerClass    = args.GetStringArg("-scriptserver", "ntclserver");
    nString startupArg           = args.GetStringArg("-startup", "home:data/scripts/startup.tcl");
    nString viewArg              = args.GetStringArg("-view", "");
    nString stageArg             = args.GetStringArg("-stage", "home:export/gfxlib/stdlight.n2");
    bool fullscreenArg           = args.GetBoolArg("-fullscreen");
    bool alwaysOnTopArg          = args.GetBoolArg("-alwaysontop");
    bool useRam                  = args.GetBoolArg("-useram");
    bool noLightStage            = args.GetBoolArg("-nolightstage");
    bool helpArg                 = args.GetBoolArg("-help");
    int xPosArg                  = args.GetIntArg("-x", 0);
    int yPosArg                  = args.GetIntArg("-y", 0);
    int widthArg                 = args.GetIntArg("-w", 640);
    int heightArg                = args.GetIntArg("-h", 480);
    nString projDir              = args.GetStringArg("-projdir", "");
    nString sceneServerClass     = args.GetStringArg("-sceneserver", "");
    nString gfxServerClass       = args.GetStringArg("-gfxserver", "");
    nString featureSetArg        = args.GetStringArg("-featureset", "");
    nString renderPath           = args.GetStringArg("-renderpath", "");
    int aaSamples                = args.GetIntArg("-aa", 0);
    vector3 eyePos(args.GetFloatArg("-eyeposx", 0.0f), args.GetFloatArg("-eyeposy", 0.0f), args.GetFloatArg("-eyeposz", 9.0f));
    vector3 eyeCoi(args.GetFloatArg("-eyecoix", 0.0f), args.GetFloatArg("-eyecoiy", 0.0f), args.GetFloatArg("-eyecoiz", 0.0f));
    vector3 eyeUp(args.GetFloatArg("-eyeupx", 0.0f), args.GetFloatArg("-eyeupy", 1.0f), args.GetFloatArg("-eyeupz", 0.0f));

    // Don't allow window width smaller than 40 and height smaller than 30.
    if (widthArg < 40)
    {
        n_printf("Invalid window width. Using width of 40.\n");
        widthArg = 40;
    }
    if (heightArg < 30)
    {
        n_assert("Invalid window height. Using height of 30.\n");
        heightArg = 30;
    }

    // If the user needs an explanation, just provide one, and don't do anything else this execution
    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
               "nviewer - Nebula2 Object Viewer\n"
               "Command line args:\n"
               "------------------\n"
               "-help                   show this help\n"
               "-startup                startup script to run, default is: home:bin/startup.tcl\n"
               "-sceneserver            scene server to use, default is nmrtsceneserver\n"
               "-scriptserver           script server to use\n"
               "-gfxserver              graphics server to use; default is platform dependent\n"
               "-featureset             Which shader feature set to use; One of: dx7, dx8, dx8sb, dx9, dx9flt\n"
               "-view                   data to load and view with the lighting setup specified (either -stage or the default)\n"
               "-stage                  light stage to load, default is: home:export/gfxlib/stdlight.n2\n"
               "-fullscreen             if present, then nviewer will go fullscreen\n"
               "-alwaysontop            present the window will be allways on top\n"
               "-w                      width of window to open (default: 640)\n"
               "-h                      height of window to open (default: 480)\n"
               "-x                      the x position of the window (default: 0)\n"
               "-y                      y position of the window (default: 0)\n"
               "-projdir                the optional project directory (assigns it to the projdir: alias, for use in the user's scripts)\n"
               "-useram                 if present, then nviewer will use ram file\n"
               "-eyeposx                position vector of camera matrix\n"
               "-eyecoix                look vector of camera matrix\n"
               "-eyeupx                 up vector of camera matrix\n"
               "-aa                     number of aa samples\n");
        return 5;
    }

    // initialize Nebula runtime
    nKernelServer kernelServer;
    #ifdef __WIN32__
        nWin32LogHandler logHandler("nviewer");
        kernelServer.SetLogHandler(&logHandler);
    #endif
    kernelServer.AddPackage(nnebula);
    #ifdef __WIN32__
    kernelServer.AddPackage(ndinput8);
    kernelServer.AddPackage(ndirect3d9);
    kernelServer.AddPackage(ndshow);
    #endif
    kernelServer.AddPackage(ngui);
    kernelServer.AddPackage(nnetwork);
    #ifdef __WIN32__
    kernelServer.AddPackage(ndsound);
    #endif

    // initialize a display mode object
    nString title;
    if (viewArg.IsValid())
    {
        title.Append(viewArg);
        title.Append(" - ");
    }
    title.Append("Nebula2 viewer");
    nDisplayMode2 displayMode;
    displayMode.SetWindowTitle("Nebula2 Viewer");
    displayMode.SetIcon("Icon");
    displayMode.SetDialogBoxMode(true);
    displayMode.SetXPos(xPosArg);
    displayMode.SetYPos(yPosArg);
    displayMode.SetWidth(widthArg);
    displayMode.SetHeight(heightArg);
    displayMode.SetVerticalSync(false);
    displayMode.SetAntiAliasSamples(aaSamples);
    if (fullscreenArg)
    {
        displayMode.SetType(nDisplayMode2::Fullscreen);
    }
    else if (alwaysOnTopArg)
    {
        displayMode.SetType(nDisplayMode2::AlwaysOnTop);
    }
    else
    {
        displayMode.SetType(nDisplayMode2::Windowed);
    }

    // under Win32 check if we should read the project directory from the registry
    // (registry keys used by the Nebula2 toolkit)
    #ifdef __WIN32__
        if (0 == projDir)
        {
            const char* regProjDir = ReadProjRegistryKey();
            if (regProjDir)
            {
                projDir = regProjDir;
            }
        }
    #endif

    // initialize a viewer app object
    nViewerApp viewerApp;
    viewerApp.SetDisplayMode(displayMode);
    viewerApp.SetUseRam(useRam);
    viewerApp.SetLightStageEnabled(!noLightStage);
    if (gfxServerClass.IsValid())   viewerApp.SetGfxServerClass(gfxServerClass);
    if (sceneServerClass.IsValid()) viewerApp.SetSceneServerClass(sceneServerClass);
    if (scriptServerClass.IsValid()) viewerApp.SetScriptServerClass(scriptServerClass);
    if (viewArg.IsValid())          viewerApp.SetSceneFile(viewArg);
    if (projDir.IsValid())          viewerApp.SetProjDir(projDir);
    if (startupArg.IsValid())       viewerApp.SetStartupScript(startupArg);
    if (renderPath.IsValid())       viewerApp.SetRenderPath(renderPath);
    if (featureSetArg.IsValid())
    {
        nGfxServer2::FeatureSet featureSet = nGfxServer2::StringToFeatureSet(featureSetArg.Get());
        if (nGfxServer2::InvalidFeatureSet == featureSet)
        {
            n_error("Invalid feature set string specified: %s", featureSetArg.Get());
        }
        viewerApp.SetFeatureSetOverride(featureSet);
    }

    viewerApp.SetStageScript(stageArg);

    //set viewer propherties
    viewerApp.GetCamControl().SetDefaultCenterOfInterest(eyeCoi);
    viewerApp.GetCamControl().SetDefaultEyePos(eyePos);
    viewerApp.GetCamControl().SetDefaultUpVec(eyeUp);

    // open and run viewer
    if (viewerApp.Open())
    {
        viewerApp.Run();
        viewerApp.Close();
    }
    return 0;
}

