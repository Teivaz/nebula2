//------------------------------------------------------------------------------
//  nviewer.cc
//  Selfcontained viewer application for Nebula.
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "kernel/nremoteserver.h"
#include "kernel/nscriptserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nsceneserver.h"
#include "scene/nrendercontext.h"
#include "scene/ntransformnode.h"
#include "variable/nvariableserver.h"
#include "resource/nresourceserver.h"
#include "mathlib/matrix.h"
#include "tools/nwinmaincmdlineargs.h"
#include "misc/nconserver.h"
#include "anim2/nanimationserver.h"

#ifdef __WIN32__
#include "kernel/nwin32loghandler.h"
#endif

static float ViewerTheta = 0.0f;
static float ViewerRho   = 0.0f;
static vector3 ViewerPos(0.0f, 0.0f, 5.0f);

//------------------------------------------------------------------------------
/*
    Define a simple input mapping.
*/
void
DefineInputMapping(nInputServer* inputServer)
{
    n_assert(inputServer);

    inputServer->BeginMap();
    inputServer->Map("keyb0:space.down",       "reset");
    inputServer->Map("keyb0:esc.down",         "console");
    inputServer->Map("relmouse0:btn0.pressed", "pan");    
    inputServer->Map("relmouse0:btn1.pressed", "look");
    inputServer->Map("relmouse0:btn2.pressed", "zoom");
    inputServer->Map("relmouse0:-x",           "left");
    inputServer->Map("relmouse0:+x",           "right");
    inputServer->Map("relmouse0:-y",           "up");
    inputServer->Map("relmouse0:+y",           "down");
    inputServer->Map("keyb0:1.down", "script:/sys/servers/gfx.closedisplay;/sys/servers/gfx.setdisplaymode fullscreen 1024 768;/sys/servers/gfx.opendisplay");
    inputServer->Map("keyb0:2.down", "script:/sys/servers/gfx.closedisplay;/sys/servers/gfx.setdisplaymode windowed 640 480;/sys/servers/gfx.opendisplay");
    inputServer->EndMap();
}

//------------------------------------------------------------------------------
/*
    Get input and modify viewer matrix.
*/
void
HandleInput(nInputServer* inputServer, nConServer* consoleServer, float frameTime, matrix44& viewMatrix)
{
    n_assert(inputServer);
    if (frameTime <= 0.0001f)
    {
        frameTime = 0.0001f;
    }

    const float lookVelocity = 0.25f;
    const float panVelocity  = 0.75f;
    const float zoomVelocity = 0.75f;

    bool reset   = inputServer->GetButton("reset");
    bool console = inputServer->GetButton("console");

    float panHori = 0.0f;
    float panVert = 0.0f;
    float zoom    = 0.0f;
    float lookHori = 0.0f;
    float lookVert = 0.0f;

    if (inputServer->GetButton("look"))
    {
        lookHori = inputServer->GetSlider("left") - inputServer->GetSlider("right");
        lookVert = inputServer->GetSlider("down") - inputServer->GetSlider("up");
    }
    if (inputServer->GetButton("pan"))
    {
        panHori = inputServer->GetSlider("left") - inputServer->GetSlider("right");
        panVert = inputServer->GetSlider("down") - inputServer->GetSlider("up");
    }
    if (inputServer->GetButton("zoom"))
    {
        panHori = inputServer->GetSlider("left") - inputServer->GetSlider("right");
        zoom    = inputServer->GetSlider("down") - inputServer->GetSlider("up");
    }

    // toggle console
    if (console)
    {
        consoleServer->Toggle();
    }

    // handle viewer reset
    if (reset)
    {
        ViewerPos.set(0.0f, 0.0f, 5.0f);
        ViewerTheta = 0.0f;
        ViewerRho   = 0.0f;
    }

    // handle viewer rotation
    ViewerTheta -= lookVert * lookVelocity;
    ViewerRho   += lookHori * lookVelocity;

    vector3 horiMoveVector(viewMatrix.x_component() * panHori * panVelocity);
    vector3 vertMoveVector(viewMatrix.y_component() * panVert * panVelocity);
    ViewerPos += horiMoveVector + vertMoveVector;

    vector3 zoomMoveVector(-viewMatrix.z_component() * zoom * zoomVelocity);
    ViewerPos += zoomMoveVector;

    viewMatrix.ident();
    viewMatrix.rotate_x(ViewerTheta);
    viewMatrix.rotate_y(ViewerRho);
    viewMatrix.translate(ViewerPos);
}

//------------------------------------------------------------------------------
/*
    Transfer the current global variables to the render context.
*/
void
TransferGlobalVariables(nVariableServer* varServer, nRenderContext& renderContext)
{
    n_assert(varServer);
    const nVariableContext& globalContext = varServer->GetGlobalVariableContext();
    int numGlobalVars = globalContext.GetNumVariables();
    int globalVarIndex;
    for (globalVarIndex = 0; globalVarIndex < numGlobalVars; globalVarIndex++)
    {
        const nVariable& globalVar = globalContext.GetVariableAt(globalVarIndex);
        nVariable* var = renderContext.GetVariable(globalVar.GetHandle());
        if (var)
        {
            *var = globalVar;
        }
        else
        {
            nVariable newVar(globalVar);
            renderContext.AddVariable(newVar);
        }
    }
}

