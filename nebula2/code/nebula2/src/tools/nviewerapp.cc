//------------------------------------------------------------------------------
//  nviewerapp.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nviewerapp.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "misc/nwatched.h"
#include "gui/nguiwindow.h"
#include "gui/nguilabel.h"
#include "gui/nguitextlabel.h"

//------------------------------------------------------------------------------
/**
*/
nViewerApp::nViewerApp() :
#ifdef __WIN32__
    gfxServerClass("nd3d9server"),
#else
    gfxServerClass("nglserver2"),
#endif
    startupScript("home:bin/startup.tcl"),
    sceneServerClass("nstdsceneserver"),
    isOpen(false),
    isOverlayEnabled(true),
    controlMode(Maya),
    featureSetOverride(nGfxServer2::InvalidFeatureSet),
    defViewerPos(0.0f, 1.0f, 0.0f),
    defViewerAngles(0.0f, 0.0f),
    defViewerZoom(0.0f, 0.0f, 9.0f),
    viewerPos(defViewerPos),
    viewerVelocity(500.0f),
    viewerAngles(defViewerAngles),
    viewerZoom(defViewerZoom)
{
    this->kernelServer = nKernelServer::Instance();
}

//------------------------------------------------------------------------------
/**
*/
nViewerApp::~nViewerApp()
{
    if (this->isOpen)
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    This re-initializes the root node (/usr/scene) and initializes the 
    render context.
*/
void
nViewerApp::ValidateRootNode()
{
    if (!this->refRootNode.isvalid())
    {
        this->refRootNode = (nTransformNode*) kernelServer->Lookup("/usr/scene");
        n_assert(this->refRootNode.isvalid());
        static const nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
        nVariable::Handle timeHandle = this->refVarServer->GetVariableHandleByName("time");
        nVariable::Handle oneHandle  = this->refVarServer->GetVariableHandleByName("one");
        nVariable::Handle windHandle = this->refVarServer->GetVariableHandleByName("wind");
        this->renderContext.AddVariable(nVariable(timeHandle, 0.5f));
        this->renderContext.AddVariable(nVariable(oneHandle, 1.0f));
        this->renderContext.AddVariable(nVariable(windHandle, wind));
        this->renderContext.SetRootNode(this->refRootNode.get());
        this->refRootNode->RenderContextCreated(&this->renderContext);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nViewerApp::Open()
{
    n_assert(!this->isOpen);

    // initialize Nebula servers
    this->refScriptServer   = (nScriptServer*)    kernelServer->New(this->GetScriptServerClass(), "/sys/servers/script");
    this->refGfxServer      = (nGfxServer2*)      kernelServer->New(this->GetGfxServerClass(), "/sys/servers/gfx");
    this->refConServer      = (nConServer*)       kernelServer->New("nconserver", "/sys/servers/console");
    this->refResourceServer = (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
    this->refSceneServer    = (nSceneServer*)     kernelServer->New(this->GetSceneServerClass(), "/sys/servers/scene");
    this->refVarServer      = (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
    this->refAnimServer     = (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
    this->refParticleServer = (nParticleServer*)  kernelServer->New("nparticleserver", "/sys/servers/particle");
    this->refVideoServer    = (nVideoServer*)     kernelServer->New("ndshowserver", "/sys/servers/video");
    this->refGuiServer      = (nGuiServer*)       kernelServer->New("nguiserver", "/sys/servers/gui");
    this->refShadowServer   = (nShadowServer*)    kernelServer->New("nshadowserver", "/sys/servers/shadow");
    this->refHttpServer     = (nHttpServer*)      kernelServer->New("nhttpserver", "/sys/servers/http");
    this->refPrefServer     = (nPrefServer*)      kernelServer->New("nwin32prefserver", "/sys/servers/pref");

    // initialize the preferences server
    this->refPrefServer->SetCompanyName("Radon Labs GmbH");
    this->refPrefServer->SetApplicationName("Nebula2 Viewer 1.0");

    // set the gfx server feature set override
    if (this->featureSetOverride != nGfxServer2::InvalidFeatureSet)
    {
        this->refGfxServer->SetFeatureSetOverride(this->featureSetOverride);
    }

    // initialize the proj: assign
    if (this->GetProjDir())
    {
        kernelServer->GetFileServer()->SetAssign("proj", this->GetProjDir());
    }
    else
    {
        kernelServer->GetFileServer()->SetAssign("proj", kernelServer->GetFileServer()->GetAssign("home"));
    }

    // open the remote port
    this->kernelServer->GetRemoteServer()->Open("nviewer");

    // run startup script (assigns must be setup before opening the display!)
    if (this->GetStartupScript())
    {
        const char* result;
        bool r;
        r = this->refScriptServer->RunScript(this->GetStartupScript(), result);
        if (false == r)
        {
            n_error("Executing startup script failed: %s",
                    result ? result : "Unknown error");
        }
    }
    
    // initialize graphics
    this->refGfxServer->SetDisplayMode(this->displayMode);
    this->refGfxServer->SetCamera(this->camera);
    this->refGfxServer->OpenDisplay();
    this->refVideoServer->Open();

    // define the input mapping
    // late initialization of input server, because it relies on 
    // refGfxServer->OpenDisplay having been called
    this->refInputServer    = (nInputServer*)     kernelServer->New("ndi8server", "/sys/servers/input");
    this->DefineInputMapping();

    // create the /usr/scene object
    kernelServer->New("ntransformnode", "/usr/scene");

    // open the scene server
    if (!this->refSceneServer->Open())
    {
        n_error("nViewerApp::Open(): Failed to open nSceneServer!");
        return false;
    }

    // initialize gui
    this->refGuiServer->SetRootPath("/gui");
    this->refGuiServer->SetDisplaySize(vector2(float(this->displayMode.GetWidth()), float(this->displayMode.GetHeight())));
    this->refGuiServer->Open();
    if (this->isOverlayEnabled)
    {
        this->InitOverlayGui();
    }

    // set the stage and load the object
    this->ValidateRootNode();
    if (this->GetSceneFile())
    {
        if (NULL != this->GetStageScript())
        {
            const char* result;
            bool r;

            // run the light stage script
            r = this->refScriptServer->RunScript(this->GetStageScript(), result);
            if (false == r)
            {
                n_error("Executing light stage script failed: %s",
                        result ? result : "Unknown error");
            }
        }

        // load the object to look at
        this->refRootNode->RenderContextDestroyed(&(this->renderContext));
        kernelServer->PushCwd(this->refRootNode.get());
        kernelServer->Load(this->GetSceneFile());
        kernelServer->PopCwd();
        this->refRootNode->RenderContextCreated(&(this->renderContext));
    }

    // initialize view matrix
    this->HandleInputMaya(0.0);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nViewerApp::Close()
{
    n_assert(this->IsOpen());

    this->refSceneServer->Close();
    this->refGuiServer->Close();
    this->refVideoServer->Close();
    this->refGfxServer->CloseDisplay();

    this->refPrefServer->Release();
    this->refHttpServer->Release();
    this->refShadowServer->Release();
    this->refRootNode->Release();
    this->refGuiServer->Release();
    this->refVideoServer->Release();
    this->refParticleServer->Release();
    this->refAnimServer->Release();
    this->refVarServer->Release();
    this->refSceneServer->Release();    
    this->refInputServer->Release();
    this->refGfxServer->Release();
    this->refScriptServer->Release();
    this->refResourceServer->Release();
    this->refConServer->Release();

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nViewerApp::Run()
{
    nVariable::Handle timeHandle = this->refVarServer->GetVariableHandleByName("time");
    nWatched watchViewerPos("viewerPos", nArg::Float4);

    // run the render loop
    bool running = true;
    nTime prevTime = 0.0;
    uint frameId = 0;
    while (this->refGfxServer->Trigger() && running)
    {
        nTime time = kernelServer->GetTimeServer()->GetTime();
        if (prevTime == 0.0)
        {
            prevTime = time;
        }
        float frameTime = (float) (time - prevTime);
        this->refGuiServer->SetTime(time);

        // trigger remote server
        kernelServer->GetRemoteServer()->Trigger();

        // trigger script server
        running = this->refScriptServer->Trigger();

        // trigger particle server
        this->refParticleServer->Trigger();

        // handle input
        this->refInputServer->Trigger(time);
        if (!this->refGuiServer->IsMouseOverGui())
        {
            this->HandleInput(frameTime);
        }

        // trigger gui server
        this->refGuiServer->Trigger();

        // initialize the render context if necessary, this
        // is necessary if someone has re-created /usr/scene
        this->ValidateRootNode();

        // trigger video server
        this->refVideoServer->Trigger();

        // update render context variables
        this->renderContext.GetVariable(timeHandle)->SetFloat((float)time);
        this->TransferGlobalVariables();
        this->renderContext.SetFrameId(frameId++);

        // render
        if (!this->refGfxServer->InDialogBoxMode())
        {
            this->OnFrameBefore();
            if (this->refSceneServer->BeginScene(this->viewMatrix))
            {
                this->refSceneServer->Attach(&this->renderContext);
                this->refSceneServer->EndScene();
                this->refSceneServer->RenderScene();             // renders the 3d scene
                this->OnFrameRendered();
                this->refGuiServer->Render();
                this->refConServer->Render();                    // do additional rendering before presenting the frame
                this->refSceneServer->PresentScene();            // present the frame
            }
        }

        prevTime = time;

        // update watchers
        watchViewerPos->SetV4(vector4(this->viewMatrix.M41, this->viewMatrix.M42, this->viewMatrix.M43, n_rad2deg(this->viewerAngles.rho)));

        // flush input events
        this->refInputServer->FlushEvents();

        // trigger kernel server at end of frame
        kernelServer->Trigger();

        // sleep for a very little while because we
        // are multitasking friendly
        n_sleep(0.0);
    }
}

//------------------------------------------------------------------------------
/**
    Transfer global variables from the variable server to the
    render context.
*/
void
nViewerApp::TransferGlobalVariables()
{
    const nVariableContext& globalContext = this->refVarServer->GetGlobalVariableContext();
    int numGlobalVars = globalContext.GetNumVariables();
    int globalVarIndex;
    for (globalVarIndex = 0; globalVarIndex < numGlobalVars; globalVarIndex++)
    {
        const nVariable& globalVar = globalContext.GetVariableAt(globalVarIndex);
        nVariable* var = this->renderContext.GetVariable(globalVar.GetHandle());
        if (var)
        {
            *var = globalVar;
        }
        else
        {
            nVariable newVar(globalVar);
            this->renderContext.AddVariable(newVar);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Handle general input
*/
void
nViewerApp::HandleInput(float frameTime)
{
    nInputServer* inputServer = this->refInputServer.get();
    
    if (Maya == this->controlMode)
    {
        this->HandleInputMaya(frameTime);
    }
    else
    {
        this->HandleInputFly(frameTime);
    }

    if (inputServer->GetButton("screenshot"))
    {
        nPathString filename;
        const char* sceneFile = this->GetSceneFile();
        if (sceneFile)
        {
            filename = sceneFile;
            filename.StripExtension();
        }
        else
        {
            filename = "screenshot";
        }
        
        int screenshotID = 0;
        char buf[N_MAXPATH];
        do
        {
            snprintf(buf, sizeof(buf), "%s%03d.bmp", filename.Get(), screenshotID++);
        } 
        while (nFileServer2::Instance()->FileExists(buf));
            
        this->refGfxServer->SaveScreenshot(buf);
    }
}

//------------------------------------------------------------------------------
/*
    Define the input mapping.
*/
void
nViewerApp::DefineInputMapping()
{
    const char* scriptResult;
    bool r;
    r = this->refScriptServer->RunFunction("OnMapInput", scriptResult);
    if (false == r)
    {
        n_error("Executing OnMapInput failed: %s",
                scriptResult ? scriptResult : "Unknown error");
    }
}

//------------------------------------------------------------------------------
/**
    Handle input for the Maya control model.
*/
void
nViewerApp::HandleInputMaya(float frameTime)
{
    nInputServer* inputServer = this->refInputServer.get();

    if (frameTime <= 0.0001f)
    {
        frameTime = 0.0001f;
    }

    const float lookVelocity = 0.25f;
    const float panVelocity  = 0.75f;
    const float zoomVelocity = 1.00f;

    bool reset   = inputServer->GetButton("reset");
    bool console = inputServer->GetButton("console");

    float panHori  = 0.0f;
    float panVert  = 0.0f;
    float zoomHori = 0.0f;
    float zoomVert = 0.0f;
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
        zoomHori    = inputServer->GetSlider("left") - inputServer->GetSlider("right");
        zoomVert    = inputServer->GetSlider("down") - inputServer->GetSlider("up"); 
    }

    // do mousewheel zoom
    if (inputServer->GetButton("zoomIn"))
    {
        zoomVert += 1.0f; 
    }
    else if (inputServer->GetButton("zoomOut"))
    {
        zoomVert -= 1.0f; 
    }

    // toggle console
    if (console)
    {
        this->refConServer->Toggle();
    }

    // handle viewer reset
    if (reset)
    {
        this->viewerPos = this->defViewerPos;
        this->viewerZoom = this->defViewerZoom;
        this->viewerAngles = this->defViewerAngles;
    }

    // handle viewer move
    vector3 horiMoveVector(this->viewMatrix.x_component() * panHori * panVelocity);
    vector3 vertMoveVector(this->viewMatrix.y_component() * panVert * panVelocity);
    this->viewerPos += horiMoveVector + vertMoveVector;

    // reset matrix
    this->viewMatrix.ident();

    // handle viewer zoom
    vector3 horiZoomMoveVector(-this->viewMatrix.z_component() * (-zoomHori) * zoomVelocity);
    vector3 vertZoomMoveVector(-this->viewMatrix.z_component() * zoomVert * zoomVelocity);
    this->viewerZoom += horiZoomMoveVector + vertZoomMoveVector ;

    // handle viewer rotation
    this->viewerAngles.theta -= lookVert * lookVelocity;
    this->viewerAngles.rho   += lookHori * lookVelocity;

    // apply changes
    this->viewMatrix.translate(this->viewerZoom);
    this->viewMatrix.rotate_x(this->viewerAngles.theta);
    this->viewMatrix.rotate_y(this->viewerAngles.rho);
    this->viewMatrix.translate(this->viewerPos);

    // switch controls?
    if (inputServer->GetButton("flycontrols"))
    {
        this->SetControlMode(Fly);
    }
}

//------------------------------------------------------------------------------
/**
    Handle input for the Fly control model.
*/
void
nViewerApp::HandleInputFly(float frameTime)
{
    nInputServer* inputServer = this->refInputServer.get();
    if (frameTime <= 0.0001f)
    {
        frameTime = 0.0001f;
    }

    // set speed
    if (inputServer->GetButton("speed0")) this->viewerVelocity = 100.0f;
    if (inputServer->GetButton("speed1")) this->viewerVelocity = 500.0f;
    if (inputServer->GetButton("speed2")) this->viewerVelocity = 5000.0f;

    bool reset   = inputServer->GetButton("reset");
    bool console = inputServer->GetButton("console");

    // toggle console
    if (console)
    {
        this->refConServer->Toggle();
    }

    // handle viewer reset
    if (reset)
    {
        this->viewerPos = this->defViewerPos;
        this->viewerZoom = this->defViewerZoom;
        this->viewerAngles = this->defViewerAngles;
    }

    // handle viewer move
    if (inputServer->GetButton("zoom"))
    {
        this->viewerPos -= this->viewMatrix.z_component() * this->viewerVelocity * frameTime;
    }
    else if (inputServer->GetButton("pan"))
    {
        this->viewerPos -= this->viewMatrix.z_component() * -this->viewerVelocity * frameTime;
    }

    // handle viewer rotate
    float lookHori = 0.0f;
    float lookVert = 0.0f;
    if (inputServer->GetButton("zoom") || inputServer->GetButton("look") || inputServer->GetButton("pan"))
    {
        lookHori = inputServer->GetSlider("left") - inputServer->GetSlider("right");
        lookVert = inputServer->GetSlider("down") - inputServer->GetSlider("up");
    }
    const float lookVelocity = 0.25f;
    this->viewerAngles.theta -= lookVert * lookVelocity;
    this->viewerAngles.rho   += lookHori * lookVelocity;

    // apply changes
    this->viewMatrix.ident();
    this->viewMatrix.rotate_x(this->viewerAngles.theta);
    this->viewMatrix.rotate_y(this->viewerAngles.rho);
    this->viewMatrix.translate(this->viewerPos);

    // switch controls?
    if (inputServer->GetButton("mayacontrols"))
    {
        this->SetControlMode(Maya);
    }
}

//------------------------------------------------------------------------------
/**
    Initialize the overlay GUI.
*/  
void
nViewerApp::InitOverlayGui()
{
    const float borderSize = 0.02f;

    // create a dummy root window
    this->refGuiServer->SetRootWindowPointer(0);
    nGuiWindow* userRootWindow = this->refGuiServer->NewWindow("nguiwindow", true);
    n_assert(userRootWindow);
    rectangle nullRect(vector2(0.0f, 0.0f), vector2(0.0f, 0.0));
    userRootWindow->SetRect(nullRect);

    kernelServer->PushCwd(userRootWindow);

    // create logo label
    nGuiLabel* rightLabel = (nGuiLabel*) kernelServer->New("nguilabel", "RightLogo");
    n_assert(rightLabel);
    vector2 rightLabelSize = this->refGuiServer->ComputeScreenSpaceBrushSize("n2logo");
    rectangle rightRect;
    rightRect.v0.set(1.0f - rightLabelSize.x - borderSize, 1.0f - rightLabelSize.y - borderSize);
    rightRect.v1.set(1.0f - borderSize, 1.0f - borderSize);
    rightLabel->SetRect(rightRect);
    rightLabel->SetDefaultBrush("n2logo");
    rightLabel->SetPressedBrush("n2logo");
    rightLabel->SetHighlightBrush("n2logo");

    // create a help text label
    nGuiTextLabel* textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "HelpLabel");
    n_assert(textLabel);
    textLabel->SetText("Esc: toggle GUI\nSpace: center view\nLMB: rotate\nMMB: pan\nRMB: zoom");
    textLabel->SetFont("GuiSmall");
    textLabel->SetAlignment(nGuiTextLabel::Left);
    textLabel->SetColor(vector4(1.0f, 1.0f, 1.0f, 1.0f));
    textLabel->SetClipping(false);
    vector2 textExtent = textLabel->GetTextExtent();
    rectangle textRect(vector2(0.0f, 0.0f), textExtent);
    textLabel->SetRect(textRect);

    kernelServer->PopCwd();

    // set the new user root window
    this->refGuiServer->SetRootWindowPointer(userRootWindow);
}

//------------------------------------------------------------------------------
/**
    Callback method which is called in the render loop before
    nSceneServer::BeginScene() is called. Overwrite this method
    in a subclass if needed.
*/
void
nViewerApp::OnFrameBefore()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Callback method which is called in the render loop after
    nSceneServer::RenderScene() is called. Overwrite this method
    in a subclass if needed.
*/
void
nViewerApp::OnFrameRendered()
{
    // empty
}
