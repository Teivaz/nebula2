//------------------------------------------------------------------------------
//  nODEViewerApp.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "clodterrain/nodeviewerapp.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "misc/nwatched.h"

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
    kernelServer(ks),
    isOpen(false),
    camera(60.0f, 4.0f/3.0f, 0.1f, 1000.0f),
    controlMode(Fly),
    defViewerPos(240.0f, 50.0f, 100.0f),
//  defViewerAngles(n_deg2rad(90.0f), n_deg2rad(0.0f)),
    defViewerAngles(n_deg2rad(70.0f),n_deg2rad(80.0f)),
//  defViewerPos(300.0f, 2000.0f, 300.0f),
//  defViewerAngles(n_deg2rad(-80.0f), n_deg2rad(-180.0f)),
    defViewerZoom(0.0f, 0.0f, 9.0f),
    viewerPos(defViewerPos),
    viewerVelocity(50.0f),
    viewerAngles(defViewerAngles),
    viewerZoom(defViewerZoom)
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
    this->refScriptServer   = (nScriptServer*)    kernelServer->New("nluaserver", "/sys/servers/script");
    this->refGfxServer      = (nGfxServer2*)      kernelServer->New("nd3d9server", "/sys/servers/gfx");
    this->refInputServer    = (nInputServer*)     kernelServer->New("ndi8server", "/sys/servers/input");
    this->refConServer      = (nConServer*)       kernelServer->New("nconserver", "/sys/servers/console");
    this->refResourceServer = (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
    this->refSceneServer    = (nSceneServer*)     kernelServer->New("nstdsceneserver", "/sys/servers/scene");
    this->refVarServer      = (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
    this->refAnimServer     = (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
    this->refParticleServer = (nParticleServer*)  kernelServer->New("nparticleserver", "/sys/servers/particle");

    // initialize the proj: assign
    if (this->GetProjDir())
    {
        kernelServer->GetFileServer()->SetAssign("proj", this->GetProjDir());
    }
    else
    {
        kernelServer->GetFileServer()->SetAssign("proj", kernelServer->GetFileServer()->GetAssign("home"));
    }

    // define the simple input mapping
    this->DefineInputMapping();

    // create scene graph root node
    this->refRootNode = (nTransformNode*) kernelServer->New("ntransformnode",  "/usr/scene");

    // open the remote port
    this->kernelServer->GetRemoteServer()->Open("nodeviewer");

    // initialize graphics
    this->refGfxServer->SetDisplayMode(this->displayMode);
    this->refGfxServer->SetCamera(this->camera);
    this->refGfxServer->OpenDisplay();

    // load the default Nebula startup.tcl script
    const char* result;
    this->refScriptServer->RunScript("home:code/contrib/nclodterrain/bin/startup.lua", result);

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

    this->InitDynamics();

    // make the joint group for contact joints
    this->contactgroup = nOpende::JointGroupCreate(20);

    // run scripts
    if (this->GetStartupScript())
    {
        const char* result;
        this->refScriptServer->RunScript(this->GetStartupScript(), result);
    }
    if (this->GetSceneFile())
    {
        // load the stage (normally stdlight.tcl)
        const char* result;
        this->refScriptServer->RunScript("home:code/contrib/nclodterrain/bin/stdlight.lua", result);

        // load the object to look at
        kernelServer->PushCwd(this->refRootNode.get());
        kernelServer->Load(this->GetSceneFile());
        kernelServer->PopCwd();
    }

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

    this->refGfxServer->CloseDisplay();

    this->refRootNode->Release();
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
        if (Maya == this->controlMode)
        {
            this->HandleInputMaya(frameTime);
        }
        else
        {
            this->HandleInputFly(frameTime);
        }
        this->refInputServer->FlushEvents();

        // update render context variables
        this->renderContext.GetVariable(timeHandle)->SetFloat((float)time);
        this->TransferGlobalVariables();
        this->renderContext.SetFrameId(frameId++);

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
        this->refConServer->Render();                    // do additional rendering before presenting the frame

        this->refSceneServer->PresentScene();            // present the frame

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

        // sleep for a very little while because we
        // are multitasking friendly
        n_sleep(0.005);
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
/*
    Define the input mapping.
*/
void
nODEViewerApp::DefineInputMapping()
{
    nInputServer* inputServer = this->refInputServer.get();
    inputServer->BeginMap();
    inputServer->Map("keyb0:space.down",        "reset");
    inputServer->Map("keyb0:esc.down",          "console");
    inputServer->Map("relmouse0:btn0.pressed",  "look");    
    inputServer->Map("relmouse0:btn1.pressed",  "zoom");
    inputServer->Map("relmouse0:btn2.pressed",  "pan");
    inputServer->Map("relmouse0:-x",            "left");
    inputServer->Map("relmouse0:+x",            "right");
    inputServer->Map("relmouse0:-y",            "up");
    inputServer->Map("relmouse0:+y",            "down");
    inputServer->Map("relmouse0:-z",            "zoomIn");
    inputServer->Map("relmouse0:+z",            "zoomOut");
    inputServer->Map("keyb0:f1.down",           "mayacontrols");
    inputServer->Map("keyb0:f2.down",           "flycontrols");
    inputServer->Map("keyb0:1.down",            "speed0");
    inputServer->Map("keyb0:2.down",            "speed1");
    inputServer->Map("keyb0:3.down",            "speed2");
    inputServer->Map("keyb0:f5.down",           "setpos0");
    inputServer->Map("keyb0:f6.down",           "setpos1");
    inputServer->Map("keyb0:f7.down",           "setpos2");
    inputServer->Map("keyb0:f8.down",           "setpos3");
    inputServer->EndMap();
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
    kernelServer->Load("home:export/gfxlib/examples/torus.n2");
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
    char bufferthing[200];
    for (thingix=0; thingix < NUMTHINGS; thingix++)
    {
        nString bodypath = "/usr/dynamics/things/body";
        bodypath += nString(thingix);
        nOpendeBody *thisbody = (nOpendeBody *)kernelServer->New("nopendebody",bodypath.Get());
        this->bodies[thingix] = thisbody;

        // initialize and position it somewhere useful
        thisbody->Create(this->refDyWorld->GetFullName(bufferthing,200));
        thisbody->SetSphereMass(1.0,1.0);

        vector3 thisposition(41.0f+21.0f*thingix,5.0f+10.0f*sinf((float)thingix), 40.0f+5.0f*thingix);
        thisbody->SetPosition(thisposition);

        nString geompath = "/usr/dynamics/things/geom";
        geompath += nString(thingix);
        {
            nOpendeBoxGeom *spheregeom = (nOpendeBoxGeom *)kernelServer->New("nopendeboxgeom",geompath.Get());
            spheregeom->Create(this->refDySpace->GetFullName(bufferthing,200));
            //spheregeom->SetRadius(1.5);
            spheregeom->SetBody(bodypath.Get());
            spheregeom->SetLengths(4.0,4.0,4.0);
            this->geoms[thingix] = spheregeom;
        }
    }

    this->ResetTestObjects();

    // add in a plane for them to fall onto
    nOpendePlaneGeom *planegeom = (nOpendePlaneGeom *)kernelServer->New("nopendeplanegeom","/usr/dynamics/things/plane");
    planegeom->Create(this->refTerrainSpace->GetFullName(bufferthing,200));
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
