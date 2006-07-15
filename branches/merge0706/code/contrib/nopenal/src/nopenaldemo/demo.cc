//----------------------------------------------------------------------------
//  demo.cc
//
//  (C)2005 Kim, Hyoun Woo
//----------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#ifdef __WIN32__
#include "kernel/nwin32loghandler.h"
#include "tools/nwinmaincmdlineargs.h"
#else
#include "tools/ncmdlineargs.h"
#endif
#include "gfx2/ndisplaymode2.h"
#include "nopenaldemo/nappopenaldemo.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndsaudioserver3);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndshow);
nNebulaUsePackage(ngui);
nNebulaUsePackage(nnetwork);
nNebulaUsePackage(napplication);
nNebulaUsePackage(nopenaldemo);
nNebulaUsePackage(nopenal);

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
    nWin32LogHandler logHandler("nappopenaldemo");
    kernelServer.SetLogHandler(&logHandler);
#endif

    
    kernelServer.AddPackage(nnebula);
    kernelServer.AddPackage(ndirect3d9);
    kernelServer.AddPackage(ndinput8);
    kernelServer.AddPackage(ndsaudioserver3);
    kernelServer.AddPackage(ndshow);
    kernelServer.AddPackage(ngui);
    kernelServer.AddPackage(napplication);    
    kernelServer.AddPackage(nopenaldemo);
    kernelServer.AddPackage(nopenal);

    // create the application.
    nAppOpenALDemo *app = (nAppOpenALDemo*)kernelServer.New("nappopenaldemo", "nappopenaldemo");

    // application settings.
    const char* appName = "appname";
    const char* companyName = "Kim, Hyoun Woo";

    const char* title = "Nebula2 OpenAL Demo";
    nDisplayMode2::Type type = nDisplayMode2::Windowed;
    int w = 640;
    int h = 480;
    bool vsync = false;
    nDisplayMode2::Bpp bpp = nDisplayMode2::Bpp32;

    nDisplayMode2 disp;
    disp.Set(title, type, 0, 0, w, h, vsync);
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

