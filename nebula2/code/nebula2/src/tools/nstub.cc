//------------------------------------------------------------------------------
//  nstub.cc
//
//  A small starter program which can be used to start other programs. Useful
//  to start large installers from CDROM through IE.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "tools/napplauncher.h"

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

    nKernelServer kernelServer;
    #ifdef __WIN32__
        nWin32LogHandler logHandler("nstub.exe - App Launcher");
        kernelServer.SetLogHandler(&logHandler);
    #endif

    const char* appArg = args.GetStringArg("-app", "bin:Setup.exe");
    const char* dirArg = args.GetStringArg("-dir", "bin:");
    bool helpArg       = args.GetBoolArg("-help");
    if (helpArg)
    {
        kernelServer.Message("nstub -- app launcher for CDROM installations\n"
                             "(C) 2004 RadonLabs GmbH\n\n"
                             "-app  - path to executable to launch (default is 'bin:Setup.exe')\n"
                             "-dir  - optional working directory (default is 'bin:'\n"
                             "-help - show this help\n");
        return 5;
    }
    if (0 == appArg)
    {
        kernelServer.Message("Error: -app arg expected (type 'nstub -help' for help)\n");
        return 10;
    }

    // configure an app launcher
    nAppLauncher appLauncher(&kernelServer);
    appLauncher.SetExecutable(appArg);
    if (dirArg)
    {
        appLauncher.SetWorkingDirectory(dirArg);
    }
    appLauncher.SetNoConsoleWindow(true);
    if (!appLauncher.Launch())
    {
        kernelServer.Message("Error: failed to launch applicaion '%s'!\n", appArg, dirArg);
        return 10;
    }
    return 0;
}
