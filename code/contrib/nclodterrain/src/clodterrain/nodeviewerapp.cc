//------------------------------------------------------------------------------
//  nODEViewerApp.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "clodterrain/nodeviewerapp.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "misc/nwatched.h"
#include "gui/nguiwindow.h"
#include "gui/nguilabel.h"

#include "opende/nopendespheregeom.h"
#include "opende/nopendeboxgeom.h"
#include "opende/nopendeplanegeom.h"
#include "opende/nopendetrimeshgeom.h"

struct callbackstuff {
    dWorldID worldid;
    dJointGroupID contactgroupid;
};

// callback for handling collision contacts
static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
  int i,n;
  callbackstuff *callbackdata = (callbackstuff *)data;

  const int N = 30;
  dContact contact[N];
  n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
  if (n > 0) {
    for (i=0; i<n; i++) {
      contact[i].surface.mode = dContactSlip1 | dContactSlip2 |
          dContactSoftERP | dContactSoftCFM | dContactApprox1;
      contact[i].surface.mu = dInfinity;
      contact[i].surface.slip1 = 0.1f;
      contact[i].surface.slip2 = 0.1f;
      contact[i].surface.soft_erp = 0.5f;
      contact[i].surface.soft_cfm = 0.3f;
      dJointID c = dJointCreateContact (callbackdata->worldid,callbackdata->contactgroupid,&contact[i]);
      dJointAttach (c,
            dGeomGetBody(contact[i].geom.g1),
            dGeomGetBody(contact[i].geom.g2));
    }
  }
}




//------------------------------------------------------------------------------
/**
*/
nODEViewerApp::nODEViewerApp(nKernelServer* ks) :
    startupScript("home:code/contrib/nclodterrain/bin/startup.lua"),
    kernelServer(ks),
    isOpen(false),
    isOverlayEnabled(true),
    camera(60.0f, 4.0f/3.0f, 0.1f, 1000.0f),
    controlMode(Fly),
    defViewerPos(240.0f, 50.0f, 100.0f),
    featureSetOverride(nGfxServer2::InvalidFeatureSet),
//  defViewerAngles(n_deg2rad(90.0f), n_deg2rad(0.0f)),
    defViewerAngles(n_deg2rad(70.0f),n_deg2rad(80.0f)),
