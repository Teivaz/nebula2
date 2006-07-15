//------------------------------------------------------------------------------
/**
    @page NebulaToolsnviewer nviewer

    nviewer

    Selfcontained viewer application for Nebula.

    <dl>
     <dt>-script</dt>
       <dd>script to run</dd>
     <dt>-view</dt>
       <dd>data to load and view with the default lighting setup</dd>
     <dt>-stage</dt>
        <dd>the light stage to load, default is: home:bin/stdlight.tcl </dd>
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
        <dd>the project directory</dd>
    </dl>

    nviewer also defines some default input handling:

    @todo Document default inputhandling

    (C) 2002 RadonLabs GmbH
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "sdbviewer/nsdbviewerapp.h"

#ifdef __WIN32__
#include "kernel/nwin32loghandler.h"
#include "tools/nwinmaincmdlineargs.h"
#else
#include "tools/ncmdlineargs.h"
#endif

nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndshow);
nNebulaUsePackage(ngui);
nNebulaUsePackage(nlua);
nNebulaUsePackage(nspatialdb);

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

    const char* scriptserverArg = args.GetStringArg("-scriptserver", "nluaserver");
    const char* sceneserverArg = args.GetStringArg("-sceneserver", "nmrtsceneserver");
    const char* startupArg = args.GetStringArg("-startup", "home:code/contrib/nspatialdb/bin/startup.lua");
    const char* viewArg   = args.GetStringArg("-view", 0);
    const char* stageArg  = args.GetStringArg("-stage", "luascript:stdlight.lua");
    const char* inputArg  = args.GetStringArg("-input", "luascript:stdinput.lua");
    bool fullscreenArg    = args.GetBoolArg("-fullscreen");
    bool alwaysOnTopArg   = args.GetBoolArg("-alwaysontop");
    int xPosArg           = args.GetIntArg("-x", 0);
    int yPosArg           = args.GetIntArg("-y", 0);
    int widthArg          = args.GetIntArg("-w", 640);
    int heightArg         = args.GetIntArg("-h", 480);
    const char* projDir   = args.GetStringArg("-projdir", 0);

    if (viewArg == NULL)
        viewArg = "localgfxlib:unitsphere.n2";

    // initialize a display mode object
    nString title;
    if (viewArg)
    {
        title.Append(viewArg);
        title.Append(" - ");
    }
    title.Append("SpatialDB viewer");
    nDisplayMode2 displayMode;
    if (fullscreenArg)
    {
        displayMode.Set(title.Get(), nDisplayMode2::Fullscreen, xPosArg, yPosArg, widthArg, heightArg, true);
    }
    else if (alwaysOnTopArg)
    {
        displayMode.Set(title.Get(), nDisplayMode2::AlwaysOnTop, xPosArg, yPosArg, widthArg, heightArg, true);
    }
    else
    {
        displayMode.Set(title.Get(), nDisplayMode2::Windowed, xPosArg, yPosArg, widthArg, heightArg, true);
    }

    // initialize Nebula runtime
    nKernelServer kernelServer;
    #ifdef __WIN32__
        nWin32LogHandler logHandler("nsdbviewer");
        kernelServer.SetLogHandler(&logHandler);
    #endif
    kernelServer.AddPackage(nnebula);
    kernelServer.AddPackage(ndinput8);
    kernelServer.AddPackage(ndirect3d9);
    kernelServer.AddPackage(ngui);
    kernelServer.AddPackage(nlua);
    kernelServer.AddPackage(nspatialdb);

    // initialize a viewer app object
    nSDBViewerApp viewerApp(&kernelServer);
    viewerApp.SetDisplayMode(displayMode);
    if (viewArg)
    {
        viewerApp.SetSceneFile(viewArg);
    }
    if (projDir)
    {
        viewerApp.SetProjDir(projDir);
    }
    viewerApp.SetScriptServerClass(scriptserverArg);
    viewerApp.SetSceneServerClass(sceneserverArg);
    viewerApp.SetStartupScript(startupArg);
    viewerApp.SetStageScript(stageArg);
    viewerApp.SetInputScript(inputArg);

    // open and run viewer
    if (viewerApp.Open())
    {
        viewerApp.Run();
        viewerApp.Close();
    }
    return 0;
}

