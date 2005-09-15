//------------------------------------------------------------------------------
//  physdemoapp.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physdemo/physdemoapp.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "misc/nwatched.h"

//------------------------------------------------------------------------------
/**
*/
PhysDemoApp::PhysDemoApp(nKernelServer* ks) :
    kernelServer(ks),
    isOpen(false),
    featureSetOverride(nGfxServer2::InvalidFeatureSet),
    defViewerPos(0.0f, 0.0f, 0.0f),
    defViewerAngles(0.0f, 0.0f),
    defViewerZoom(0.0f, 0.0f, 9.0f),
    viewerPos(defViewerPos),
    viewerVelocity(500.0f),
    fpsCutoff(0.066f),
    viewerAngles(defViewerAngles),
    viewerZoom(defViewerZoom),
    screenshotID(0),
    fontSize(20),
    objectID(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PhysDemoApp::~PhysDemoApp()
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
PhysDemoApp::Open()
{
    nString result;

    n_assert(!this->isOpen);
    n_assert(this->GetInputScript());

    // initialize Nebula servers
    this->refScriptServer   = (nScriptServer*)    kernelServer->New("ntclserver", "/sys/servers/script");
    this->refGfxServer      = (nGfxServer2*)      kernelServer->New("nd3d9server", "/sys/servers/gfx");
    this->refConServer      = (nConServer*)       kernelServer->New("nconserver", "/sys/servers/console");
    this->refResourceServer = (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
    this->refSceneServer    = (nSceneServer*)     kernelServer->New("nsceneserver", "/sys/servers/scene");
    this->refVarServer      = (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
    this->refAnimServer     = (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
    this->refParticleServer = (nParticleServer*)  kernelServer->New("nparticleserver", "/sys/servers/particle");
    this->refGuiServer      = (nGuiServer*)       kernelServer->New("nguiserver", "/sys/servers/gui");
    this->refShadowServer   = (nShadowServer2*)   kernelServer->New("nshadowserver2", "/sys/servers/shadow");

    //  Set the proj: assign
    nFileServer2*   fs = static_cast<nFileServer2*>(kernelServer->Lookup("/sys/servers/file2"));
    fs->SetAssign("proj", "home:code/contrib/physdemo");
    
    // run the startup script
    this->refScriptServer->RunScript("proj:bin/startup.tcl", result);

    // set the gfx server feature set override
    if (this->featureSetOverride != nGfxServer2::InvalidFeatureSet)
    {
        this->refGfxServer->SetFeatureSetOverride(this->featureSetOverride);
    }

    // initialize the physics-related classes
    this->refPhysWorld      = (nOpendeWorld*)    kernelServer->New("nopendeworld", "/phys/world");
    this->refPhysWorld->SetGravity(vector3(0.0f, -9.8f, 0.0f));
    this->refPhysWorld->SetQuickStepNumIterations(PHYS_QUICKSTEP_ITERS);
    this->refPhysWorld->SetAutoDisableFlag(true);
    this->refPhysWorld->SetAutoDisableLinearThreshold(PHYS_LINEAR_VEL_THRESHOLD);
    this->refPhysWorld->SetAutoDisableAngularThreshold(PHYS_ANGULAR_VEL_THRESHOLD);
    this->refPhysWorld->SetAutoDisableSteps(PHYS_VEL_THRESHOLD_TIMEOUT);
    this->refPhysColSpace   = (nOpendeHashSpace*)    kernelServer->New("nopendehashspace", "/phys/world/space");
    this->refPhysColSpace->Create();

    // Create the contact joint group and array (and set some values for the contact joints, since in this sim, every surface is treated identically)
    this->physColJointGroupId = nOpende::JointGroupCreate(0);
    this->physContactArray = new dContact[PHYS_MAX_CONTACTS];

    for (int index = 0; index < PHYS_MAX_CONTACTS; index++)
    {
        this->physContactArray[index].surface.mode = dContactApprox1;
        this->physContactArray[index].surface.mu = 0.75;
    }
    
    // open the remote port
    this->kernelServer->GetRemoteServer()->Open("physdemo");

    // initialize graphics
    this->refGfxServer->SetDisplayMode(this->displayMode);
    this->camera.SetFarPlane(500.0f);
    this->refGfxServer->SetCamera(this->camera);

    // open the scene server
    if (!this->refSceneServer->Open())
    {
        n_error("PhysDemoApp::Open(): Failed to open nSceneServer!");
        return false;
    }

    // define the input mapping
    // late initialization of input server, because it relies on 
    // refGfxServer->OpenDisplay having been called
    this->refInputServer    = (nInputServer*)     kernelServer->New("ndi8server", "/sys/servers/input");
    this->refInputServer->Open();
    this->refScriptServer->RunScript(this->GetInputScript(), result);

    // initialize gui
    this->refGuiServer->SetRootPath("/gui");
    this->refGuiServer->Open();
    this->InitOverlayGui();

    this->timeHandle = this->refVarServer->GetVariableHandleByName("time");
    this->windHandle = this->refVarServer->GetVariableHandleByName("wind");

    // create a nroot to hold all objects
    kernelServer->New("nroot", "/objects");

    // create the scene nodes that will be used to represent the objects.
    // these scene nodes will be shared so every box on screen is actually using
    // the same scene node.
    kernelServer->LoadAs("shapes:box/box.n2", "/scenenodes/box");
    kernelServer->LoadAs("shapes:sphere/sphere.n2", "/scenenodes/sphere");
    kernelServer->LoadAs("shapes:sphere/bigsphere.n2", "/scenenodes/bigsphere");
    kernelServer->LoadAs("shapes:big_flat_plane.n2", "/scenenodes/floor");

    // create a single default light
    nTransformNode *lightNode = (nTransformNode *)kernelServer->LoadAs("lights:point_lights/simple_light.n2", "/scenenodes/default_light");
    lightNode->RenderContextCreated(&this->lightRenderContext);
    lightRenderContext.SetRootNode(lightNode);
    
    // create the floor for the physical world
    this->CreateFloor(0.0f, -5.0f, 0.0f);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysDemoApp::Close()
{
    n_assert(this->IsOpen());

    // kill the GUI server before closing the gfx server
    this->refGuiServer->Close();
    // disable any possibility of future rendering
    this->refGfxServer->CloseDisplay();
    
    // clear the objects
    kernelServer->Lookup("/objects")->Release();

    // clear the scene nodes
    kernelServer->Lookup("/scenenodes")->Release();

    // Destroy the collision joint group
    nOpende::JointGroupDestroy(this->physColJointGroupId);

    // Destroy the contact joint array used by the collision callback
    delete(this->physContactArray);

    // Finally, move on to releasing the servers/etc
    this->refShadowServer->Release();
    this->refParticleServer->Release();
    this->refAnimServer->Release();
    this->refVarServer->Release();
    this->refSceneServer->Release();    
    this->refInputServer->Release();
    this->refGfxServer->Release();
    this->refScriptServer->Release();
    this->refResourceServer->Release();
    this->refConServer->Release();

    this->refPhysColSpace->Release();
    this->refPhysWorld->Release();

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysDemoApp::Run()
{
    nWatched watchViewerPos("viewerPos", nArg::Float4);

    // run the render loop
    bool running = true;
    nTime prevTime = 0.0;
    uint frameId = 0;
    float physTime = 0;
    while (this->refGfxServer->Trigger() && running)
    {
        nTime time = kernelServer->GetTimeServer()->GetTime();
        if (prevTime == 0.0)
        {
            prevTime = time;
        }
        float frameTime = (float) (time - prevTime);

        // This code right here prevents the FPS from dropping below a certain level.  If the FPS
        // does drop below that level, the entire world enters a sort of slow-mo mode until the
        // conditions causing such extreme drop in perf correct themselves.
        if (frameTime > this->fpsCutoff)
        {
            n_printf("Throttling frame time - cascade conditions detected\n");
            frameTime = this->fpsCutoff;
        }

        this->currFPS = 1.0f / frameTime;

        // Update the GUI's concept of time
        this->refGuiServer->SetTime(time);
        // Also update the GUI FPS element
        char guiString[256];
        sprintf(guiString, "FPS: %.1f", this->currFPS);
        this->guiFPSLabel->SetText(guiString);

        // Update the GUI's CFM label
        sprintf(guiString, "CFM: %.5f", this->refPhysWorld->GetCFM());
        this->guiCFMLabel->SetText(guiString);

        // Update the GUI's ERP label
        sprintf(guiString, "ERP: %.5f", this->refPhysWorld->GetERP());
        this->guiERPLabel->SetText(guiString);

        // Update the GUI's FPS LockP label
        sprintf(guiString, "FPS Lock: %.1f", 1.0f / this->fpsCutoff);
        this->guiFPSCutoffLabel->SetText(guiString);

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

        // flush the events, we're done with them.
        this->refInputServer->FlushEvents();

        // Update the physical world
        physTime += frameTime;
        this->UpdatePhysWorld(physTime);

        // Render the world
        this->RenderWorld(time, frameId);

        prevTime = time;

        // update watchers
        watchViewerPos->SetV4(vector4(viewMatrix.M41, viewMatrix.M42, viewMatrix.M43, n_rad2deg(this->viewerAngles.rho)));

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
PhysDemoApp::HandleInput(float frameTime)
{
    nInputServer* inputServer = this->refInputServer.get();

    // Calculate the camera's actual position
    vector3 cameraPos(this->viewerZoom);
    cameraPos.rotate(vector3(1, 0, 0), this->viewerAngles.theta);
    cameraPos.rotate(vector3(0, 1, 0), this->viewerAngles.rho);
    cameraPos += this->viewerPos;
    
    this->HandleMovementInput(frameTime);

    if (inputServer->GetButton("screenshot"))
    {
        nString filename;
        
        filename = "physdemo";

        filename.AppendInt(this->screenshotID++);
        filename.Append(".bmp");

        this->refGfxServer->SaveScreenshot(filename.Get());
    }

    // Here starts our physically-related buttons
    
    // Create a box at a random location
    if (inputServer->GetButton("create_box"))
    {
        this->CreateBox((rand()%1000)/200.0f - 2.5f, 10.0f, (rand()%1000)/200.0f - 2.5f);
    }
    // Create a sphere at a random location
    if (inputServer->GetButton("create_sphere"))
    {
        this->CreateSphere((rand()%1000)/200.0f - 2.5f, 10.0f, (rand()%1000)/200.0f - 2.5f);
    }
    // Create a big sphere at a random location
    if (inputServer->GetButton("create_big_sphere"))
    {
        this->CreateBigSphere((rand()%1000)/200.0f - 2.5f, 10.0f, (rand()%1000)/200.0f - 2.5f);
    }
    // Create a stack of boxes at a random location
    if (inputServer->GetButton("create_stack"))
    {
        float x = (rand()%1000)/200.0f - 2.5f;
        float z = (rand()%1000)/200.0f - 2.5f;

        this->CreateBox(x, -4.1f, z);
        this->CreateBox(x, -3.2f, z);
        this->CreateBox(x, -2.25f, z);
    }
    // Drop a box from the camera
    if (inputServer->GetButton("drop_box"))
    {
        this->CreateBox(cameraPos.x, cameraPos.y, cameraPos.z);
    }
    // Drop a sphere from the camera
    if (inputServer->GetButton("drop_sphere"))
    {
        this->CreateSphere(cameraPos.x, cameraPos.y, cameraPos.z);
    }
    // Drop a stack from the camera
    if (inputServer->GetButton("drop_stack"))
    {
        float x = cameraPos.x;
        float z = cameraPos.z;

        this->CreateBox(x, -4.6f, z);
        this->CreateBox(x, -3.55f, z);
        this->CreateBox(x, -2.5f, z);
    }
    // Create a straight stack line on the floor, using the camera's position as a reference.
    if (inputServer->GetButton("create_stack_line"))
    {
        float x = cameraPos.x;
        float z = cameraPos.z;

        if (x > -10 && x < 10)
        {
            for (float index = -10; index < 10.0f; index += 1.8f)
            {
                this->CreateBox(x, -4, index, true);
                this->CreateBox(x, -3, index, true);
                this->CreateBox(x, -2, index, true);
            }
        }
        if (z > -10 && z < 10)
        {
            for (float index = -10; index < 10.0f; index += 1.8f)
            {
                this->CreateBox(index, -4, z, true);
                this->CreateBox(index, -3, z, true);
                this->CreateBox(index, -2, z, true);
            }
        }
    }
    // Create a wall aligned with the view's position
    if (inputServer->GetButton("create_wall"))
    {
        float x = cameraPos.x;
        float z = cameraPos.z;

        int height = 1;

        if (x > -10 && x < 10)
        {
            for (float index = -10; index < 10.0f; index += 1.0f)
            {
                // Create these boxes as disabled.  This is how you'd do it in a game, with stacks
                // this big, and the player is almost never going to be capable of building something
                // like this on his own.
                for (float height_index = 0; height_index < height; height_index++)
                    this->CreateBox(x, height_index - 4, index, true);

                n_printf("height: %i\n", height);

                if (index < 0)
                    height++;
                else
                    height--;
            }
        }
        else if (z > -10 && z < 10)
        {
            for (float index = -10; index < 10.0f; index += 1.0f)
            {
                for (float height_index = 0; height_index < height; height_index++)
                    this->CreateBox(index, height_index - 4, z);

                if (index < 0)
                    height++;
                else
                    height--;
            }
        }
    }
    // Fire a super-massive bullet from the camera in the direction the camera is looking
    if (inputServer->GetButton("fire_bullet"))
    {
        SimpleObject *obj = this->CreateBullet(cameraPos.x, cameraPos.y, cameraPos.z);

        vector3 forceVec(0, 0, -1.0f);
        forceVec.rotate(vector3(1, 0, 0), this->viewerAngles.theta);
        forceVec.rotate(vector3(0, 1, 0), this->viewerAngles.rho);

        obj->refPhysBody->AddForce(forceVec * 100000.0f);
    }
    // Create a floor-clearing explosion
    if (inputServer->GetButton("kaboom"))
    {
        this->CreateExplosion((rand()%1000)/200.0f - 2.5f, -4.0f, (rand()%1000)/200.0f - 2.5f, 30000.0, true);
    }
    // Increase the framerate cut-off
    if (inputServer->GetButton("fps_cutoff_down"))
    {
        this->fpsCutoff += 0.0005f;
    }
    // Decrease the framerate cut-off
    if (inputServer->GetButton("fps_cutoff_up"))
    {
        if (this->fpsCutoff - 0.0005f >= 0.0f)
            this->fpsCutoff -= 0.0005f;
    }
    // Increase the global CFM
    if (inputServer->GetButton("cfm_up"))
    {
        this->fpsCutoff = this->fpsCutoff + 0.0001f;
    }
    // Decrease the global CFM
    if (inputServer->GetButton("cfm_down"))
    {
        if (this->refPhysWorld->GetCFM() - 0.0001f >= 0.0f)
            this->refPhysWorld->SetCFM(this->refPhysWorld->GetCFM() - 0.0001f);
    }
    // Increase the global ERP
    if (inputServer->GetButton("erp_up"))
    {
        this->refPhysWorld->SetERP(this->refPhysWorld->GetERP() + 0.0001f);
    }
    // Decrease the global ERP
    if (inputServer->GetButton("erp_down"))
    {
        if (this->refPhysWorld->GetERP() - 0.0001 >= 0.0)
            this->refPhysWorld->SetERP(this->refPhysWorld->GetERP() - 0.0001f);
    }
}

//------------------------------------------------------------------------------
/**
    Handle the movement input
*/
void
PhysDemoApp::HandleMovementInput(float frameTime)
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
}

//------------------------------------------------------------------------------
/**
    Create the base physical "floor"
*/
SimpleObject *
PhysDemoApp::CreateFloor(float x, float y, float z)
{
    // create unique name for this object
    nString name = "floor";
    name.AppendInt(this->objectID++);

    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", name.Get());
    kernelServer->PopCwd();

    // Find the scene node to use with this object
    nSceneNode *scenenode = (nSceneNode*)kernelServer->Lookup("/scenenodes/floor");
    
    // Set the scene node in the render context
    newObj->renderContext.SetRootNode(scenenode);
    
    // update the render context variables
    newObj->renderContext.AddVariable(nVariable(this->timeHandle, 0.0f));
    nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    newObj->renderContext.AddVariable(nVariable(this->windHandle, wind));
    
    // tell the scene node that it is being used by (another) render context
    scenenode->RenderContextCreated(&newObj->renderContext);

    // set the position
    newObj->Transform.settranslation(vector3(x, y, z));

    // Now create the physical representation
    // (the floor has no body, only a geom, which is connected to body 0, "the world")
    
    // The physics nodes are created as children of the curren object.
    kernelServer->PushCwd(newObj);
    nOpendeBoxGeom *physGeom = (nOpendeBoxGeom *)kernelServer->New("nopendeboxgeom", "physgeom");
    
    n_assert(physGeom);
    
    physGeom->Create("/phys/world/space");
    physGeom->SetPosition(vector3(x, y, z));
    physGeom->SetLengths(20.0f, 1.0f, 20.0f);
    physGeom->SetBody((dBodyID)0);
    newObj->refPhysGeom = physGeom;    

    // pop the Cwd
    kernelServer->PopCwd();

    return newObj;
}

//------------------------------------------------------------------------------
/**
    Create a box
*/
SimpleObject *
PhysDemoApp::CreateBox(float x, float y, float z, bool createDisabled)
{
    // create unique name for this object
    nString name = "box";
    name.AppendInt(this->objectID++);

    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", name.Get());
    kernelServer->PopCwd();

    // Find the scene node to use with this object
    nSceneNode *scenenode = (nSceneNode*)kernelServer->Lookup("/scenenodes/box");
    
    // Set the scene node in the render context
    newObj->renderContext.SetRootNode(scenenode);
    
    // update the render context variables
    newObj->renderContext.AddVariable(nVariable(this->timeHandle, 0.0f));
    nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    newObj->renderContext.AddVariable(nVariable(this->windHandle, wind));
    
    // tell the scene node that it is being used by (another) render context
    scenenode->RenderContextCreated(&newObj->renderContext);

    // set the position
    newObj->Transform.settranslation(vector3(x, y, z));
    newObj->renderContext.SetTransform(newObj->Transform.getmatrix());

    // Now create the physical representation
    // The physics nodes are created as children of the curren object.
    kernelServer->PushCwd(newObj);
    newObj->refPhysBody = (nOpendeBody *)kernelServer->New("nopendebody", "physbody");
    newObj->refPhysBody->Create("/phys/world");
    newObj->refPhysBody->SetPosition(vector3(x, y, z));
    newObj->refPhysBody->SetBoxMass(10.0f, 1.0f, 1.0f, 1.0f);

    nOpendeBoxGeom *physGeom = (nOpendeBoxGeom *)kernelServer->New("nopendeboxgeom", "physgeom");
    physGeom->Create("/phys/world/space");
    physGeom->SetLengths(1.0f, 1.0f, 1.0f);
    physGeom->SetBody(newObj->refPhysBody->id);
    physGeom->SetData((void *)newObj);
    newObj->refPhysGeom = physGeom;

    if (createDisabled)
        newObj->refPhysBody->Disable();

    // Finished defining objects, pop the Cwd
    kernelServer->PopCwd();

    return newObj;
}

//------------------------------------------------------------------------------
/**
    Create a sphere
*/
SimpleObject *
PhysDemoApp::CreateSphere(float x, float y, float z)
{
    
    // create unique name for this object
    nString name = "sphere";
    name.AppendInt(this->objectID++);

    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", name.Get());
    kernelServer->PopCwd();

    // Find the scene node to use with this object
    nSceneNode *scenenode = (nSceneNode*)kernelServer->Lookup("/scenenodes/sphere");
    
    // Set the scene node in the render context
    newObj->renderContext.SetRootNode(scenenode);
    
    // update the render context variables
    newObj->renderContext.AddVariable(nVariable(this->timeHandle, 0.0f));
    nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    newObj->renderContext.AddVariable(nVariable(this->windHandle, wind));
    
    // tell the scene node that it is being used by (another) render context
    scenenode->RenderContextCreated(&newObj->renderContext);

    // set the position
    newObj->Transform.settranslation(vector3(x, y, z));

    // Now create the physical representation
    // The physics nodes are created as children of the curren object.
    kernelServer->PushCwd(newObj);
    newObj->refPhysBody = (nOpendeBody *)kernelServer->New("nopendebody", "physbody");
    newObj->refPhysBody->Create("/phys/world");
    newObj->refPhysBody->SetPosition(vector3(x, y, z));
    newObj->refPhysBody->SetSphereMass(10.0f, 0.5f);

    nOpendeSphereGeom *physGeom = (nOpendeSphereGeom *)kernelServer->New("nopendespheregeom", "physgeom");
    physGeom->Create("/phys/world/space");
    physGeom->SetRadius(0.5f);
    physGeom->SetBody(newObj->refPhysBody->id);
    physGeom->SetData((void *)newObj);
    newObj->refPhysGeom = physGeom;

    // Finished defining objects, pop the Cwd
    kernelServer->PopCwd();

    return newObj;
}

//------------------------------------------------------------------------------
/**
    Create a big sphere
*/
SimpleObject *
PhysDemoApp::CreateBigSphere(float x, float y, float z)
{
    // create unique name for this object
    nString name = "bigsphere";
    name.AppendInt(this->objectID++);

    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", name.Get());
    kernelServer->PopCwd();

    // Find the scene node to use with this object
    nSceneNode *scenenode = (nSceneNode*)kernelServer->Lookup("/scenenodes/bigsphere");
    
    // Set the scene node in the render context
    newObj->renderContext.SetRootNode(scenenode);
    
    // update the render context variables
    newObj->renderContext.AddVariable(nVariable(this->timeHandle, 0.0f));
    nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    newObj->renderContext.AddVariable(nVariable(this->windHandle, wind));
    
    // tell the scene node that it is being used by (another) render context
    scenenode->RenderContextCreated(&newObj->renderContext);

    // set the position
    newObj->Transform.settranslation(vector3(x, y, z));
    
    // Now create the physical representation
    // The physics nodes are created as children of the curren object.
    kernelServer->PushCwd(newObj);
    newObj->refPhysBody = (nOpendeBody *)kernelServer->New("nopendebody", "physbody");
    newObj->refPhysBody->Create("/phys/world");
    newObj->refPhysBody->SetPosition(vector3(x, y, z));
    newObj->refPhysBody->SetSphereMass(10.0f, 1.0f);

    nOpendeSphereGeom *physGeom = (nOpendeSphereGeom *)kernelServer->New("nopendespheregeom", "physgeom");
    physGeom->Create("/phys/world/space");
    physGeom->SetRadius(1.0f);
    physGeom->SetBody(newObj->refPhysBody->id);
    physGeom->SetData((void *)newObj);
    newObj->refPhysGeom = physGeom;

    // Finished defining objects, pop the Cwd
    kernelServer->PopCwd();

    return newObj;
}

//------------------------------------------------------------------------------
/**
    Create a bullet (an INCREDIBLY massive sphere)
*/
SimpleObject *
PhysDemoApp::CreateBullet(float x, float y, float z)
{
    // create unique name for this object
    nString name = "bullet";
    name.AppendInt(this->objectID++);

    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", name.Get());
    kernelServer->PopCwd();

    // Find the scene node to use with this object
    nSceneNode *scenenode = (nSceneNode*)kernelServer->Lookup("/scenenodes/sphere");
    
    // Set the scene node in the render context
    newObj->renderContext.SetRootNode(scenenode);
    
    // update the render context variables
    newObj->renderContext.AddVariable(nVariable(this->timeHandle, 0.0f));
    nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    newObj->renderContext.AddVariable(nVariable(this->windHandle, wind));
    
    // tell the scene node that it is being used by (another) render context
    // this gives the scene nodes(s) a chance to create any per-instance data
    scenenode->RenderContextCreated(&newObj->renderContext);

    // set the position
    newObj->Transform.settranslation(vector3(x, y, z));

    // Now create the physical representation
    // The physics nodes are created as children of the current object.
    kernelServer->PushCwd(newObj);
    newObj->refPhysBody = (nOpendeBody *)kernelServer->New("nopendebody", "physbody");
    newObj->refPhysBody->Create("/phys/world");
    newObj->refPhysBody->SetPosition(vector3(x, y, z));
    newObj->refPhysBody->SetSphereMass(100.0f, 0.5f);

    nOpendeSphereGeom *physGeom = (nOpendeSphereGeom *)kernelServer->New("nopendespheregeom", "physgeom");
    physGeom->Create("/phys/world/space");
    physGeom->SetRadius(0.5f);
    physGeom->SetBody(newObj->refPhysBody->id);
    physGeom->SetData((void *)newObj);
    newObj->refPhysGeom = physGeom;

    // Finished defining objects, pop the Cwd
    kernelServer->PopCwd();

    // Let's give the bullet some floaty text
    this->kernelServer->PushCwd(this->refGuiServer->GetRootWindowPointer());

    newObj->refFloatyText = (nGuiTextLabel *) kernelServer->New("nguitextlabel", name.Get());
    n_assert(newObj->refFloatyText.isvalid());
    newObj->refFloatyText->SetText(name.Get());
    newObj->refFloatyText->SetAlignment(nGuiTextLabel::Center);
    newObj->refFloatyText->SetColor(vector4(1.0f, 0.0f, 0.0f, 1.0f));
    newObj->refFloatyText->SetFont("physDefaultFont");

    newObj->textWidth = 0.15f;
    newObj->textHeight = 0.05f;

    this->kernelServer->PopCwd();

    return newObj;
}

//------------------------------------------------------------------------------
/**
    Create an explosion
*/
void
PhysDemoApp::CreateExplosion(float x, float y, float z, float force, bool enableObjects)
{
    nRoot* objects = kernelServer->Lookup("/objects");
    SimpleObject* curObj;
    for (curObj = (SimpleObject*) objects->GetHead();
         curObj;
         curObj = (SimpleObject*) curObj->GetSucc())
    {
        if (curObj->refPhysBody.isvalid())
        {
            vector3 explosionVector = curObj->refPhysBody->GetPosition() - vector3(x, y, z);
            explosionVector.norm();
            explosionVector = explosionVector * force;
    
            curObj->refPhysBody->AddForceAtPos(explosionVector, vector3(x, y, z));

            if (enableObjects)
                curObj->refPhysBody->Enable();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Collision resolution callback
*/
void PhysDemoApp::PhysCollisionCallback(void *data, dGeomID o1, dGeomID o2)
{
    PhysDemoApp *app = (PhysDemoApp *)data;
    int numContacts = nOpende::Collide(o1, o2, PHYS_MAX_CONTACTS, &app->physContactArray[0].geom, sizeof(dContact));

    for (int index = 0; index < numContacts; index++)
    {
        dJointID jointId = nOpende::JointCreateContact(app->refPhysWorld->id, app->physColJointGroupId, &app->physContactArray[index]);
        nOpende::JointAttach(jointId, nOpende::GeomGetBody(o1), nOpende::GeomGetBody(o2));
    }
}

//------------------------------------------------------------------------------
/**
    Update the physical world
*/
void PhysDemoApp::UpdatePhysWorld(float &physTime)
{
    while (physTime > PHYSICS_STEPSIZE)
    {
        // Update the collision data
        nOpende::JointGroupEmpty(this->physColJointGroupId);
        this->refPhysColSpace->Collide(this, this->PhysCollisionCallback);

        // Step the world by frameTime (the amount of time since the last frame)
        this->refPhysWorld->QuickStep(PHYSICS_STEPSIZE);
        
        physTime -= PHYSICS_STEPSIZE;
    }


    // And now step through each object, updating the object's position to match the physical
    // object's position
    nRoot* objects = kernelServer->Lookup("/objects");
    SimpleObject* curObj;
    for (curObj = (SimpleObject*) objects->GetHead();
         curObj;
         curObj = (SimpleObject*) curObj->GetSucc())
    {
        if (curObj->refPhysBody.isvalid() && curObj->refPhysBody->IsEnabled())
        {
            // If the object is moving normally, dampen the movement a bit to simulate low-level friction
            // (the amount of dampening should depend on whether the object is contacting another or not,
            // and a contact-type-specific dampening amount, but oh well, this generally works)
            vector3 vel = curObj->refPhysBody->GetLinearVel();
            curObj->refPhysBody->AddForce(vel * -0.01f);
            vel = curObj->refPhysBody->GetAngularVel();
            curObj->refPhysBody->AddTorque(vel * -0.01f);

            // If the object has fallen below -10.0f y coord, kill it
            if (curObj->refPhysBody->GetPosition().y < -10.0f)
            {
                // we need to do this because we are about to remove an
                // object from the same linked list that we are iterating through.
                SimpleObject* pred = (SimpleObject*) curObj->GetPred();
                curObj->Release();
                curObj = pred;
            }
            // Otherwise, update the position
            else
            {
                curObj->Transform.settranslation(curObj->refPhysBody->GetPosition());
                curObj->Transform.setquatrotation(curObj->refPhysBody->GetQuaternion());
                curObj->renderContext.SetTransform(curObj->Transform.getmatrix());
            }
        }

        // if the floaty text is initialized, update its position
        if (curObj->refPhysBody.isvalid() && curObj->refFloatyText.isvalid())
        {
            matrix44 projMat = this->refGfxServer->GetTransform(nGfxServer2::ViewProjection);
            vector3 projCoords = projMat.transform_coord(curObj->Transform.gettranslation());

            // scale the projected coords to be in proper screen space.
            projCoords.x = projCoords.x / 2.0f + 0.5f;
            projCoords.y = -projCoords.y / 2.0f + 0.5f;

            // And finally, set up the rect to reflect the projected coordinates.
            rectangle screenCoords;
            screenCoords.v0.set(projCoords.x - curObj->textWidth / 2.0f, projCoords.y - curObj->textHeight / 2.0f - 0.05f);
            screenCoords.v1.set(projCoords.x + curObj->textWidth / 2.0f, projCoords.y + curObj->textHeight / 2.0f - 0.05f);
            curObj->refFloatyText->SetRect(screenCoords);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Renders all objects in the world
*/
void PhysDemoApp::RenderWorld(nTime time, uint frameId)
{
    // Begin rendering
    this->refSceneServer->BeginScene(viewMatrix);

    // move this to a simpleobject
    // Render a single default light (PhysDemo only needs the one)
    nTransformNode *lightNode = (nTransformNode *)kernelServer->Lookup("/scenenodes/default_light");
    if (lightNode)
        this->refSceneServer->Attach(&lightRenderContext);

    // loop through all objects in "/scenenodes"
    nRoot* objects = kernelServer->Lookup("/objects");
    SimpleObject* curObj;
    for (curObj = (SimpleObject*) objects->GetHead();
         curObj;
         curObj = (SimpleObject*) curObj->GetSucc())
    {
        // update render context variables
        curObj->renderContext.GetVariable(this->timeHandle)->SetFloat((float)time);
        curObj->renderContext.SetFrameId(frameId);

        // render the object        
        this->refSceneServer->Attach(&curObj->renderContext);
    }

    // Complete the rendering of the scene
    this->refSceneServer->EndScene();
    this->refSceneServer->RenderScene();             // renders the 3d scene
    //this->refGuiServer->Render(); // jamba: this is private now?                   // render the GUI elements
    this->refConServer->Render();                    // render the console and watch variables
    this->refSceneServer->PresentScene();            // present the frame
}

//------------------------------------------------------------------------------
/**
    Initialize the overlay GUI.
*/  
void
PhysDemoApp::InitOverlayGui()
{
    const float borderSize = 0.02f;

    // add an appropriate font for this app
    this->refGuiServer->AddSystemFont("physDefaultFont", "New Times Roman", this->fontSize, true, false, false);

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

    // let's add a text readout for the current FPS
    this->guiFPSLabel = (nGuiTextLabel *) kernelServer->New("nguitextlabel", "fpsreadout");
    n_assert(this->guiFPSLabel);
    logoRect.v0.set(0.0f, 0.0f);
    logoRect.v1.set(0.2f, 0.05f);
    this->guiFPSLabel->SetRect(logoRect);
    this->guiFPSLabel->SetText("0.0");
    this->guiFPSLabel->SetAlignment(nGuiTextLabel::Left);
    this->guiFPSLabel->SetColor(vector4(1.0f, 0.0f, 0.0f, 1.0f));
    this->guiFPSLabel->SetFont("physDefaultFont");

    // Also add a readout for CFM
    this->guiCFMLabel = (nGuiTextLabel *) kernelServer->New("nguitextlabel", "cfmreadout");
    n_assert(this->guiCFMLabel);
    logoRect.v0.set(0.0f, 0.05f);
    logoRect.v1.set(0.2f, 0.10f);
    this->guiCFMLabel->SetRect(logoRect);
    this->guiCFMLabel->SetText("0.0");
    this->guiCFMLabel->SetAlignment(nGuiTextLabel::Left);
    this->guiCFMLabel->SetColor(vector4(1.0f, 0.0f, 0.0f, 1.0f));
    this->guiCFMLabel->SetFont("physDefaultFont");

    // Also add a readout for ERP
    this->guiERPLabel = (nGuiTextLabel *) kernelServer->New("nguitextlabel", "erpreadout");
    n_assert(this->guiERPLabel);
    logoRect.v0.set(0.0f, 0.10f);
    logoRect.v1.set(0.2f, 0.15f);
    this->guiERPLabel->SetRect(logoRect);
    this->guiERPLabel->SetText("0.0");
    this->guiERPLabel->SetAlignment(nGuiTextLabel::Left);
    this->guiERPLabel->SetColor(vector4(1.0f, 0.0f, 0.0f, 1.0f));
    this->guiERPLabel->SetFont("physDefaultFont");

    // Also add a readout for fps lock
    this->guiFPSCutoffLabel = (nGuiTextLabel *) kernelServer->New("nguitextlabel", "fpscutoffreadout");
    n_assert(this->guiFPSCutoffLabel);
    logoRect.v0.set(0.0f, 0.15f);
    logoRect.v1.set(0.2f, 0.20f);
    this->guiFPSCutoffLabel->SetRect(logoRect);
    this->guiFPSCutoffLabel->SetText("0.0");
    this->guiFPSCutoffLabel->SetAlignment(nGuiTextLabel::Left);
    this->guiFPSCutoffLabel->SetColor(vector4(1.0f, 0.0f, 0.0f, 1.0f));
    this->guiFPSCutoffLabel->SetFont("physDefaultFont");


    kernelServer->PopCwd();

    // set the new user root window
    this->refGuiServer->SetRootWindowPointer(userRootWindow);
}