//  defViewerPos(300.0f, 2000.0f, 300.0f),
//  defViewerAngles(n_deg2rad(-80.0f), n_deg2rad(-180.0f)),
    defViewerZoom(0.0f, 0.0f, 9.0f),
    viewerPos(defViewerPos),
    viewerVelocity(50.0f),
    viewerAngles(defViewerAngles),
    viewerZoom(defViewerZoom),
    screenshotID(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nODEViewerApp::~nODEViewerApp()
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
nODEViewerApp::Open()
{
    n_assert(!this->isOpen);

    // initialize Nebula servers
    this->refScriptServer   = (nScriptServer*)    kernelServer->New(this->GetScriptServerClass(), "/sys/servers/script");
    this->refGfxServer      = (nGfxServer2*)      kernelServer->New("nd3d9server", "/sys/servers/gfx");
    this->refConServer      = (nConServer*)       kernelServer->New("nconserver", "/sys/servers/console");
    this->refResourceServer = (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
    this->refSceneServer    = (nSceneServer*)     kernelServer->New(this->GetSceneServerClass(), "/sys/servers/scene");
    this->refVarServer      = (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
    this->refAnimServer     = (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
    this->refParticleServer = (nParticleServer*)  kernelServer->New("nparticleserver", "/sys/servers/particle");
    this->refGuiServer      = (nGuiServer*)       kernelServer->New("nguiserver", "/sys/servers/gui");
    this->refShadowServer   = (nShadowServer2*)   kernelServer->New("nshadowserver2", "/sys/servers/shadow");

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
    this->kernelServer->GetRemoteServer()->Open("nodeviewer");

    // initialize graphics
    this->refGfxServer->SetDisplayMode(this->displayMode);

    // run startup script
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

    this->refGfxServer->SetCamera(this->camera);

    this->refSceneServer->SetOcclusionQuery(false);
    this->refSceneServer->SetObeyLightLinks(false);

    this->refGfxServer->OpenDisplay();

    nString renderpathpath(kernelServer->GetFileServer()->ManglePath("nclodshaders:renderpath.xml"));
    this->refSceneServer->SetRenderPathFilename(renderpathpath);

    this->refSceneServer->Open();

    // define the input mapping
    // late initialization of input server, because it relies on
    // refGfxServer->OpenDisplay having been called
    this->refInputServer    = (nInputServer*)     kernelServer->New("ndi8server", "/sys/servers/input");
    if (NULL != this->GetInputScript())
    {
        nString result;
        this->refScriptServer->RunScript(this->GetInputScript(), result);
    }

    // initialize gui
    this->refGuiServer->SetRootPath("/gui");
    this->refGuiServer->Open();
    if (this->isOverlayEnabled)
    {
        this->InitOverlayGui();
    }

    this->InitDynamics();  // must create the spaces for geoms to inhabit before running the scene script!

    if (this->GetSceneFile())
    {
        // load the stage (normally stdlight.lua)
        nString result;
        this->refScriptServer->RunScript("home:code/contrib/nclodterrain/bin/stdlight.lua", result);

        // load the object to look at
        kernelServer->PushCwd(this->refRootNode.get());
        kernelServer->Load(this->GetSceneFile());
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

    // make the joint group for contact joints
    this->contactgroup = nOpende::JointGroupCreate(20);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void nODEViewerApp::Close()
{
    n_assert(this->IsOpen());

    int thingix;
    for (thingix=0; thingix < NUMTHINGS; thingix++)
    {
        this->bodies[thingix]->Release();
        this->geoms[thingix]->Release();
    }

    this->planegeom->Release();

    this->refDySpace->Release();
    this->refDyWorld->Release();
    this->refDyServer->Release();

    this->refSceneServer->Close();

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
void nODEViewerApp::Run()
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
        this->TransferGlobalVariables();
        this->renderContext.SetFrameId(frameId++);

        if (!this->refGfxServer->InDialogBoxMode())
        {
            // render
            this->refSceneServer->BeginScene(viewMatrix);
            this->refSceneServer->Attach(&this->renderContext);

            // attach ode bodies - extract the transform from the body position/orientation and
            // put that into the rendercontext
            int geomix;
            for (geomix = 0; geomix < NUMTHINGS; geomix++)
            {
                vector3 pos = this->bodies[geomix]->GetPosition();
                quaternion quat = this->bodies[geomix]->GetQuaternion();
                matrix44 bodyxform(quat);
                bodyxform.translate(pos);
                this->bodyContext.SetTransform(bodyxform);
                this->refSceneServer->Attach(&this->bodyContext);
            }

            this->refSceneServer->EndScene();
            this->refSceneServer->RenderScene();             // renders the 3d scene
            //this->refGuiServer->Render();                    // do additional rendering before presenting the frame
            this->refConServer->Render();

            this->refSceneServer->PresentScene();            // present the frame
        }

        prevTime = time;

        // update watchers
        watchViewerPos->SetV4(vector4(viewMatrix.M41, viewMatrix.M42, viewMatrix.M43, n_rad2deg(this->viewerAngles.rho)));

        //
        // crank the world simulation
        //

        // first generate contact joints
        nOpende::JointGroupEmpty(this->contactgroup);
        struct callbackstuff stuff;
        stuff.worldid = this->refDyWorld->id;
        stuff.contactgroupid = this->contactgroup;

        // collide moving objects w/ each other, and then with the terrain space
        // note that we don't call Collide() on the terrain space, as we don't want
        // terrain chunks to collide with each other
        this->refDySpace->Collide(&stuff, &nearCallback);
        if (this->refTerrainSpace.isvalid())
        {
            this->refDySpace->Collide2(this->refTerrainSpace->nOpendeGeom::id(), &stuff, &nearCallback);
        }

        // update body positions and velocity
        this->refDyWorld->StepFast1(0.15f,10);

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
void nODEViewerApp::TransferGlobalVariables()
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
nODEViewerApp::HandleInput(float frameTime)
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
        filename.AppendInt(this->screenshotID++);
        filename.Append(".bmp");

        this->refGfxServer->SaveScreenshot(filename.Get());
    }
}

//------------------------------------------------------------------------------
/**
    Handle input for the Maya control model.
*/
void
nODEViewerApp::HandleInputMaya(float frameTime)
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

    // handle viewer reset
    if (reset)
    {
        this->viewerPos = this->defViewerPos;
        this->viewerZoom = this->defViewerZoom;
        this->viewerAngles = this->defViewerAngles;
        this->ResetTestObjects();
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
    this->viewMatrix.rotate_z(this->viewerAngles.rho);
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
nODEViewerApp::HandleInputFly(float frameTime)
{
    nInputServer* inputServer = this->refInputServer.get();
    if (frameTime <= 0.0001f)
    {
        frameTime = 0.0001f;
    }

    // set speed
    if (inputServer->GetButton("speed0")) this->viewerVelocity = 5.5f;
    if (inputServer->GetButton("speed1")) this->viewerVelocity = -5.0f;
    if (inputServer->GetButton("speed2")) this->viewerVelocity = 50.0f;

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
        this->ResetTestObjects();
    }

    // handle viewer move
    if (inputServer->GetButton("zoom"))
    {
        this->viewerPos -= this->viewMatrix.z_component() * this->viewerVelocity * frameTime;
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
    this->viewerAngles.theta -= lookVert * lookVelocity;
    this->viewerAngles.rho   += lookHori * lookVelocity;

    // apply changes
    this->viewMatrix.ident();
    this->viewMatrix.rotate_x(this->viewerAngles.theta);
    this->viewMatrix.rotate_z(this->viewerAngles.rho);
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
nODEViewerApp::InitOverlayGui()
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



/// setup dynamics objects
void nODEViewerApp::InitDynamics()
{
    // initialize ODE elements: world, space, geoms, and bodies
    this->refDyWorld = (nOpendeWorld *)kernelServer->New("nopendeworld","/usr/dynamics/world");
    vector3 gravity(0.0f,0.0f,-0.1f);
    this->refDyWorld->SetGravity(gravity);
    this->refDyWorld->SetERP(0.9f);
    this->refDyWorld->SetCFM(0.9f);
    this->refDySpace = (nOpendeSpace *)kernelServer->New("nopendehashspace","/usr/dynamics/ballisticspace");
    this->refDySpace->Create();
    this->refTerrainSpace = (nOpendeSpace *)kernelServer->New("nopendehashspace","/usr/dynamics/terrainspace");
    this->refTerrainSpace->Create();
    this->refDyServer = (nOpendeServer *)kernelServer->New("nopendeserver","/usr/dynamics/server");

    // load visnodes to mark ode bodies
    this->refSimplebody = (nTransformNode *)kernelServer->New("ntransformnode", "/usr/dynamics/odemarker");
    kernelServer->PushCwd(this->refSimplebody.get());
    kernelServer->Load("gfxlib:examples/torus.n2");
    kernelServer->PopCwd();

    // initialize ODE render context for ode bodies
    // initialize the main render context
    nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    nVariable::Handle timeHandle = this->refVarServer->GetVariableHandleByName("time");
    nVariable::Handle oneHandle  = this->refVarServer->GetVariableHandleByName("one");
    nVariable::Handle windHandle = this->refVarServer->GetVariableHandleByName("wind");
    this->bodyContext.AddVariable(nVariable(timeHandle, 0.5f));
    this->bodyContext.AddVariable(nVariable(oneHandle, 1.0f));
    this->bodyContext.AddVariable(nVariable(windHandle, wind));
    this->bodyContext.SetRootNode(this->refSimplebody.get());
    this->refSimplebody->RenderContextCreated(&this->bodyContext);

    int thingix;
    for (thingix=0; thingix < NUMTHINGS; thingix++)
    {
        nString bodypath = "/usr/dynamics/things/body";
        bodypath.AppendInt(thingix);
        nOpendeBody *thisbody = (nOpendeBody *)kernelServer->New("nopendebody",bodypath.Get());
        this->bodies[thingix] = thisbody;

        // initialize and position it somewhere useful
        thisbody->Create(this->refDyWorld->GetFullName().Get());
        thisbody->SetSphereMass(1.0,1.0);

        vector3 thisposition(41.0f+21.0f*thingix,5.0f+10.0f*sinf((float)thingix), 40.0f+5.0f*thingix);
        thisbody->SetPosition(thisposition);

        nString geompath = "/usr/dynamics/things/geom";
        geompath.AppendInt(thingix);
        {
            nOpendeBoxGeom *spheregeom = (nOpendeBoxGeom *)kernelServer->New("nopendeboxgeom",geompath.Get());
            spheregeom->Create(this->refDySpace->GetFullName().Get());
            //spheregeom->SetRadius(1.5);
            spheregeom->SetBody(bodypath.Get());
            spheregeom->SetLengths(4.2f,4.2f,4.2f);
            this->geoms[thingix] = spheregeom;
        }
    }

    this->ResetTestObjects();

    // add in a plane for them to fall onto
    nOpendePlaneGeom *planegeom = (nOpendePlaneGeom *)kernelServer->New("nopendeplanegeom","/usr/dynamics/things/plane");
    planegeom->Create(this->refTerrainSpace->GetFullName().Get());
    planegeom->SetParams(0.f,0.f,1.f,-20.f);
    this->planegeom = planegeom;
//  this->refDySpace->AddGeom(planegeom->id());
}

/// reset object positions
void nODEViewerApp::ResetTestObjects()
{
    // reset all the thing positions
    for (int thingix=0; thingix < NUMTHINGS; thingix++)
    {
        vector3 thisposition(41.0f+21.0f*thingix,35.0f+40.0f*thingix, 55.0f+20.0f*thingix);
        vector3 zerovelocity(0,0,0);
        quaternion q(0,0,0,1);
        this->bodies[thingix]->SetPosition(thisposition);
        this->bodies[thingix]->SetQuaternion(q);
        this->bodies[thingix]->SetLinearVel(zerovelocity);
        this->bodies[thingix]->SetAngularVel(zerovelocity);

    }
}
