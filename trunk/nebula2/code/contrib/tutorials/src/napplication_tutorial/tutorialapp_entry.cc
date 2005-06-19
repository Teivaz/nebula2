#include "kernel/nkernelserver.h"
#include "gfx2/ndisplaymode2.h"
#include "napplication_tutorial/ntutorialapp.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndshow);
nNebulaUsePackage(ngui);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndsaudioserver3);
nNebulaUsePackage(napplication);
nNebulaUsePackage(tutorialapp_exe);

//----------------------------------------------------------------------------
void main()
{
    // Kernel Server preparation:
    nKernelServer kernelServer;

    kernelServer.AddPackage(nnebula);
    kernelServer.AddPackage(ndirect3d9);
    kernelServer.AddPackage(ndshow);
    kernelServer.AddPackage(ngui);
    kernelServer.AddPackage(ndinput8);
    kernelServer.AddPackage(ndsaudioserver3);
    kernelServer.AddPackage(napplication);
    kernelServer.AddPackage(tutorialapp_exe);
    
    n_printf("Go back to sleep, bruce.\n");
    
    nTutorialApp* theApp = static_cast<nTutorialApp*>(kernelServer.New("ntutorialapp", "theApp"));
    nDisplayMode2 disp("HelloWorld!", nDisplayMode2::Windowed, 100, 100, 600, 400, false);
    theApp->SetDisplayMode(disp);
    // For this tutorial we want our project directory to be the nebula2 directory so 
    // we don't set one here since the default project directory is the nebula2 directory.
    //theApp->SetProjectDirectory("path_to_your_project");
    theApp->SetStartupScript("data:scripts/startup.tcl");
    
    if (theApp->Open())
    {
        theApp->Run();
    }
    
    theApp->Close();
    theApp->Release();
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
