//------------------------------------------------------------------------------
//  nSDBViewerApp.cc
//  (C) 2003 RadonLabs GmbH
//  (C) 2004 Gary Haussmann
//------------------------------------------------------------------------------
#include "spatialdb/nsdbviewerapp.h"
#include "spatialdb/nvisiblefrustumgenarray.h"
#include "spatialdb/noccludingfrustumgenarray.h"
#include "spatialdb/nvisiblespheregenarray.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "misc/nwatched.h"
#include "gui/nguiwindow.h"
#include "gui/nguilabel.h"

#include <math.h>

//------------------------------------------------------------------------------
/**
*/
nSDBViewerApp::nSDBViewerApp(nKernelServer* ks) :
    startupScript("nsdbscripts:startup.lua"),
    kernelServer(ks),
    isOpen(false),
    isOverlayEnabled(true),
    camera(60.0f, 4.0f/3.0f, 0.1f, 1000.0f),
    controlMode(Maya),
    defViewerPos(0.0f, 0.0f, 0.0f),
//  defViewerAngles(n_deg2rad(90.0f), n_deg2rad(0.0f)),
    defViewerAngles(0,0),
//  defViewerPos(300.0f, 2000.0f, 300.0f),
//  defViewerAngles(n_deg2rad(-80.0f), n_deg2rad(-180.0f)),
    defViewerZoom(0.0f, 0.0f, 0.0f),
//    viewerPos(0.f,0.0f,50.0f),
    viewerVelocity(10.0f),
//    viewerAngles(defViewerAngles),
//    viewerZoom(defViewerZoom),
    screenshotID(0),
    m_frustumclip(true), m_occludingfrustumclip(false), m_sphereclip(false), m_occludingsphereclip(false),
    m_viewcamera(0), m_activecamera(1), m_viscamera(1), m_testobjects(10,10)
{
    // initialize test cameras
    markcameras[0].viewerAngles = polar2(n_deg2rad(-60.0f), n_deg2rad(45.f));
    markcameras[0].viewerPos = this->defViewerPos;
    markcameras[0].viewerZoom = vector3(0,0,50.0f);
    markcameras[1].viewerAngles = this->defViewerAngles;
    markcameras[1].viewerPos = this->defViewerPos;
    markcameras[1].viewerZoom = this->defViewerZoom;
    markcameras[2].viewerAngles = this->defViewerAngles;
    markcameras[2].viewerPos = this->defViewerPos + vector3(7.0f,0.0f,0.0f);;
    markcameras[2].viewerZoom = this->defViewerZoom;
}

