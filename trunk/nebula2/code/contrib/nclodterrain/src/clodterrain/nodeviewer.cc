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
#include "clodterrain/nodeviewerapp.h"

#ifdef __WIN32__
#include "kernel/nwin32loghandler.h"
#include "tools/nwinmaincmdlineargs.h"
#else
#include "tools/ncmdlineargs.h"
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

    const char* scriptArg = args.GetStringArg("-script", 0);
    const char* viewArg   = args.GetStringArg("-view", 0);
    const char* stageArg  = args.GetStringArg("-stage", "home:bin/stdlight.tcl");
    bool fullscreenArg    = args.GetBoolArg("-fullscreen");
    bool alwaysOnTopArg   = args.GetBoolArg("-alwaysontop");
    int xPosArg           = args.GetIntArg("-x", 0);
    int yPosArg           = args.GetIntArg("-y", 0);
    int widthArg          = args.GetIntArg("-w", 640);
    int heightArg         = args.GetIntArg("-h", 480);
    const char* projDir   = args.GetStringArg("-projdir", 0);

//    scriptArg = "home:code/contrib/nclodterrain/bin/compilesplatterrain.lua";
//    scriptArg = "home:code/contrib/nclodterrain/bin/compileterrain.lua";
    if (scriptArg == NULL)
        viewArg = "home:code/contrib/nclodterrain/bin/clod.n2";
    
    // initialize a display mode object
    nString title;
    if (viewArg)
    {
        title.Append(viewArg);
        title.Append(" - ");
    }
    title.Append("Nebula2 viewer");
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
        nWin32LogHandler logHandler("nclododeviewer");
        kernelServer.SetLogHandler(&logHandler);
    #endif

    // initialize a viewer app object
    nODEViewerApp viewerApp(&kernelServer);
    viewerApp.SetDisplayMode(displayMode);
    if (viewArg)
    {
        viewerApp.SetSceneFile(viewArg);
    }
    if (projDir)
    {
        viewerApp.SetProjDir(projDir);
    }
    if (scriptArg)
    {
        viewerApp.SetStartupScript(scriptArg);
    }

    // open and run viewer
    if (viewerApp.Open())
    {
        viewerApp.Run();
        viewerApp.Close();
    }
    return 0;
}

