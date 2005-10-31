//----------------------------------------------------------------------------
//  %(appNameL)s.cc
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#ifdef __WIN32__
#include "kernel/nwin32loghandler.h"
#include "tools/nwinmaincmdlineargs.h"
#else
#include "tools/ncmdlineargs.h"
#endif
#include "gfx2/ndisplaymode2.h"
#include "%(subDirL)s/%(classNameL)s.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndsound);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndshow);
nNebulaUsePackage(ngui);
nNebulaUsePackage(nnetwork);
nNebulaUsePackage(napplication);
%(scriptServerPackage)s
nNebulaUsePackage(%(targetName)s);

%(pythonRegPackage)s

//----------------------------------------------------------------------------
/**
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
    // Uncomment and add your own project directory if it is necessary.
    //const char* projDir   = args.GetStringArg("-projdir", 0);

    // Kernel Server preparation:
    nKernelServer kernelServer;
#ifdef __WIN32__
    nWin32LogHandler logHandler("%(classNameL)s");
    kernelServer.SetLogHandler(&logHandler);
#endif

    %(addPackageDef)s

    // create the application.
    %(className)s *app = (%(className)s*)kernelServer.New("%(classNameL)s", "%(appObjName)s");

    // application settings.
    const char* appName = "%(appName)s";
    const char* companyName = "%(companyName)s";

    const char* title = "%(windowTitle)s";
    nDisplayMode2::Type type = %(windowType)s;
    int w = %(windowWidth)s;
    int h = %(windowHeight)s;
    bool vsync = %(vsync)s;
    nDisplayMode2::Bpp bpp = %(bpp)s;

    nDisplayMode2 disp;
    disp.Set(title, type, 0, 0, w, h, vsync, false, "NEBULAICON");
    disp.SetBpp(bpp);

    app->SetAppName(appName);
    app->SetCompanyName(companyName);
    app->SetDisplayMode(disp);

    // Uncomment and add your own project directory if it is necessary.
    //app->SetProjectDirectory(projDir);
    // Uncomment and add your own startup script if it is necessary.
    //app->SetStartupScript("home:<your own startup script>");
    if (app->Open())
    {
        app->Run();
    }

    app->Close();
    app->Release();

    return 0;
}

