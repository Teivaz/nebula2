#ifdef __WIN32__
    // System Includes
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nwin32loghandler.h"
#include "kernel/ndefaultloghandler.h"
#include "util/npathstring.h"
#include "BombsquadBruce/bbgame.h"
#include "BombsquadBruce/BBEngine.h"
#include "BombsquadBruce/general.h"

nNebulaUsePackage( nnebula );
nNebulaUsePackage( ndinput8 );
nNebulaUsePackage( ndirect3d9 );
nNebulaUsePackage( nlua );
nNebulaUsePackage( nmap );
nNebulaUsePackage( nspatialdb );
nNebulaUsePackage( ngui );
nNebulaUsePackage( ndsaudioserver3 );
nNebulaUsePackage( bombsquadBruce );

// Application entry method
#ifdef __WIN32__
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#else
int main( int argc, char * argv[] )
#endif
{
#ifdef __WIN32__
    char buffer[N_MAXPATH];
    GetModuleFileName( 0, buffer, sizeof(buffer) );
    nPathString exePath( buffer );
    exePath.ConvertBackslashes();
#else
    nPathString exePath( argv[0] );
#endif
    nPathString gameName = exePath.ExtractFileName();
    gameName.StripExtension();

    // Create the kernel and log servers, then let the game take over
    nKernelServer* kernelServer = new nKernelServer();
#ifdef __WIN32__
    nLogHandler* logHandler = new nWin32LogHandler( gameName.Get(), "CrazyChipmunk" );
#else
    nLogHandler* logHandler = new nDefaultLogHandler();
#endif
    kernelServer->SetLogHandler( logHandler );

    kernelServer->AddPackage( nnebula );
    kernelServer->AddPackage( ndinput8 );
    kernelServer->AddPackage( ndirect3d9 );
    kernelServer->AddPackage( nlua );
    kernelServer->AddPackage( nmap );
    kernelServer->AddPackage( nspatialdb );
    kernelServer->AddPackage( ngui );
    kernelServer->AddPackage( ndsaudioserver3 );
    kernelServer->AddPackage( bombsquadBruce );

    BBEngine* pEngine = static_cast<BBEngine*>( kernelServer->New( "bbengine", "/engine" ) );
    pEngine->StartEngine(); // runs until the game engine is stopped
                                     //(probably in script), then cleans up
    pEngine->Release();

    delete kernelServer;
    delete logHandler;

    return 0;
}