//------------------------------------------------------------------------------
/*
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

    const char* scriptArg     = args.GetStringArg("-script", 0);
    const char* viewArg       = args.GetStringArg("-view", 0);
    bool fullscreenArg        = args.GetBoolArg("-fullscreen");
    int widthArg              = args.GetIntArg("-w", 640);
    int heightArg             = args.GetIntArg("-h", 480);

    // initialize Nebula runtime
    nKernelServer* kernelServer = new nKernelServer;
    
    // initialize a custom log handler
    #ifdef __WIN32__
        nWin32LogHandler logHandler("nviewer");
        kernelServer->SetLogHandler(&logHandler);
    #endif

    nTimeServer* timeServer     = kernelServer->GetTimeServer();
    nRemoteServer* remoteServer = kernelServer->GetRemoteServer();
    nScriptServer* scriptServer = (nScriptServer*) kernelServer->New("ntclserver", "/sys/servers/script");

    nGfxServer2* gfxServer          = 0;
    nInputServer* inputServer       = 0;
    nSceneServer* sceneServer       = 0;
    nVariableServer* varServer      = 0;
    nResourceServer* resourceServer = 0;
    nConServer* consoleServer       = 0;
    nAnimationServer* animServer    = 0;

    gfxServer       = (nGfxServer2*)      kernelServer->New("nd3d9server",     "/sys/servers/gfx");
    inputServer     = (nInputServer*)     kernelServer->New("ndi8server",      "/sys/servers/input");
    consoleServer   = (nConServer*)       kernelServer->New("nconserver",      "/sys/servers/console");
    resourceServer  = (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
    sceneServer     = (nSceneServer*)     kernelServer->New("nmrtsceneserver", "/sys/servers/scene");
    varServer       = (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
    animServer      = (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
    n_assert(gfxServer);
    n_assert(inputServer);
    n_assert(sceneServer);
    n_assert(varServer);
    n_assert(resourceServer);
    n_assert(animServer);

    // load the default Nebula startup.tcl script
    const char* result;
    scriptServer->RunScript("home:bin/startup.tcl", result);

    // define a simple input mapping
    DefineInputMapping(inputServer);

    // create scene graph root node
    nTransformNode*  rootNode = (nTransformNode*) kernelServer->New("ntransformnode",  "/usr/scene");

    // open the remote port
    remoteServer->Open("nviewer");

    // initialize graphics
    nDisplayMode2 displayMode(fullscreenArg ? nDisplayMode2::FULLSCREEN : nDisplayMode2::WINDOWED, widthArg, heightArg);
    gfxServer->SetDisplayMode(displayMode);
    nString title;
    if (viewArg)
    {
        title.Append(viewArg);
        title.Append(" - ");
    }
    title.Append("Nebula2 viewer");
    gfxServer->SetWindowTitle(title.Get());
    gfxServer->OpenDisplay();

    // run scripts
    if (scriptArg)
    {
        const char* result;
        scriptServer->RunScript(scriptArg, result);
    }
    if (viewArg)
    {
        // load the standard lighting
        const char* result;
        scriptServer->RunScript("home:bin/stdlight.tcl", result);

        // load the object to look at
        kernelServer->PushCwd(rootNode);
        kernelServer->Load(viewArg);
        kernelServer->PopCwd();
    }

    // initialize a render context
    nRenderContext renderContext;
    nVariable::Handle timeHandle = varServer->GetVariableHandleByName("chnTime");
    nVariable::Handle oneHandle  = varServer->GetVariableHandleByName("chnOne");
    renderContext.AddVariable(nVariable(timeHandle, 0.5f));
    renderContext.AddVariable(nVariable(oneHandle, 0.5f));
    rootNode->RenderContextCreated(&renderContext);

    // run the render loop
    matrix44 viewMatrix;
    bool running = true;
    double prevTime = 0.0;
    uint frameId = 0;
    while (gfxServer->Trigger() && running)
    {
        double time = timeServer->GetTime();
        if (prevTime == 0.0)
        {
            prevTime = time;
        }
        float frameTime = (float) (time - prevTime);

        // trigger remote server
        remoteServer->Trigger();

        // trigger script server
        running = scriptServer->Trigger();

        // handle input
        inputServer->Trigger(time);
        HandleInput(inputServer, consoleServer, frameTime, viewMatrix);
        inputServer->FlushEvents();

        // update render context variables
        renderContext.GetVariable(timeHandle)->SetFloat((float)time);
        TransferGlobalVariables(varServer, renderContext);
        renderContext.SetFrameId(frameId++);

        // render
        sceneServer->BeginScene(viewMatrix);
        sceneServer->Attach(rootNode, &renderContext);
        sceneServer->EndScene();
        consoleServer->Render();
        sceneServer->RenderScene();

        prevTime = time;
    }

    // shutdown
    gfxServer->CloseDisplay();

    rootNode->Release();
    animServer->Release();
    sceneServer->Release();
    inputServer->Release();
    gfxServer->Release();
    scriptServer->Release();
    varServer->Release();
    resourceServer->Release();

    delete kernelServer;
    return 0;
}

