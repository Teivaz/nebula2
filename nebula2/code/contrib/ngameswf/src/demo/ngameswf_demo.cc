//-----------------------------------------------------------------------------
// ngameswf_demo.cc
//
// A simple demonstration application for ngameswf.
//-----------------------------------------------------------------------------

#include "kernel/nref.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "gfx2/ngfxserver2.h"
#include "input/ninputserver.h"
#include "variable/nvariableserver.h"
#include "resource/nresourceserver.h"
#include "misc/nconserver.h"
#include "tools/nwinmaincmdlineargs.h"
#include "kernel/nwin32loghandler.h"
#include "kernel/ntimeserver.h"
#include "kernel/nfile.h"
#include "kernel/nfileserver2.h"
#include "scene/nsceneserver.h"
#include "scene/nstdsceneserver.h"
#include "mathlib/matrix.h"

#include "ngameswf/ngameswfserver.h"
#include "ngameswf/ngameswfmovie.h"

// global variables
const char* scriptserverArg;
const char* sceneserverArg;
const char* startupArg;
int widthArg;
int heightArg;

nKernelServer*        kernelServer;
nTimeServer*          timeServer;
nRef<nScriptServer>   scriptServer;
nRef<nGfxServer2>     gfxServer;
nRef<nInputServer>    inputServer;
nRef<nVariableServer> variableServer;
nRef<nResourceServer> resourceServer;
nRef<nConServer>      consoleServer;
nRef<nSceneServer>    sceneServer;
nRef<nGameSwfServer>  gameswfServer;
nRef<nGameSwfMovie>   gameswfMovie;
nRef<nGameSwfMovie>   gameswfMovie2;

double prevTime = 0.0;
matrix44 viewMatrix;

static float rotation   = 0.0f;
static bool rotateLeft  = false;
static bool rotateRight = false;

nMesh2		  *torusMesh    = NULL;
nShader2      *torusShader  = NULL;
nTexture2     *torusTexture = NULL;

// forward declarations.
bool open ();
bool initSWF ();
bool run ();
void close ();
void handleInput();

//-----------------------------------------------------------------------------
/**
    handle 'fscommand' from swf movie.
*/
static
void FSCallback (gameswf::movie_interface* movie, 
                 const char* command, const char* args)
{
    // hide background of movie or not.
	if(stricmp(command, "hidebg") == 0)
	{
		if(stricmp(args, "true") == 0)
			gameswfMovie->SetBackgroundAlpha(0.0f);
		else
			gameswfMovie->SetBackgroundAlpha(1.0f);
	}

    // turn on/off rotation flag of 'torus' mesh object.
	if(stricmp(command, "startrotate") == 0)
	{
		if(stricmp(args, "left") == 0)
			rotateLeft = true;
		else 
			rotateRight = true;
	}
	else
    if(stricmp(command, "stoprotate") == 0)
	{
		if(stricmp(args, "left") == 0)
			rotateLeft = false;
		else
			rotateRight = false;
	}
}

void TransferGlobalVariables();

