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

//------------------------------------------------------------------------------
/**
*/
nViewerApp::nViewerApp(nKernelServer* ks) :
    startupScript("home:bin/startup.tcl"),
    kernelServer(ks),
    isOpen(false),
    isOverlayEnabled(true),
    controlMode(Maya),
    featureSetOverride(nGfxServer2::InvalidFeatureSet),
    defViewerPos(0.0f, 0.0f, 0.0f),
    defViewerAngles(0.0f, 0.0f),
    defViewerZoom(0.0f, 0.0f, 9.0f),
    viewerPos(defViewerPos),
    viewerVelocity(500.0f),
    viewerAngles(defViewerAngles),
    viewerZoom(defViewerZoom),
    screenshotID(0)
{
    // empty
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
*/
bool
nViewerApp::Open()
{
    const char* result;
    n_assert(!this->isOpen);

    // initialize Nebula servers
    this->refScriptServer   = (nScriptServer*)    kernelServer->New(this->GetScriptServerClass(), "/sys/servers/script");
    this->refGfxServer      = (nGfxServer2*)      kernelServer->New("nd3d9server", "/sys/servers/gfx");
    this->refInputServer    = (nInputServer*)     kernelServer->New("ndi8server", "/sys/servers/input");
    this->refConServer      = (nConServer*)       kernelServer->New("nconserver", "/sys/servers/console");
    this->refResourceServer = (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
    this->refSceneServer    = (nSceneServer*)     kernelServer->New(this->GetSceneServerClass(), "/sys/servers/scene");
    this->refVarServer      = (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
    this->refAnimServer     = (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
    this->refParticleServer = (nParticleServer*)  kernelServer->New("nparticleserver", "/sys/servers/particle");
    this->refVideoServer    = (nVideoServer*)     kernelServer->New("ndshowserver", "/sys/servers/vdeo");
    this->refGuiServer      = (nGuiServer*)       kernelServer->New("nguiserver", "/sys/servers/gui");

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

    // create scene graph root node
    this->refRootNode = (nTransformNode*) kernelServer->New("ntransformnode",  "/usr/scene");

    // open the remote port
    this->kernelServer->GetRemoteServer()->Open("nviewer");

    // run startup script (assigns must be setup before opening the display!)
    if (this->GetStartupScript())
    {
        this->refScriptServer->RunScript(this->GetStartupScript(), result);
    }
    
    // define the input mapping
    if (NULL != this->GetInputScript())
    {
        this->refScriptServer->RunScript(this->GetInputScript(), result);
    }

    // initialize graphics
    this->refGfxServer->SetDisplayMode(this->displayMode);
    this->refGfxServer->SetCamera(this->camera);
    this->refGfxServer->OpenDisplay();
    this->refVideoServer->Open();

    // initialize gui
    this->refGuiServer->SetRootPath("/gui");
    this->refGuiServer->Open();
    if (this->isOverlayEnabled)
    {
        this->InitOverlayGui();
    }

    // set the stage and load the object
    if (this->GetSceneFile())
    {
        if (NULL != this->GetStageScript())
        {
            const char* result;

            // run the light stage script
            this->refScriptServer->RunScript(this->GetStageScript(), result);
        }

        // load the object to look at
        kernelServer->PushCwd(this->refRootNode.get());
        kernelServer->Load(this->GetSceneFile());
        kernelServer->PopCwd();
    }

    // initialize the render context
    nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    nVariable::Handle timeHandle = this->refVarServer->GetVariableHandleByName("time");
    nVariable::Handle oneHandle  = this->refVarServer->GetVariableHandleByName("one");
    nVariable::Handle windHandle = this->refVarServer->GetVariableHandleByName("wind");
    this->renderContext.AddVariable(nVariable(timeHandle, 0.5f));
    this->renderContext.AddVariable(nVariable(oneHandle, 1.0f));
    this->renderContext.AddVariable(nVariable(windHandle, wind));
    this->renderContext.SetRootNode(this->refRootNode.get());
    this->refRootNode->RenderContextCreated(&this->renderContext);

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

    this->refGuiServer->Close();
    this->refVideoServer->Close();
    this->refGfxServer->CloseDisplay();

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

        // trigger video server
        this->refVideoServer->Trigger();

        // update render context variables
        this->renderContext.GetVariable(timeHandle)->SetFloat((float)time);
        this->TransferGlobalVariables();
        this->renderContext.SetFrameId(frameId++);

        // render
        if (!this->refGfxServer->InDialogBoxMode())
        {
            if (this->refSceneServer->BeginScene(viewMatrix))
            {
                this->refSceneServer->Attach(&this->renderContext);
                this->refSceneServer->EndScene();
                this->refSceneServer->RenderScene();             // renders the 3d scene
                this->refGuiServer->Render();
                this->refConServer->Render();                    // do additional rendering before presenting the frame
                this->refSceneServer->PresentScene();            // present the frame
            }
        }

        prevTime = time;

        // update watchers
        watchViewerPos->SetV4(vector4(viewMatrix.M41, viewMatrix.M42, viewMatrix.M43, n_rad2deg(this->viewerAngles.rho)));

        // flush input events
        this->refInputServer->FlushEvents();

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
        filename.Append(nString(this->screenshotID++));
        filename.Append(".bmp");

        this->refGfxServer->SaveScreenshot(filename.Get());
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
    if (inputServer->GetButton("speed0")) this->viewerVelocity = 20.0f;
    if (inputServer->GetButton("speed1")) this->viewerVelocity = 500.0f;
    if (inputServer->GetButton("speed2")) this->viewerVelocity = 5000.0f;

    // set predefined positions
    if (inputServer->GetButton("setpos0"))
    {
        this->viewerPos.set(119226.0f, 1373.0f, 89417.0f);
        this->viewerAngles.rho = n_deg2rad(-419.0f);
    }
    if (inputServer->GetButton("setpos1"))
    {
        this->viewerPos.set(96878.0f, 905.0f, 129697.0f);
        this->viewerAngles.rho = n_deg2rad(42.0f);
    }
    if (inputServer->GetButton("setpos2"))
    {
        this->viewerPos.set(96991.0f, 2028.0f, 155915.0f);
        this->viewerAngles.rho = n_deg2rad(-383.0f);
    }
    if (inputServer->GetButton("setpos3"))
    {
        this->viewerPos.set(7103.0f, 645.0f, 9505.0f);
        this->viewerAngles.rho = n_deg2rad(-130.0f);
    }

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
    rectangle rect(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f));
    userRootWindow->SetRect(rect);

    kernelServer->PushCwd(userRootWindow);

    // create logo label
    nGuiLabel* logoLabel = (nGuiLabel*) kernelServer->New("nguilabel", "n2logo");
    n_assert(logoLabel);
    vector2 logoLabelSize = this->refGuiServer->ComputeScreenSpaceBrushSize("n2logo");
    rectangle logoRect;
    logoRect.v0.set(1.0f - logoLabelSize.x - borderSize, 1.0f - logoLabelSize.y - borderSize);
    logoRect.v1.set(1.0f - borderSize, 1.0f - borderSize);
    logoLabel->SetRect(logoRect);
    logoLabel->SetDefaultBrush("n2logo");
    logoLabel->SetPressedBrush("n2logo");
    logoLabel->SetHighlightBrush("n2logo");

    kernelServer->PopCwd();

    // set the new user root window
    this->refGuiServer->SetRootWindowPointer(userRootWindow);
}
