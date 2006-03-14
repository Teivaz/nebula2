//------------------------------------------------------------------------------
//  viewer.cc
//
//  A generic Mangalore viewer. The main task is to display a Nebula2 scene
//  with physics enabled.
//
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "viewer/viewerapp.h"
#include "tools/nwinmaincmdlineargs.h"
#include <windows.h>

//------------------------------------------------------------------------------
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    nWinMainCmdLineArgs args(lpCmdLine);

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
