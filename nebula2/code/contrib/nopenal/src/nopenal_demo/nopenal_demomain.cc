//------------------------------------------------------------------------------
/**
    @page Nebula2 OpenAL ports

    nOpenAL

    OpenAL for Nebula2.

    <dl>
     <dt>-startup</dt>
       <dd>startup script to run, default is: home:bin/startup.tcl</dd>
     <dt>-sceneserver</dt>
       <dd>scene server to use</dd>
     <dt>-scriptserver</dt>
       <dd>script server to use</dd>
     <dt>-input</dt>
       <dd>input binding script to run, default is: home:bin/stdinput.tcl</dd>
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
        <dd>the optional project directory (assigns it to the projdir: alias, for use in the user's scripts)</dd>
    </dl>

    (C) 2004 Bang, Chang Kyu
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "nopenal_demo/nopenal_demo.h"

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

    const char* scriptserverArg = args.GetStringArg("-scriptserver", "ntclserver");
    const char* sceneserverArg = args.GetStringArg("-sceneserver", "nstdsceneserver");
    const char* startupArg = args.GetStringArg("-startup", "home:bin/startup.tcl");
    const char* viewArg   = args.GetStringArg("-view", 0);
    const char* stageArg  = args.GetStringArg("-stage", "home:bin/stdlight.tcl");
    const char* inputArg  = args.GetStringArg("-input", "home:bin/stdinput.tcl");
    bool fullscreenArg    = args.GetBoolArg("-fullscreen");
    bool alwaysOnTopArg   = args.GetBoolArg("-alwaysontop");
    bool helpArg           = args.GetBoolArg("-help");
    int xPosArg           = args.GetIntArg("-x", 0);
    int yPosArg           = args.GetIntArg("-y", 0);
    int widthArg          = args.GetIntArg("-w", 640);
    int heightArg         = args.GetIntArg("-h", 480);
    const char* projDir   = args.GetStringArg("-projdir", 0);

    // If the user needs an explanation, just provide one, and don't do anything else this execution
    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
               "nopenaldemo - Nebula2 OpenAL Demo\n"
               "Command line args:\n"
               "------------------\n"
               "-help                   show this help\n"
               "-startup                startup script to run, default is: home:bin/startup.tcl\n"
               "-sceneserver            scene server to use\n"
               "-scriptserver           script server to use\n"
               "-input                  input binding script to run, default is: home:bin/stdinput.tcl\n"
               "-view                   data to load and view with the lighting setup specified (either -stage or the default)\n"
               "-stage                  light stage to load, default is: home:bin/stdlight.tcl\n"
               "-fullscreen             if present, then nviewer will go fullscreen\n"
               "-alwaysontop            present the window will be allways on top\n"
               "-w                      width of window to open (default: 640)\n"
               "-h                      height of window to open (default: 480)\n"
               "-x                      the x position of the window (default: 0)\n"
               "-y                      y position of the window (default: 0)\n"
               "-projdir                the optional project directory (assigns it to the projdir: alias, for use in the user's scripts)\n");
        return 5;
    }
    
    // initialize a display mode object
    nString title;
    if (viewArg)
    {
        title.Append(viewArg);
        title.Append(" - ");
    }
    title.Append("Nebula2 - OpenAL Demo");
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
        nWin32LogHandler logHandler("nOpenAL");
        kernelServer.SetLogHandler(&logHandler);
    #endif

    // initialize a viewer app object
    nOpenALDemo nopenalDemo(&kernelServer);
    nopenalDemo.SetDisplayMode(displayMode);
    if (viewArg)
    {
        nopenalDemo.SetSceneFile(viewArg);
    }
    if (projDir)
    {
        nopenalDemo.SetProjDir(projDir);
    }
    
    nopenalDemo.SetScriptServerClass(scriptserverArg);
    nopenalDemo.SetSceneServerClass(sceneserverArg);
    nopenalDemo.SetStartupScript(startupArg);
    nopenalDemo.SetStageScript(stageArg);
    nopenalDemo.SetInputScript(inputArg);

    // open and run viewer
    if (nopenalDemo.Open())
    {
        nopenalDemo.Run();
        nopenalDemo.Close();
    }
    return 0;
}