//-----------------------------------------------------------------------------
/**
    Application entry point.
*/
#ifdef __WIN32__
int WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
         LPSTR lpCmdLine, int nShowCmd)
{
    nWinMainCmdLineArgs args(lpCmdLine);
#else
int
main (int argc, const char* args)
{
    nCmdLineArgs args(argc, argv);
#endif

    // Get command-line arguments.
    scriptserverArg = args.GetStringArg ("-scriptserver", "ntclserver");
    sceneserverArg  = args.GetStringArg ("-sceneserver", "nstdsceneserver");
    startupArg      = args.GetStringArg ("-startup", "home:bin/startup.tcl");
    widthArg        = args.GetIntArg    ("-width", 800);
    heightArg       = args.GetIntArg    ("-height", 600);

    nWin32LogHandler logHandler("ngameswfdemo");

    kernelServer = new nKernelServer;
    kernelServer->SetLogHandler (&logHandler);

    // create servers and open display.
    open();

    // execute app.
    bool running = true;
    while (running)
        running = run();

    // close servers.
    close();

    return 0;
}

//-----------------------------------------------------------------------------
/**
    create servers and open display device.
*/
bool open ()
{
    timeServer     = kernelServer->GetTimeServer ();
    scriptServer   = (nScriptServer*)kernelServer->New (scriptserverArg, "/sys/servers/script");
    gfxServer      = (nGfxServer2*)kernelServer->New ("nd3d9server", "/sys/servers/gfx");
    inputServer    = (nInputServer*)kernelServer->New ("ndi8server", "/sys/servers/input");
    variableServer = (nVariableServer*)kernelServer->New ("nvariableserver", "/sys/servers/variable");
    resourceServer = (nResourceServer*)kernelServer->New ("nresourceserver", "/sys/servers/resource");
    consoleServer  = (nConServer*)kernelServer->New("nconserver", "/sys/servers/console");

    n_assert (scriptServer.get() != NULL);
    n_assert (gfxServer.get() != NULL);
    n_assert (inputServer.get() != NULL);
    n_assert (variableServer.get() != NULL);
    n_assert (resourceServer.get() != NULL);
    
    const char* result;
    scriptServer->RunScript (startupArg, result);

    inputServer->BeginMap();
    inputServer->Map("keyb0:esc.down", "console");
    inputServer->EndMap();

    // specify display mode.
    bool fullscreen = false;
    nDisplayMode2 displayMode ("ngameswf demo", 
                               fullscreen ? nDisplayMode2::Fullscreen : nDisplayMode2::Windowed,
                               0, 0, widthArg, heightArg, true);
    gfxServer->SetDisplayMode (displayMode);

    gfxServer->OpenDisplay ();

    if (!initSWF())
    {
        n_printf ("open: faild initSWF. \n");
        return false;
    }

    // create 'torus' mesh object.
	torusMesh = gfxServer->NewMesh("torus_mesh");
	torusMesh->SetFilename("meshes:examples/torus.n3d2");
	torusMesh->Load();

	torusShader = gfxServer->NewShader("torus_shader");
	torusShader->SetFilename("gswf:gswf_demotorus.fx");
	torusShader->Load();

	torusTexture = gfxServer->NewTexture("torus_diff");
	torusTexture->SetFilename("textures:examples/brick.bmp");
	torusTexture->Load();

    return true;
}

//-----------------------------------------------------------------------------
/**
    initialize ngameswf stuff.
      - create ngameswf server
      - create ngameswf movie and load .swf file
*/
bool initSWF ()
{
    // create nGameSwfServer
    gameswfServer = (nGameSwfServer*)kernelServer->New ("ngameswfserver", "/sys/servers/gameswf");
    n_assert (gameswfServer.get ());

    // set external callback function.
    gameswfServer->SetFSCommandCallback(FSCallback);

    // create gameswf movie
    gameswfMovie = (nGameSwfMovie*)kernelServer->New ("ngameswfmovie", "/usr/swfs/movie");
    if (!gameswfMovie.get())
    {
        n_printf ("initSWF:Failed to create gameswf movie.\n");
        return false;
    }

    char moviefilename [N_MAXPATH];
    nFileServer2* fileServer = kernelServer->GetFileServer ();
    fileServer->ManglePath ("home:export/swfs/ngameswf.swf", moviefilename, sizeof(moviefilename));

    if (!gameswfMovie->CreateMovie (moviefilename))
        n_printf ("initSWF: Cannot load %s file.\n", moviefilename);
    gameswfMovie->SetBackgroundAlpha (1.0f);

    int w = gfxServer->displayMode.GetWidth ();
    int h = gfxServer->displayMode.GetHeight ();
    gameswfMovie->SetDisplayViewport(0, 0, w, h);

    return true;
}

//-----------------------------------------------------------------------------
/**
    render scene and handle input.
*/
bool run ()
{
    if (!gfxServer->Trigger())
        return false;

    kernelServer->Trigger ();
    timeServer->Trigger();

    double time = timeServer->GetTime();
    if (prevTime < 0.000001f)
    {
        prevTime = 0.0;
        prevTime = time;
    }
    float frameTime = (float)(time - prevTime);

    if (!scriptServer->Trigger ())
        return false;

    // process input
    inputServer->Trigger (time);
    handleInput();

	matrix44 modelViewMatrix;
	modelViewMatrix.translate(vector3(0.0f, 0.0f, 10.0f));
	modelViewMatrix.rotate_x(n_deg2rad(90.0f));
	modelViewMatrix.rotate_z(n_deg2rad(rotation));
    modelViewMatrix.invert_simple();

    gfxServer->SetTransform(nGfxServer2::View, modelViewMatrix);

    if (gfxServer->BeginScene())
    {
        gfxServer->Clear(nGfxServer2::AllBuffers, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0);

        // render torus mesh object.
		gfxServer->SetMesh(0, torusMesh);
		gfxServer->SetVertexRange(0, torusMesh->GetNumVertices());
		gfxServer->SetIndexRange(0, torusMesh->GetNumIndices());
		torusShader->SetTexture(nShader2::Parameter::DiffMap0, torusTexture);
		torusShader->SetMatrix(nShader2::Parameter::ModelViewProjection, 
                               gfxServer->GetTransform(nGfxServer2::ModelViewProjection));
		gfxServer->SetShader(torusShader);
		gfxServer->DrawIndexed(TriangleList);

        // render swf movie.
        gameswfMovie->Render (); 

        gfxServer->DrawTextBuffer();
        gfxServer->EndScene();

        consoleServer->Render ();

        gfxServer->PresentScene();
    }

    prevTime = time;

    return true;
}

//-----------------------------------------------------------------------------
/**
    close created servers.
*/
void close ()
{
    gfxServer->CloseDisplay();

	torusMesh->Release();
	torusShader->Release();
	torusTexture->Release();

    gameswfMovie->Release();
    gameswfServer->Release ();

    consoleServer->Release();
    resourceServer->Release();
    variableServer->Release();
    inputServer->Release();
    gfxServer->Release();
    scriptServer->Release();

    delete kernelServer;
}

//-----------------------------------------------------------------------------
/**
    handle input event.
*/
void handleInput()
{
    bool console = inputServer->GetButton("console");
    if (console)
    {
        consoleServer->Toggle();
    }

	nInputEvent* inputEvent = inputServer->FirstEvent();

	if(inputEvent)
	{
		nInputEvent* nextInputEvent;

		do
		{
			nextInputEvent = inputServer->NextEvent(inputEvent);

			if(inputEvent->GetType() == N_INPUT_MOUSE_MOVE)
			{
				gameswfMovie->OnMouseMove(inputEvent->GetAbsXPos(), 
                                          inputEvent->GetAbsYPos());
			}
			else 
            if(inputEvent->GetType() == N_INPUT_BUTTON_DOWN && 
               inputEvent->GetDeviceId() == N_INPUT_MOUSE(0))
			{
				gameswfMovie->OnMouseDown();
			}
			else 
            if(inputEvent->GetType() == N_INPUT_BUTTON_UP && 
               inputEvent->GetDeviceId() == N_INPUT_MOUSE(0))
			{
				gameswfMovie->OnMouseUp();
			}
		}
		while ((inputEvent = nextInputEvent));
	} 

    inputServer->FlushEvents(); 

	if(rotateLeft)
		rotation += 2.5f;
	
	if(rotateRight)
		rotation -= 2.5f;
}

