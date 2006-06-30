/**
   @file networkdemo.cc
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Main function of Network Demo application.
   @brief $Id$

   This file is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
/**
	@page NebulaDemosnetworkdemor networkdemo

    networkdemo

    Example of use nNetwork module.

    <dl>
     <dt>-fullscreen</dt>
       <dd>if present, then nviewer will go fullscreen</dd>
     <dt>-w</dt>
       <dd>width of window to open (default: 640)</dd>
     <dt>-h</dt>
       <dd>height of window to open (default: 480)</dd>
	 <dt>-x</dt>
		<dd>the x position of the window (default: 0)</dd>
	 <dt>-y</dt>
		<dd>the y position of the window (default: 0)</dd>
    </dl>

*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "networkdemo/networkdemoapp.h"

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
    bool fullscreenArg    = args.GetBoolArg("-fullscreen");
    bool helpArg           = args.GetBoolArg("-help");
    int xPosArg           = args.GetIntArg("-x", 0);
    int yPosArg           = args.GetIntArg("-y", 0);
    int widthArg          = args.GetIntArg("-w", 640);
    int heightArg         = args.GetIntArg("-h", 480);

    if (helpArg)
    {
        printf("NetworkDemo - demo of nNetwork\n"
               "Command line args:\n"
               "------------------\n"
               "-help                   show this help\n"
               "-fullscreen             if present, then app will go fullscreen\n"
               "-w                      width of window to open (default: 640)\n"
               "-h                      height of window to open (default: 480)\n"
               "-x                      the x position of the window (default: 0)\n"
               "-y                      y position of the window (default: 0)\n");
        return 5;
    }

     // initialize a display mode object
    nString title;

    title.Append("NetworkDemo");
    nDisplayMode2 displayMode;
    if (fullscreenArg)
    {
        displayMode.Set(title.Get(), nDisplayMode2::Fullscreen, xPosArg, yPosArg, widthArg, heightArg, true, false, 0);
    }
    else
    {
        displayMode.Set(title.Get(), nDisplayMode2::Windowed, xPosArg, yPosArg, widthArg, heightArg, true, false, 0);
    }

    // initialize Nebula runtime
    nKernelServer kernelServer;
    #ifdef __WIN32__
        nWin32LogHandler logHandler("NetworkDemo");
        kernelServer.SetLogHandler(&logHandler);
    #endif

    // initialize a viewer app object
    NetworkDemoApp NetworkDemoApp(&kernelServer);
    NetworkDemoApp.SetDisplayMode(displayMode);

    // open and run viewer
    if (NetworkDemoApp.Open())
    {
        NetworkDemoApp.Run();
        NetworkDemoApp.Close();
    }
    return 0;
}
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
