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
    sceneServerClass("nsceneserver"),
    isOpen(false),
    isOverlayEnabled(true),
    useRam(false),
    lightStageEnabled(true),
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
*/
bool
nViewerApp::Open()
{
    n_assert(!this->isOpen);

    // initialize Nebula servers
    this->refScriptServer   = (nScriptServer*)    kernelServer->New(this->GetScriptServerClass().Get(), "/sys/servers/script");
    this->refGfxServer      = (nGfxServer2*)      kernelServer->New(this->GetGfxServerClass().Get(), "/sys/servers/gfx");
    this->refInputServer    = (nInputServer*)     kernelServer->New("ndi8server", "/sys/servers/input");
    this->refConServer      = (nConServer*)       kernelServer->New("nconserver", "/sys/servers/console");
    this->refResourceServer = (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
    this->refSceneServer    = (nSceneServer*)     kernelServer->New(this->GetSceneServerClass().Get(), "/sys/servers/scene");
    this->refVarServer      = (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
    this->refAnimServer     = (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
    this->refParticleServer = (nParticleServer*)  kernelServer->New("nparticleserver", "/sys/servers/particle");
    this->refParticleServer2= (nParticleServer2*) kernelServer->New("nparticleserver2", "/sys/servers/particle2");
    this->refVideoServer    = (nVideoServer*)     kernelServer->New("ndshowserver", "/sys/servers/video");
    this->refGuiServer      = (nGuiServer*)       kernelServer->New("nguiserver", "/sys/servers/gui");
    this->refHttpServer     = (nHttpServer*)      kernelServer->New("nhttpserver", "/sys/servers/http");
    this->refPrefServer     = (nPrefServer*)      kernelServer->New("nwin32prefserver", "/sys/servers/pref");
    this->refAudioServer    = (nAudioServer3*)    kernelServer->New("ndsoundserver3",  "/sys/servers/audio");
    this->refCaptureServer  = (nCaptureServer*)   kernelServer->New("ncaptureserver", "/sys/servers/capture");
    this->refShadowServer   = (nShadowServer2*)   kernelServer->New("nshadowserver2", "/sys/servers/shadow2");
    this->refToolkitServer  = (nToolkitServer*)   kernelServer->New("ntoolkitserver", "/sys/servers/toolkit");

    // initialize the preferences server
    this->refPrefServer->SetCompanyName("Radon Labs GmbH");
    this->refPrefServer->SetApplicationName("Nebula2 Viewer 1.0");

    // initialize the capture server
    this->refCaptureServer->SetBaseDirectory("user:Radon Labs GmbH/nviewer/capture");

    // set the gfx server feature set override
    if (this->featureSetOverride != nGfxServer2::InvalidFeatureSet)
    {
        this->refGfxServer->SetFeatureSetOverride(this->featureSetOverride);
    }

    // initialize the proj: assign
    if (!this->GetProjDir().IsEmpty())
    {
        kernelServer->GetFileServer()->SetAssign("proj", this->GetProjDir().Get());
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
    if (!this->GetStartupScript().IsEmpty())
    {
        nString result;
        bool r;
        r = this->refScriptServer->RunScript(this->GetStartupScript().Get(), result);
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
    if (!this->renderPath.IsEmpty())
    {
        this->refSceneServer->SetRenderPathFilename(this->renderPath);
    }
    this->refSceneServer->SetOcclusionQuery(false);
    this->refSceneServer->SetObeyLightLinks(false);

    // open the scene server
    if (!this->refSceneServer->Open())
    {
        n_error("nViewerApp::Open(): Failed to open nSceneServer!");
        return false;
    }
    this->refVideoServer->Open();

    // define the input mapping
    this->refInputServer->Open();
    this->DefineInputMapping();

    this->refAudioServer->Open();

    // Setup /usr/scene and add default entry + light
    this->nodeList.SetStageScript(this->GetStageScript());
    this->nodeList.SetLightStageEnabled(this->lightStageEnabled);
    this->nodeList.Open();

    // initialize gui
    this->refGuiServer->SetRootPath("/gui");
    this->refGuiServer->SetDisplaySize(vector2(float(this->displayMode.GetWidth()), float(this->displayMode.GetHeight())));
    this->refGuiServer->Open();
    if (this->isOverlayEnabled)
    {
        this->InitOverlayGui();
    }

    // load the object
    if (!this->GetSceneFile().IsEmpty())
    {
        // Switch to ramfileserver if demanded.
        if (this->UseRam())
        {
            nKernelServer::Instance()->ReplaceFileServer("nramfileserver");
        }
        this->nodeList.LoadObject(this->GetSceneFile());
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

    this->nodeList.Close();

    this->refGuiServer->Close();
    this->refVideoServer->Close();
    this->refSceneServer->Close();

    this->refCaptureServer->Release();
    this->refAudioServer->Release();
    this->refPrefServer->Release();
    this->refHttpServer->Release();
    this->refShadowServer->Release();
    this->refGuiServer->Release();
    this->refVideoServer->Release();
    this->refParticleServer->Release();
    this->refParticleServer2->Release();
    this->refAnimServer->Release();
    this->refVarServer->Release();
    this->refSceneServer->Release();
    this->refInputServer->Release();
    this->refGfxServer->Release();
    this->refScriptServer->Release();
    this->refResourceServer->Release();
    this->refConServer->Release();
    this->refToolkitServer->Release();

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nViewerApp::Run()
{
    nWatched watchViewerPos("viewerPos", nArg::Float4);
    nWatched watchCamPos("camPos", nArg::Float4);

    // run the render loop
    bool running = true;
    float frameTime;
    nTime prevTime = 0.0;
    uint frameId = 0;
    while (this->refGfxServer->Trigger() && running)
    {
        nTimeServer::Instance()->Trigger();
        nTime time = nTimeServer::Instance()->GetTime();
        if (prevTime == 0.0)
        {
            prevTime = time;
        }

        // Trigger Audioserver
        this->refAudioServer->BeginScene(time);

        frameTime = (float) (time - prevTime);
        this->refGuiServer->SetTime(time);

        // trigger remote server
        kernelServer->GetRemoteServer()->Trigger();

        // trigger script server
        running = this->refScriptServer->Trigger();

        // trigger particle server
        this->refParticleServer->Trigger();
        this->refParticleServer2->Trigger();

        // handle input
        nInputServer* inputServer = nInputServer::Instance();
        inputServer->Trigger(time);

        if (!this->refGuiServer->IsMouseOverGui())
        {
            // give inputs to camControl
            this->camControl.SetResetButton(inputServer->GetButton("reset"));
            this->camControl.SetLookButton(inputServer->GetButton("look"));
            this->camControl.SetPanButton(inputServer->GetButton("pan"));
            this->camControl.SetZoomButton(inputServer->GetButton("zoom"));
            this->camControl.SetSliderLeft(inputServer->GetSlider("left"));
            this->camControl.SetSliderRight(inputServer->GetSlider("right"));
            this->camControl.SetSliderUp(inputServer->GetSlider("up"));
            this->camControl.SetSliderDown(inputServer->GetSlider("down"));

            // Toggle console
            if (inputServer->GetButton("console"))
            {
                this->refConServer->Toggle();
            }

            // Make Screenshot
            if (inputServer->GetButton("makescreenshot"))
            {
                nString basename = this->GetSceneFile().Get();
                if (basename.IsValid())
                {
                    basename.StripExtension();
                }
                else
                {
                    basename = "screenshot";
                }

                int screenshotID = 0;
                nString filename;
                do
                {
                    filename.Format("user:%s%03d.bmp", filename.Get(), screenshotID++);
                }
                while (nFileServer2::Instance()->FileExists(filename));

                nTexture2* screenshotBuffer = (nTexture2*) nResourceServer::Instance()->FindResource("Screenshot",nResource::Texture);
                n_assert(screenshotBuffer);
                screenshotBuffer->SaveTextureToFile(filename, nTexture2::JPG);
            };
            // update view and get the actual viewMatrix
            this->camControl.Update();
            this->viewMatrix = this->camControl.GetViewMatrix();
        }

        // trigger gui server
        this->refGuiServer->Trigger();

        // trigger video server
        this->refVideoServer->Trigger();

        // update render context variables
        this->nodeList.Trigger((float)time, frameId++);

        // render
        if (!this->refGfxServer->InDialogBoxMode())
        {
            this->OnFrameBefore();
            if (this->refSceneServer->BeginScene(this->viewMatrix))
            {
                for (uint index = 0; index < this->nodeList.GetCount(); index++)
                {
                    this->refSceneServer->Attach(this->nodeList.GetRenderContextAt(index));
                }
                this->refSceneServer->EndScene();
                this->refSceneServer->RenderScene();             // renders the 3d scene
                this->OnFrameRendered();
                this->refCaptureServer->Trigger();
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

        // Trigger Audioserver / important for streaming sounds
        this->refAudioServer->UpdateAllSounds();
        this->refAudioServer->EndScene();

        // give up time slice
        n_sleep(0.0);
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
    r = this->refScriptServer->RunFunction("OnViewerMapInput", scriptResult);
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
    rightLabel->OnShow();

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
    textLabel->OnShow();

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
