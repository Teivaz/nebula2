//------------------------------------------------------------------------------
//  napplication_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/napplication.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "kernel/nscriptserver.h"
#include "gfx2/ngfxserver2.h"
#include "input/ninputserver.h"
#include "misc/nconserver.h"
#include "resource/nresourceserver.h"
#include "scene/nsceneserver.h"
#include "variable/nvariableserver.h"
#include "anim2/nanimationserver.h"
#include "particle/nparticleserver.h"
#include "video/nvideoserver.h"
#include "gui/nguiserver.h"
#include "shadow/nshadowserver.h"
#include "kernel/nremoteserver.h"
#include "audio3/naudioserver3.h"
#include "misc/nprefserver.h"
#include "application/nappcamera.h"
#include "application/nappstate.h"
#include "gui/nguiwindow.h"
#include "locale/nlocaleserver.h"

nNebulaScriptClass(nApplication, "nroot");

//------------------------------------------------------------------------------
/**
*/
nApplication::nApplication() :
    isOpen(false),
    quitRequested(false),
    companyName("Radon Labs GmbH"),
    appName("Generic"),
    startupScript("proj:data/scripts/startup.tcl"),
    stateTransitionTime(0.0),
    time(0.0),
    stateTime(0.0),
    frameTime(0.0),
    appCamera(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nApplication::~nApplication()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
    Open the application. This will initialize the Nebula2 runtime, load
    the startup script, and invoke the script functions OnStartup,
    OnGraphicsStartup, OnGuiStartup and OnMapInput.
*/
bool
nApplication::Open()
{
    n_assert(!this->isOpen);
    n_assert(0 == this->appCamera);

    this->quitRequested = false;

    // initialize Nebula2 runtime
    this->refScriptServer    = this->CreateScriptServer();
    this->refGfxServer       = this->CreateGfxServer();
    this->refInputServer     = this->CreateInputServer();
    this->refConServer       = this->CreateConsoleServer();
    this->refResourceServer  = this->CreateResourceServer();
    this->refSceneServer     = this->CreateSceneServer();
    this->refVariableServer  = this->CreateVariableServer();
    this->refAnimationServer = this->CreateAnimationServer();
    this->refParticleServer  = this->CreateParticleServer();
    this->refVideoServer     = this->CreateVideoServer();
    this->refGuiServer       = this->CreateGuiServer();
    this->refShadowServer    = this->CreateShadowServer();
    this->refAudioServer     = this->CreateAudioServer();
    this->refPrefServer      = this->CreatePrefServer();
    this->refLocaleServer    = this->CreateLocaleServer();

    // initialize preferences server
    this->refPrefServer->SetCompanyName(this->GetCompanyName());
    this->refPrefServer->SetApplicationName(this->GetAppName());

    // create state object
    kernelServer->PushCwd(this);
    this->refAppStates = kernelServer->New("nroot", "appstates");
    kernelServer->PopCwd();

    // initialize the proj: assign
    nFileServer2* fileServer = nFileServer2::Instance();
    if (!this->projDir.IsEmpty())
    {
        fileServer->SetAssign("proj", this->GetProjectDirectory().Get());
    }
    else
    {
        fileServer->SetAssign("proj", kernelServer->GetFileServer()->GetAssign("home"));
    }

    // open the remote port
    kernelServer->GetRemoteServer()->Open(this->GetAppName().Get());

    // run startup script (assigns must be setup before opening the display!)
    const char* scriptResult;
    this->refScriptServer->RunScript(this->startupScript.Get(), scriptResult);
    this->refScriptServer->Run("OnStartup", scriptResult);

    // initialize locale server
    this->refLocaleServer->SetLocaleTableFilename("data:tables/locale.xml");
    this->refLocaleServer->Open();

    // make sure the read/write appdata directories exists
    fileServer->MakePath("appdata:");
    fileServer->MakePath("save:");

    // initialize graphics
    this->refGfxServer->SetDisplayMode(this->displayMode);
    this->refGfxServer->SetCamera(this->gfxCamera);
    this->refGfxServer->OpenDisplay();
    this->refScriptServer->Run("OnGraphicsStartup", scriptResult);
    this->refVideoServer->Open();

    // initialize audio
    if (!this->refAudioServer->Open())
    {
        this->Close();
        return false;
    }

    // open the scene server
    this->refSceneServer->SetBgColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    if (!this->refSceneServer->Open())
    {
        this->Close();
        return false;
    }

    // initialize gui
    this->refGuiServer->SetRootPath("/gui");
    this->refGuiServer->SetDisplaySize(vector2(float(this->displayMode.GetWidth()), float(this->displayMode.GetHeight())));
    this->refGuiServer->Open();

    // create a user root window
    this->refGuiServer->SetRootWindowPointer(0);
    nGuiWindow* userRootWindow = this->refGuiServer->NewWindow("nguiwindow", true);
    this->refGuiServer->SetRootWindowPointer(userRootWindow);

    // initialize input mapping
    this->refScriptServer->Run("OnMapInput", scriptResult);

    // initialize camera
    this->appCamera = this->CreateAppCamera();

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the application.
*/
void
nApplication::Close()
{
    if (this->appCamera)
    {
        n_delete(this->appCamera);
        this->appCamera = 0;
    }

    if (this->refAppStates.isvalid())
    {
        this->refAppStates->Release();
    }

    this->refLocaleServer->Release();
    this->refPrefServer->Release();
    this->refAudioServer->Release();
    this->refShadowServer->Release();
    this->refGuiServer->Release();
    this->refVideoServer->Release();
    this->refParticleServer->Release();
    this->refAnimationServer->Release();
    this->refVariableServer->Release();
    this->refSceneServer->Release();
    this->refConServer->Release();
    this->refInputServer->Release();
    this->refGfxServer->Release();
    this->refResourceServer->Release();
    this->refScriptServer->Release();

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Run the application, this method will return when the application
    should quit.
*/
void
nApplication::Run()
{
    while (!this->GetQuitRequested())
    {
        // update time values
        this->OnUpdateTime();

        // perform state transition
        if ((!this->curState.isvalid()) ||
            (this->nextState.isvalid() && (this->nextState.get() != this->curState.get())))
        {
            this->DoStateTransition();
        }

        // perform current frame
        this->DoFrame();

        // give up time slice
        n_sleep(0.0f);
    }
}

//------------------------------------------------------------------------------
/**
    Update time values.
*/
void
nApplication::OnUpdateTime()
{
    nTimeServer* timeServer = nTimeServer::Instance();
    timeServer->Trigger();
    nTime curTime = timeServer->GetTime();
    nTime diff    = curTime - this->time;

    // catch time exceptions
    if (diff <= 0.0)
    {
        diff = 0.0001f;
        curTime = this->time = diff;
    }
    this->frameTime = diff;
    this->time = curTime;
    this->stateTime = this->time - this->stateTransitionTime;
}

//------------------------------------------------------------------------------
/**
    Perform a state transition.
*/
void
nApplication::DoStateTransition()
{
    n_assert(this->nextState.isvalid());

    if (this->curState.isvalid())
    {
        n_assert(this->curState.get() != this->nextState.get());
        this->curState->OnStateLeave(this->nextState->GetName());
        this->nextState->OnStateEnter(this->curState->GetName());
    }
    else
    {
        this->nextState->OnStateEnter("");
    }
    this->curState = this->nextState;
    this->stateTransitionTime = this->time;
}

//------------------------------------------------------------------------------
/**
    Set a new application state. The state will be activated during the
    next DoStateTransition().
*/
void
nApplication::SetState(const nString& stateName)
{
    n_assert(!stateName.IsEmpty());
    nAppState* appState = this->FindState(stateName);
    if (0 != appState)
    {
        if (this->curState.isvalid())
        {
            if (appState != this->curState)
            {
                this->nextState = appState;
            }
        }
        else
        {
            this->nextState = appState;
        }
    }
    else
    {
        n_printf("nApplication::SetState(): invalid app state '%s'!\n", stateName.Get());
    }
}

//------------------------------------------------------------------------------
/**
    Returns the currently active application state.
*/
nString
nApplication::GetCurrentState() const
{
    nString stateName;
    if (this->curState.isvalid())
    {
        stateName = this->curState->GetName();
    }
    return stateName;
}

//------------------------------------------------------------------------------
/**
    Find an application state by name.
*/
nAppState*
nApplication::FindState(const nString& stateName) const
{
    n_assert(!stateName.IsEmpty());
    return (nAppState*) this->refAppStates->Find(stateName.Get());
}

//------------------------------------------------------------------------------
/**
    Create a new application state object of the given class and name,
    and link it into the app state pool.
*/
nAppState*
nApplication::CreateState(const nString& className, const nString& stateName)
{
    n_assert(!className.IsEmpty());
    n_assert(!stateName.IsEmpty());
    n_assert(!this->refAppStates->Find(stateName.Get()));
    kernelServer->PushCwd(this->refAppStates);
    nAppState* newState = (nAppState*) kernelServer->New(className.Get(), stateName.Get());
    kernelServer->PopCwd();
    newState->OnCreate(this);
    return newState;
}

//------------------------------------------------------------------------------
/**
    Do one complete frame.
*/
void
nApplication::DoFrame()
{
    n_assert(this->appCamera);
    n_assert(this->curState.isvalid());

    // distribute time to subsystems
    nGuiServer::Instance()->SetTime(this->time);

    // trigger the various servers
    if (!this->refScriptServer->Trigger()) 
    {
        this->SetQuitRequested(true);
    }
    if (!this->refGfxServer->Trigger()) 
    {
        this->SetQuitRequested(true);
    }
    kernelServer->Trigger();
    nConServer::Instance()->Trigger();
    nInputServer::Instance()->Trigger(this->time);
    nGuiServer::Instance()->Trigger();
    nVideoServer::Instance()->Trigger();

    // perform logic triggering
    this->curState->OnFrame();    

    // perform rendering
    nAudioServer3* audioServer = nAudioServer3::Instance();
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nSceneServer* sceneServer = nSceneServer::Instance();

    audioServer->BeginScene(this->time);        
    audioServer->UpdateListener(this->audioListener);
    if (!gfxServer->InDialogBoxMode())
    {
        const matrix44& viewMatrix = this->appCamera->GetViewMatrix();
        if (sceneServer->BeginScene(viewMatrix))
        {
            nParticleServer::Instance()->Trigger();
            this->OnRender3D();
            sceneServer->RenderScene();
            sceneServer->EndScene();
            this->OnRender2D();
            nGuiServer::Instance()->Render();
            nConServer::Instance()->Render();
            sceneServer->PresentScene();
        }
    }
    audioServer->EndScene();
    nInputServer::Instance()->FlushEvents();
}

//------------------------------------------------------------------------------
/**
    Perform 3d rendering. This calls the OnRender3d() method of the current
    app state object. Usually, all that needs to be done is to attach
    Nebula2 scene graph nodes to the scene server.
*/
void
nApplication::OnRender3D()
{
    n_assert(this->curState.isvalid());
    this->curState->OnRender3D();
}

//------------------------------------------------------------------------------
/**
    Perform 2d rendering. This calls the OnRender2d() method of the current
    app state object.
*/
void
nApplication::OnRender2D()
{
    n_assert(this->curState.isvalid());
    this->curState->OnRender2D();
}

//------------------------------------------------------------------------------
/**
*/
nScriptServer*
nApplication::CreateScriptServer()
{
    return (nScriptServer*) kernelServer->New("ntclserver", "/sys/servers/script");
}

//------------------------------------------------------------------------------
/**
*/
nGfxServer2*
nApplication::CreateGfxServer()
{
    return (nGfxServer2*) kernelServer->New("nd3d9server", "/sys/servers/gfx");
}

//------------------------------------------------------------------------------
/**
*/
nInputServer*
nApplication::CreateInputServer()
{
    return (nInputServer*) kernelServer->New("ndi8server", "/sys/servers/input");
}

//------------------------------------------------------------------------------
/**
*/
nConServer*
nApplication::CreateConsoleServer()
{
    return (nConServer*) kernelServer->New("nconserver", "/sys/servers/console");
}

//------------------------------------------------------------------------------
/**
*/
nResourceServer*
nApplication::CreateResourceServer()
{
    return (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
}

//------------------------------------------------------------------------------
/**
*/
nSceneServer*
nApplication::CreateSceneServer()
{
    return (nSceneServer*) kernelServer->New("nmrtsceneserver", "/sys/servers/scene");
}

//------------------------------------------------------------------------------
/**
*/
nVariableServer*
nApplication::CreateVariableServer()
{
    return (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
}

//------------------------------------------------------------------------------
/**
*/
nAnimationServer*
nApplication::CreateAnimationServer()
{
    return (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
}

//------------------------------------------------------------------------------
/**
*/
nParticleServer*
nApplication::CreateParticleServer()
{
    return (nParticleServer*) kernelServer->New("nparticleserver", "/sys/servers/particle");
}

//------------------------------------------------------------------------------
/**
*/
nVideoServer*
nApplication::CreateVideoServer()
{
    return (nVideoServer*) kernelServer->New("ndshowserver", "/sys/servers/video");
}

//------------------------------------------------------------------------------
/**
*/
nGuiServer*
nApplication::CreateGuiServer()
{
    return (nGuiServer*) kernelServer->New("nguiserver", "/sys/servers/gui");
}

//------------------------------------------------------------------------------
/**
*/
nShadowServer*
nApplication::CreateShadowServer()
{
    return (nShadowServer*) kernelServer->New("nshadowserver", "/sys/servers/shadow");
}

//------------------------------------------------------------------------------
/**
*/
nAppCamera*
nApplication::CreateAppCamera()
{
    return new nAppCamera;
}

//------------------------------------------------------------------------------
/**
*/
nAudioServer3*
nApplication::CreateAudioServer()
{
    return (nAudioServer3*) kernelServer->New("ndsoundserver3", "/sys/servers/audio");
}

//------------------------------------------------------------------------------
/**
*/
nPrefServer*
nApplication::CreatePrefServer()
{
    return (nPrefServer*) kernelServer->New("nprefserver", "/sys/servers/nwin32prefserver");
}

//------------------------------------------------------------------------------
/**
*/
nLocaleServer*
nApplication::CreateLocaleServer()
{
    return (nLocaleServer*) kernelServer->New("nlocaleserver", "/sys/servers/locale");
}