//------------------------------------------------------------------------------
/**
*/
nSDBViewerApp::~nSDBViewerApp()
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
nSDBViewerApp::Open()
{
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
    this->refCameraMarkerNode = (nTransformNode*)kernelServer->New("ntransformnode", "/usr/camera");
    this->refTestObjectNode = (nTransformNode*)kernelServer->New("ntransformnode", "/usr/testobject");
    this->refOccluderObjectNode = (nTransformNode*)kernelServer->New("ntransformnode", "/usr/occluderobject");

    // open the remote port
    this->kernelServer->GetRemoteServer()->Open("nsdbviewer");

    // run startup script
    if (this->GetStartupScript())
    {
        const char* result;
        this->refScriptServer->RunScript(this->GetStartupScript(), result);
    }

    // initialize graphics
    this->refGfxServer->SetDisplayMode(this->displayMode);
    this->refGfxServer->SetCamera(this->camera);
    this->refGfxServer->OpenDisplay();

    // define the input mapping
    if (NULL != this->GetInputScript())
    {
        const char* result;
        this->refScriptServer->RunScript(this->GetInputScript(), result);
    }

    // initialize gui
    this->refGuiServer->SetRootPath("/gui");
    this->refGuiServer->Open();
    if (this->isOverlayEnabled)
    {
        this->InitOverlayGui();
    }

    if (this->GetSceneFile())
    {
        // load the stage (normally stdlight.lua)
        const char* result;
        this->refScriptServer->RunScript(this->GetStageScript(), result);

        // load the object to look at
        kernelServer->PushCwd(this->refTestObjectNode.get());
        kernelServer->Load(this->GetSceneFile());
        kernelServer->PopCwd();

        kernelServer->PushCwd(this->refCameraMarkerNode.get());
        kernelServer->Load("localgfxlib:cammarker.n2");
        kernelServer->PopCwd();

        kernelServer->PushCwd(this->refOccluderObjectNode.get());
        kernelServer->Load("localgfxlib:unitcube.n2");
        kernelServer->PopCwd();
    }

    // initialize the main render context
    nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    nVariable::Handle timeHandle = this->refVarServer->GetVariableHandleByName("time");
    nVariable::Handle oneHandle  = this->refVarServer->GetVariableHandleByName("one");
    nVariable::Handle windHandle = this->refVarServer->GetVariableHandleByName("wind");
    this->renderContext.AddVariable(nVariable(timeHandle, 0.5f));
    this->renderContext.AddVariable(nVariable(oneHandle, 1.0f));
    this->renderContext.AddVariable(nVariable(windHandle, wind));
    this->renderContext.SetRootNode(this->refRootNode.get());
    this->refRootNode->RenderContextCreated(&this->renderContext);


    for (int camix=0; camix < nSDBViewerApp::CAMERACOUNT; camix++)
    {
        markcameras[camix].camerarc.AddVariable(nVariable(timeHandle,0.5f));
        markcameras[camix].camerarc.AddVariable(nVariable(oneHandle, 1.0f));
        markcameras[camix].camerarc.AddVariable(nVariable(windHandle, wind));
        markcameras[camix].camerarc.SetRootNode(this->refCameraMarkerNode.get());
        this->refCameraMarkerNode->RenderContextCreated(&(markcameras[camix].camerarc));
        matrix44 dummy;
        markcameras[camix].GenerateTransform(dummy);
    }

    m_rootsector = (nSpatialSector *)kernelServer->New("nspatialsector", "/world/rootsector");
    m_rootsector->Configure(matrix44());
    ResetTestObjects();

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void nSDBViewerApp::Close()
{
    n_assert(this->IsOpen());

    DeleteTestObjects();

    this->refGuiServer->Close();
    this->refGfxServer->CloseDisplay();

    this->refRootNode->Release();
    this->refGuiServer->Release();
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
void nSDBViewerApp::Run()
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

        // update render context variables
        this->renderContext.GetVariable(timeHandle)->SetFloat((float)time);
//        this->TransferGlobalVariables();
        this->renderContext.SetFrameId(frameId++);

        if (!this->refGfxServer->InDialogBoxMode())
        {
            // render
            matrix44 cameravm;
            markcameras[m_viewcamera].GenerateTransform(cameravm);

            this->refSceneServer->BeginScene(cameravm);

            // attach light
            this->refSceneServer->Attach(&this->renderContext);

            // attach test objects
            UpdateObjectMarks(); // mark visible objects by color
//            const nVariable *globaltime = this->renderContext.GetVariable(timeHandle);
            // if no clipping is defined, render everything
            bool renderall(!m_frustumclip & !m_sphereclip & !m_occludingfrustumclip);
            for (nArray<SDBTestObject *>::iterator objectiter = m_testobjects.Begin();
                                                 objectiter != m_testobjects.End();
                                                 objectiter++)
            {
                SDBTestObject *curobject = *objectiter;
                if ( renderall || (curobject->spatialmarkflags > 0) )
                {
                    nVariable* var = curobject->renderc.GetVariable(timeHandle);
                    if (var)
                    {
                        var->SetFloat((float)time);
                    }
                    else
                    {
                        nVariable newVar(timeHandle, (float)time);
                        curobject->renderc.AddVariable(newVar);
                    }
                    this->refSceneServer->Attach(&(curobject->renderc));
                }
            }
            
            // attach camera markers-note that camera transforms were updated in UpdateObjectMarks()
            for (int camix=0; camix < nSDBViewerApp::CAMERACOUNT; camix++)
            {
                nRenderContext &camrc = markcameras[camix].camerarc;
                camrc.SetFrameId(frameId);
                // turn off lighting for all cameras but the active one
                nFloat4 camlight = {0.f,0.f,0.f,1.0f};
                if (camix == m_activecamera)
                {   
                    camlight.x = camlight.w = 1.0f;
                    camlight.y = camlight.z = 1.0f;
                }
                nShaderParams &camparams = camrc.GetShaderOverrides();
                nShaderArg shaderdiffuselight(camlight);
                camparams.SetArg(nShaderState::MatDiffuse, shaderdiffuselight);
                this->refSceneServer->Attach(&camrc);

            }

            this->refSceneServer->EndScene();
            this->refSceneServer->RenderScene();             // renders the 3d scene
            this->refGuiServer->Render();                    // do additional rendering before presenting the frame
            this->refConServer->Render();

            // draw the visitor debug info
            matrix44 cameraxform0, identmatrix;
            this->markcameras[m_viscamera].GenerateTransform(cameraxform0);
            nVisibleFrustumGenArray::VisibleElements dummyarray(10,10);
            nCamera2 newcamera(this->camera);
            newcamera.SetFarPlane(40.f);
            newcamera.SetNearPlane(2.0f);
            nVisibleFrustumGenArray generator(newcamera, cameraxform0, dummyarray);
            identmatrix.ident();
            this->refGfxServer->SetTransform(nGfxServer2::Model, identmatrix);
            generator.VisualizeDebug(this->refGfxServer.get());


            this->refSceneServer->PresentScene();            // present the frame
        }

        prevTime = time;

        // update watchers
        nSDBViewerApp::CameraDescription &activecamera = this->markcameras[m_activecamera];
        watchViewerPos->SetV4(vector4(activecamera.viewMatrix.M41, activecamera.viewMatrix.M42, activecamera.viewMatrix.M43, 
            n_rad2deg(activecamera.viewerAngles.rho)));

        // flush input events
        this->refInputServer->FlushEvents();

        // sleep for a very little while because we
        // are multitasking friendly
        n_sleep(0.0);
    }
}


