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
#include "BombsquadBruce/ccgame.h"
#include "BombsquadBruce/CCEngine.h"
#include "BombsquadBruce/general.h"

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

	CCEngine* pEngine = static_cast<CCEngine*>( kernelServer->New( "ccengine", "/engine" ) );
	pEngine->StartEngine(); // runs until the game engine is stopped
                                     //(probably in script), then cleans up
	pEngine->Release();

	delete kernelServer;
	delete logHandler;

	return 0;
}
