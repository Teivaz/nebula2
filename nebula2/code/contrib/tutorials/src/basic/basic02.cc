//------------------------------------------------------------------------------
/**
    @file
    @ingroup Nebula2TutorialsBasic
    
    @brief Basic Example 02 - Opening a graphics window.
    
    This example opens a graphics window and clears it to a particular colour.
    In order to open a window we need to do quite a lot more than the previous
    example. 
*/

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "resource/nresourceserver.h"
#include "gfx2/ngfxserver2.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndirect3d9);

main(int argc, const char** argv)
{
    nKernelServer* kernelServer = new nKernelServer();

    // all packages have to be declared and added before you use them
    kernelServer->AddPackage(nnebula);
    kernelServer->AddPackage(ndirect3d9);
    
    nRef<nResourceServer> refResourceServer;
    nRef<nGfxServer2> refGfxServer;
    refResourceServer = (nResourceServer*)kernelServer->New("nresourceserver", "/sys/servers/resource");
    refGfxServer = (nGfxServer2*)kernelServer->New("nd3d9server", "/sys/servers/gfx");

    // set shaders assign, needed for the gfx server,
    // this means that "shaders:" can be used to refer to the given directory.
    kernelServer->GetFileServer()->SetAssign("shaders", "home:data/shaders/fixed");
    
    nDisplayMode2 DisplayMode(
        "Nebula2: Basic Example 02", 
        nDisplayMode2::Windowed, 
        0, 0, 
        640, 480, 
        false);
    refGfxServer->SetDisplayMode(DisplayMode);
    
    bool result;
    result = refGfxServer->OpenDisplay();
    n_assert(result);

    // to avoid dereferencing the nRef all the time
    // get a pointer to the gfx server. 
    nGfxServer2* gfxServer = refGfxServer.get();

    // trigger the gfx server once every frame
    // Trigger() returns false if you close the window
    while (gfxServer->Trigger())
    {
        gfxServer->BeginScene();
        gfxServer->Clear(nGfxServer2::AllBuffers, 0.0f, 0.8f, 0.6f, 0.4f, 0.0f, 0);

        gfxServer->EndScene();
        gfxServer->PresentScene();
        
        // allow Windows to multitask
        n_sleep(0.0);
    }

    refGfxServer->CloseDisplay();

    refGfxServer->Release();
    refResourceServer->Release();

    delete kernelServer;
    return;
}