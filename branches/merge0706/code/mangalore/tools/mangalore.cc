//------------------------------------------------------------------------------
//  mangalore.cc
//
//  The Mangalore level launcher application.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/mangaapp.h"
#include "tools/ncmdlineargs.h"
#include "tools/nwinmaincmdlineargs.h"
#include "gfx2/ndisplaymode2.h"

#include <windows.h>

//------------------------------------------------------------------------------
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

    // get command line arguments
    const char* levelArg   = args.GetStringArg("-level", "levels:_preview.xml");
    const char* projDirArg = args.GetStringArg("-projdir", 0);
    bool fullscreenArg     = args.GetBoolArg("-fullscreen");
    int xPosArg            = args.GetIntArg("-x", 0);
    int yPosArg            = args.GetIntArg("-y", 0);
    int widthArg           = args.GetIntArg("-w", 640);
    int heightArg          = args.GetIntArg("-h", 480);
    const char* title      = args.GetStringArg("-title", "Mangalore Level Viewer");

    // define display mode
    nDisplayMode2 displayMode;
    if (fullscreenArg)
    {
        displayMode.Set(title, nDisplayMode2::Fullscreen, xPosArg, yPosArg, widthArg, heightArg, true);
    }
    else
    {
        displayMode.Set(title, nDisplayMode2::Windowed, xPosArg, yPosArg, widthArg, heightArg, true);
    }

    // initialize an application object
    Application::MangaApp app;
    app.SetProjectDirectory(projDirArg);
    app.SetDisplayMode(displayMode);
    app.SetLevelPath(levelArg);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    return 0;
}