//------------------------------------------------------------------------------
/**
    Handle general input
*/
void
nSDBViewerApp::HandleInput(float frameTime)
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
        filename.AppendInt(this->screenshotID++);
        filename.Append(".bmp");

        this->refGfxServer->SaveScreenshot(filename.Get());
    }

    this->HandleInputPlay(frameTime);
    this->HandlePlaySwitch(frameTime);

}

/// handle movement of the 'play' objects using visibility
void nSDBViewerApp::HandleInputPlay(float frameTime)
{
    nInputServer* inputServer = this->refInputServer.get();
    
    // process 'play' commands like move/turn for the active object

    CameraDescription &playcamera = markcameras[m_activecamera];
    const float turnspeed = 1.0f;
    const float movespeed = 5.0f;

    if (inputServer->GetButton("turnright"))
    {
        if (inputServer->GetButton("strafemode"))
            playcamera.viewerPos += playcamera.viewMatrix.x_component() * movespeed * frameTime;
        else
            playcamera.viewerAngles.rho -= turnspeed * frameTime;
    }

    if (inputServer->GetButton("turnleft"))
    {
        if (inputServer->GetButton("strafemode"))
            playcamera.viewerPos -= playcamera.viewMatrix.x_component() * movespeed * frameTime;
        else
            playcamera.viewerAngles.rho += turnspeed * frameTime;
    }

    if (inputServer->GetButton("moveforward"))
    {
        playcamera.viewerPos -= playcamera.viewMatrix.z_component() * movespeed * frameTime;
    }
    if (inputServer->GetButton("movebackward"))
    {
        playcamera.viewerPos += playcamera.viewMatrix.z_component() * movespeed * frameTime;
    }

    matrix44 dummy;
    playcamera.GenerateTransform(dummy);
    
}
/// handle state change--mainly switching play object
void nSDBViewerApp::HandlePlaySwitch(float framtTime)
{
    nInputServer* inputServer = this->refInputServer.get();

    if (inputServer->GetButton("activatecam1"))
    {
        m_activecamera = 1;
        m_viscamera = 1;
    }
    if (inputServer->GetButton("activatecam2"))
    {
        m_activecamera = 2;
        m_viscamera = 2;
    }
    
    if (inputServer->GetButton("togglefrustum"))
    {
        // cycle between frustum clip and occluding frustum clip
        m_frustumclip = ! m_frustumclip;
        m_occludingfrustumclip = !m_frustumclip;
    }

    if (inputServer->GetButton("togglesphere"))
    {
        m_sphereclip = ! m_sphereclip;
    }

    if (inputServer->GetButton("toggleoccludingsphere"))
    {
        m_occludingsphereclip = ! m_occludingsphereclip;
    }

    if (inputServer->GetButton("cycleviewcamera"))
    {
        // toggle between the chase cam and the active camera
        if (m_viewcamera == m_activecamera)
            m_viewcamera = 0;
        else
            m_viewcamera = m_activecamera;
    }

    if (inputServer->GetButton("cyclecontrolcamera"))
    {
        m_activecamera = (m_activecamera+1) % nSDBViewerApp::CAMERACOUNT;
        // never switch keyboard control to the 'main' camera
        if (m_activecamera == 0)
            m_activecamera = 1;
        // switch over view if it was on the control camera
        if (m_viewcamera != 0)
            m_viewcamera = m_activecamera;
    }

    if (inputServer->GetButton("cycleviscamera"))
    {
        m_viscamera = (m_viscamera+1) % nSDBViewerApp::CAMERACOUNT;
    }
}

