//------------------------------------------------------------------------------
//  viewer.cc
//
//  A generic Mangalore viewer. The main task is to display a Nebula2 scene
//  with physics enabled.
//
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "viewer/viewerapp.h"
#ifdef __WIN32__
#include "tools/nwinmaincmdlineargs.h"
#include <windows.h>
#else
#include "tools/ncmdlineargs.h"
#endif

//------------------------------------------------------------------------------
#ifdef __WIN32__
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    nWinMainCmdLineArgs args(lpCmdLine);
#else
int main(int argc, const char **argv)
{
    nCmdLineArgs args(argc, argv);
#endif

    // initialize the Mangalore application
    Viewer::ViewerApp app;
	app.SetCmdLineArgs(args);
    if (app.Open())
    {
		app.Run();
		app.Close();
    }
    return 0;
}

