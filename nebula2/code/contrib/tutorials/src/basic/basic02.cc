//------------------------------------------------------------------------------
/**
    @file
    @ingroup Nebula2TutorialsBasic

    @brief Basic Example 02 - Opening a graphics window.

    This example opens a graphics window and clears it to a particular
    colour. In order to open a window we need to do quite a lot more
    than the previous example.

    The main class we need to deal with is nGfxServer2, this is the base
    class for all the graphics implementations. Unlike the kernel server
    all other servers have to be created through the kernel server, this
    is true for any nRoot derived class and servers are always derived
    from nRoot. In order to be able to create a server, or other nRoot
    derived object, we have to first ensure that it has been registered
    with the kernel server; this is done by adding packages. The base
    gfx classes are contained in the nnebula package and the D3D9 based
    graphics server is in the ndirect3d9 package. See @ref
    Nebula2Packages to see the other packages. There are two steps to
    adding a package, the first is to declare which packages we are
    using, and then after creating the kernel server we have to add the
    packages.

    The graphics server can now be created via nKernelServer::New(). It
    is also necessary to add a resource server (nResourceServer) as the
    graphics server expects one to be available.
*/

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "resource/nresourceserver.h"
#include "gfx2/ngfxserver2.h"

// declare packages
nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndirect3d9);

int
main(int /*argc*/, const char** /*argv*/)
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
        false, false, "icon");
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
        gfxServer->BeginFrame();
        gfxServer->BeginScene();
        gfxServer->Clear(nGfxServer2::AllBuffers, 0.0f, 0.8f, 0.6f, 0.4f, 0.0f, 0);

        gfxServer->EndScene();
        gfxServer->PresentScene();
        gfxServer->EndFrame();

        // allow Windows to multitask
        n_sleep(0.0);
    }

    refGfxServer->CloseDisplay();

    refGfxServer->Release();
    refResourceServer->Release();

    delete kernelServer;
    return 0;
}