//------------------------------------------------------------------------------
/**
    Handle input for the Maya control model.
*/
void
nSDBViewerApp::HandleInputMaya(float frameTime)
{
    nInputServer* inputServer = this->refInputServer.get();

    if (frameTime <= 0.0001f)
    {
        frameTime = 0.0001f;
    }

    const float lookVelocity = 0.25f;
    const float panVelocity  = 10.75f;
    const float zoomVelocity = 10.00f;

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

    // toggle console
    if (console)
    {
        this->refConServer->Toggle();
    }

    CameraDescription &controlcamera(this->markcameras[0]);//m_viewcamera]);

    // handle viewer reset
    if (reset)
    {
        controlcamera.viewerPos = this->defViewerPos;
        controlcamera.viewerZoom = this->defViewerZoom;
        controlcamera.viewerAngles = this->defViewerAngles;
    }

    // handle viewer move
    vector3 horiMoveVector(controlcamera.viewMatrix.x_component() * panHori * panVelocity);
    vector3 vertMoveVector(controlcamera.viewMatrix.y_component() * panVert * panVelocity);
    controlcamera.viewerPos += horiMoveVector + vertMoveVector;

    // handle viewer zoom
    vector3 horiZoomMoveVector(0,0, zoomHori * zoomVelocity);
    vector3 vertZoomMoveVector(0,0, - zoomVert * zoomVelocity);
    controlcamera.viewerZoom += horiZoomMoveVector + vertZoomMoveVector ;

    // handle viewer rotation
    controlcamera.viewerAngles.theta -= lookVert * lookVelocity;
    controlcamera.viewerAngles.rho   += lookHori * lookVelocity;

    // apply changes
    matrix44 dummy;
    controlcamera.GenerateTransform(dummy);

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
nSDBViewerApp::HandleInputFly(float frameTime)
{
    nInputServer* inputServer = this->refInputServer.get();
    if (frameTime <= 0.0001f)
    {
        frameTime = 0.0001f;
    }

    CameraDescription &controlcamera(this->markcameras[0]);//m_viewcamera]);

    // set predefined positions
    if (inputServer->GetButton("setpos0"))
    {
        controlcamera.viewerPos.set(119226.0f, 1373.0f, 89417.0f);
        controlcamera.viewerAngles.rho = n_deg2rad(-419.0f);
    }
    if (inputServer->GetButton("setpos1"))
    {
        controlcamera.viewerPos.set(96878.0f, 905.0f, 129697.0f);
        controlcamera.viewerAngles.rho = n_deg2rad(42.0f);
    }
    if (inputServer->GetButton("setpos2"))
    {
        controlcamera.viewerPos.set(96991.0f, 2028.0f, 155915.0f);
        controlcamera.viewerAngles.rho = n_deg2rad(-383.0f);
    }
    if (inputServer->GetButton("setpos3"))
    {
        controlcamera.viewerPos.set(7103.0f, 645.0f, 9505.0f);
        controlcamera.viewerAngles.rho = n_deg2rad(-130.0f);
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
        controlcamera.viewerPos = this->defViewerPos;
        controlcamera.viewerZoom = this->defViewerZoom;
        controlcamera.viewerAngles = this->defViewerAngles;
        this->ResetTestObjects();
    }

    // handle viewer move
    if (inputServer->GetButton("zoom"))
    {
        controlcamera.viewerPos -= vector3(0,0,1) * this->viewerVelocity * frameTime;
    }

    // handle viewer rotate
    float lookHori = 0.0f;
    float lookVert = 0.0f;
    if (inputServer->GetButton("zoom") || inputServer->GetButton("look"))
    {
        lookHori = inputServer->GetSlider("left") - inputServer->GetSlider("right");
        lookVert = inputServer->GetSlider("down") - inputServer->GetSlider("up");
    }
    const float lookVelocity = 0.25f;
    controlcamera.viewerAngles.theta -= lookVert * lookVelocity;
    controlcamera.viewerAngles.rho   += lookHori * lookVelocity;

    // apply changes
    controlcamera.viewerZoom = vector3(0.0f,0.0f,0.0f);
    matrix44 dummy;
    controlcamera.GenerateTransform(dummy);

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
nSDBViewerApp::InitOverlayGui()
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


// clear out the current objects and create a new set
void nSDBViewerApp::ResetTestObjects()
{
    // clear out any stuff already in there
    DeleteTestObjects();

    const int gridfreq = 6;
    const float gridsize = 30.0f;
    const int numtestobjects = gridfreq * gridfreq * gridfreq;

    if (!m_rootsector.isvalid())
        return;

    // make some test objects!
    for (int tix=0; tix < numtestobjects; tix++)
    {
        vector3 elementpos(  tix % gridfreq , (tix/gridfreq) % gridfreq, tix/(gridfreq*gridfreq) );
        elementpos -= vector3((gridfreq-1)/2.0,(gridfreq-1)/2.0,(gridfreq-1)/2.0);
        elementpos *= (gridsize / gridfreq);
        float elementradius = 0.5 * sqrt(3.0); // radius of a sphere enclosing a unit cube centered on teh origin

        SDBTestObject *newobject = new SDBTestObject;
        m_rootsector->AddElement(&(newobject->spatialinfo) );
        newobject->spatialinfo.Set(elementpos, elementradius);
        newobject->spatialinfo.SetPtr(newobject);
        MoveObject(newobject, elementpos);
        newobject->renderc.SetRootNode(this->refTestObjectNode.get());
        this->refTestObjectNode->RenderContextCreated(&(newobject->renderc));

        m_testobjects.Append(newobject);
    }

    // add two occluders for now
    for (int oix=0; oix < 2; oix++)
    {
        vector3 elementpos(oix * 8 - 4, oix * 4 - 4,0);
        float elementradius = 6.0;
        nSDBViewerApp::SDBTestObject *newobject = new SDBTestObject;
        m_rootsector->AddElement(&(newobject->occluderinfo) );
        newobject->occluderinfo.Set(elementpos, elementradius);
        newobject->occluderinfo.SetPtr(newobject);
        newobject->usingoccluder = true;
        MoveObject(newobject, elementpos);
        newobject->renderc.SetRootNode(this->refOccluderObjectNode.get());
        this->refOccluderObjectNode->RenderContextCreated(&(newobject->renderc));

        m_testobjects.Append(newobject);
    }
}

/// delete all test objects
void nSDBViewerApp::DeleteTestObjects()
{
    // for each element, we need to remove it from the sector and then
    // delete the nSpatialElement object
    for (nArray<SDBTestObject *>::iterator objectiter = m_testobjects.Begin();
                                         objectiter != m_testobjects.End();
                                         objectiter++)
    {
        SDBTestObject *curobject = *objectiter;
        this->refRootNode->RenderContextDestroyed(&(curobject->renderc));
        if (m_rootsector.isvalid())
        {
            if (curobject->usingoccluder)
                m_rootsector->RemoveElement(&(curobject->occluderinfo));
            else
                m_rootsector->RemoveElement(&(curobject->spatialinfo));
        }
        delete curobject;
    }

    m_testobjects.Clear();
}

/// mark objects with the three cameras
void nSDBViewerApp::UpdateObjectMarks()
{
//    markcameras[1].viewerAngles.rho += 0.005f;
//    markcameras[1].viewerAngles.theta += 0.01f;
    // clear out current marks
    for (nArray<SDBTestObject *>::iterator objectiter = m_testobjects.Begin();
                                           objectiter != m_testobjects.End();
                                           objectiter++)
    {
        (*objectiter)->spatialmarkflags = 0;
    }

    // create a visitor and generate a list of objects visible by camera 1
    if (m_frustumclip)
    {
        // create a viewmatrix for camera 0
        matrix44 cameraxform0;
        markcameras[m_viscamera].GenerateTransform(cameraxform0);
        nVisibleFrustumGenArray::VisibleElements visiblearray(100,100);
        nVisibleFrustumGenArray generator(this->camera, cameraxform0, visiblearray);

        generator.nVisibleFrustumVisitor::Visit(m_rootsector.get(),3);

        // mark all elements found
        nVisibleFrustumGenArray::VisibleElements::iterator viselement;
        for (viselement= visiblearray.Begin();
            viselement != visiblearray.End();
            viselement++)
        {
            SDBTestObject *curobject = (SDBTestObject *)((*viselement)->GetPtr());
            curobject->spatialmarkflags |= 1;
        }

        // clear out the list after we're done with it
        generator.Reset();
    }
    if (m_sphereclip)
    {
        // create a clipper
        sphere cameraviewsphere(markcameras[m_viscamera].viewerPos, 10.0f);
        nVisibleSphereGenArray::VisibleElements visiblearray(100,100);
        nVisibleSphereGenArray generator(cameraviewsphere, visiblearray);

        generator.nVisibleSphereVisitor::Visit(m_rootsector.get(),3);

        // mark all elements found
        nVisibleSphereGenArray::VisibleElements::iterator viselement;
        for (viselement= visiblearray.Begin();
            viselement != visiblearray.End();
            viselement++)
        {
            SDBTestObject *curobject = (SDBTestObject *)((*viselement)->GetPtr());
            curobject->spatialmarkflags |= 2;
        }

        // clear out the list after we're done with it
        generator.Reset();
    }
    if (m_occludingfrustumclip)
    {
        // create a clipper
        // create a viewmatrix for camera 0
        matrix44 cameraxform0;
        markcameras[m_viscamera].GenerateTransform(cameraxform0);
        nOccludingFrustumGenArray::VisibleElements visiblearray(100,100);
        nOccludingFrustumGenArray generator(this->camera, cameraxform0, visiblearray);

        generator.nOccludingFrustumVisitor::Visit(m_rootsector.get(),3);

        // mark all elements found
        nOccludingFrustumGenArray::VisibleElements::iterator viselement;
        for (viselement= visiblearray.Begin();
            viselement != visiblearray.End();
            viselement++)
        {
            SDBTestObject *curobject = (SDBTestObject *)((*viselement)->GetPtr());
            curobject->spatialmarkflags |= 4;
        }

        // clear out the list after we're done with it
        generator.Reset();
    }
}

void nSDBViewerApp::MoveObject(SDBTestObject *object, const vector3 &newpos)
{
    // update the object's render context and sector position
    float effectiveradius = 1.0;
    if (m_rootsector.isvalid())
    {
        // occluder or spatial element?
        if (object->usingoccluder)
        {
            // for an occluder we need to provide a conservative bounding box
            vector3 boxextent(vector3(1,1,1) * (object->occluderinfo.radius / sqrt(3.0f)) );
            bbox3 occluderbbox(newpos, boxextent);
            m_rootsector->UpdateElement(&(object->occluderinfo), newpos, occluderbbox);
            effectiveradius = boxextent.x;
        }
        else
        {
            m_rootsector->UpdateElement(&(object->spatialinfo), newpos, object->spatialinfo.radius);
            effectiveradius = object->spatialinfo.radius;
        }
    }
    matrix44 newtransform;
    newtransform.ident();
    newtransform.scale(vector3(1,1,1) * effectiveradius);
    newtransform.translate(newpos);
    object->renderc.SetTransform(newtransform);
}

