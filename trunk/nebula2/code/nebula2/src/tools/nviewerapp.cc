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
    startupScript("home:data/scripts/startup.tcl"),
    sceneServerClass("nmrtsceneserver"),
    isOpen(false),
    isOverlayEnabled(true),
    useRam(false),
    featureSetOverride(nGfxServer2::InvalidFeatureSet)
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

    // Initialize the display mode now so that the startup script can
    // override it.
    this->refGfxServer->SetDisplayMode(this->displayMode);

    // run startup script (assigns must be setup before opening the display!)
    if (this->GetStartupScript())
    {
        nString result;
        bool r;
        r = this->refScriptServer->RunScript(this->GetStartupScript(), result);
        if (false == r)
        {
            n_error("Executing startup script failed: %s",
                    !result.IsEmpty() ? result.Get() : "Unknown error");
        }
    }

    nString scriptResult;
    this->refScriptServer->RunFunction("OnStartup", scriptResult);
    this->refScriptServer->RunFunction("OnGraphicsStartup", scriptResult);

    // initialize graphics
    this->refGfxServer->SetCamera(this->camera);
    // open the scene server
    if (!this->refSceneServer->Open())
    {
        n_error("nViewerApp::Open(): Failed to open nSceneServer!");
        return false;
    }
    this->refVideoServer->Open();

    // define the input mapping
    // late initialization of input server, because it relies on 
    // refGfxServer->OpenDisplay having been called
    this->refInputServer    = (nInputServer*)     kernelServer->New("ndi8server", "/sys/servers/input");
    this->DefineInputMapping();

    // create the /usr/scene object
    kernelServer->New("ntransformnode", "/usr/scene");



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
            // load new object
            kernelServer->PushCwd(this->refRootNode.get());
            // source the light stage...
            kernelServer->Load(this->GetStageScript());
            kernelServer->PopCwd();
        }

        // Switch to ramfileserver if demanded.
        if (UseRam())
        {
            nKernelServer::Instance()->ReplaceFileServer("nramfileserver");
        }
        
        // load the object to look at
        this->refRootNode->RenderContextDestroyed(&(this->renderContext));
        kernelServer->PushCwd(this->refRootNode.get());
        kernelServer->Load(this->GetSceneFile());
        kernelServer->PopCwd();
        this->refRootNode->RenderContextCreated(&(this->renderContext));
    }

    // initialize view matrix
    camControl.Initialize();
    viewMatrix = camControl.GetViewMatrix();

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
    this->refSceneServer->Close();

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
    nWatched watchCamPos("camPos", nArg::Float4);

    // run the render loop
    bool running = true;
    float frameTime;
    nTime prevTime = 0.0;
    uint frameId = 0;
    while (this->refGfxServer->Trigger() && running)
    {
        nTime time = kernelServer->GetTimeServer()->GetTime();
        if (prevTime == 0.0)
        {
            prevTime = time;
        }
        frameTime = (float) (time - prevTime);
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
            // give inputs to camControl
            this->camControl.SetResetButton(this->refInputServer->GetButton("reset"));
            this->camControl.SetLookButton(this->refInputServer->GetButton("look"));
            this->camControl.SetPanButton(this->refInputServer->GetButton("pan"));
            this->camControl.SetZoomButton(this->refInputServer->GetButton("zoom"));
            this->camControl.SetSliderLeft(this->refInputServer->GetSlider("left"));
            this->camControl.SetSliderRight(this->refInputServer->GetSlider("right"));
            this->camControl.SetSliderUp(this->refInputServer->GetSlider("up"));
            this->camControl.SetSliderDown(this->refInputServer->GetSlider("down"));
            
            // Toggle console
            if (true == this->refInputServer->GetButton("console"))
            {
                this->refConServer->Toggle();
            }

            // screenshot
            if (this->refInputServer->GetButton("screenshot"))
            {
                nString filename;
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

            // update view and get the actual viewMatrix
            this->camControl.Update();
            this->viewMatrix = this->camControl.GetViewMatrix();
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
                this->refSceneServer->PresentScene();            // present the frame
            }
        }

        prevTime = time;

        // update watchers
        watchViewerPos->SetV4(camControl.GetViewMatrix().pos_component());
        watchCamPos->SetV4(camControl.GetCenterOfInterest());

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
/*
    Define the input mapping.
*/
void
nViewerApp::DefineInputMapping()
{
    nString scriptResult;
    bool r;
    r = this->refScriptServer->RunFunction("OnMapInput", scriptResult);
    if (false == r)
    {
        n_error("Executing OnMapInput failed: %s",
                !scriptResult.IsEmpty() ? scriptResult.Get() : "Unknown error");
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
