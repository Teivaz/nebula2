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
    viewerAngles(defViewerAngles),
    viewerZoom(defViewerZoom),
    screenshotID(0),
    fontSize(20)
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
    const char* result;

    n_assert(!this->isOpen);
    n_assert(this->GetInputScript());

    // initialize Nebula servers
    this->refScriptServer   = (nScriptServer*)   kernelServer->New("ntclserver", "/sys/servers/script");
    this->refGfxServer      = (nGfxServer2*)      kernelServer->New("nd3d9server", "/sys/servers/gfx");
    this->refInputServer    = (nInputServer*)    kernelServer->New("ndi8server", "/sys/servers/input");
    this->refConServer      = (nConServer*)      kernelServer->New("nconserver", "/sys/servers/console");
    this->refResourceServer = (nResourceServer*) kernelServer->New("nresourceserver", "/sys/servers/resource");
    this->refSceneServer    = (nSceneServer*)    kernelServer->New("nstdsceneserver", "/sys/servers/scene");
    this->refVarServer      = (nVariableServer*) kernelServer->New("nvariableserver", "/sys/servers/variable");
    this->refAnimServer     = (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
    this->refParticleServer = (nParticleServer*)  kernelServer->New("nparticleserver", "/sys/servers/particle");
    this->refUIDServer      = (eUIDServer*)      kernelServer->New("euidserver", "/sys/servers/uidserver");
    this->refGuiServer      = (nGuiServer*)       kernelServer->New("nguiserver", "/sys/servers/gui");

    // run the startup script
    this->refScriptServer->RunScript("startup.tcl", result);

    // set the gfx server feature set override
    if (this->featureSetOverride != nGfxServer2::InvalidFeatureSet)
    {
        this->refGfxServer->SetFeatureSetOverride(this->featureSetOverride);
    }

    // initialize the physics-related classes
    this->refPhysWorld      = (nOpendeWorld*)    kernelServer->New("nopendeworld", "/phys/world");
    // We're setting the gravity higher, and later, using big forces, to make everything "feel" right
    // (this is mostly because of the scale of the objects in the demo not being terrifically great)
    this->refPhysWorld->SetGravity(vector3(0.0f, -19.8f, 0.0f));
    this->refPhysWorld->SetERP(0.2f);
    this->refPhysColSpace   = (nOpendeHashSpace*)    kernelServer->New("nopendehashspace", "/phys/world/space");
    this->refPhysColSpace->Create();

    // Create the contact joint group and array (and set some values for the contact joints, since in this sim, every surface is treated identically)
    this->physColJointGroupId = nOpende::JointGroupCreate(0);
    this->physContactArray = new dContact[3];
    this->physContactArray[0].surface.mode = dContactApprox1;
    this->physContactArray[1].surface.mode = dContactApprox1;
    this->physContactArray[2].surface.mode = dContactApprox1;
    this->physContactArray[0].surface.mu = dInfinity / 2.0f;
    this->physContactArray[1].surface.mu = dInfinity / 2.0f;
    this->physContactArray[2].surface.mu = dInfinity / 2.0f;

    // define the input mapping
    this->refScriptServer->RunScript(this->GetInputScript(), result);

    // open the remote port
    this->kernelServer->GetRemoteServer()->Open("nviewer");

    // initialize graphics
    this->refGfxServer->SetDisplayMode(this->displayMode);
    this->camera.SetFarPlane(500.0f);
    this->refGfxServer->SetCamera(this->camera);
    this->refGfxServer->OpenDisplay();

    // initialize gui
    this->refGuiServer->SetRootPath("/gui");
    this->refGuiServer->Open();
    this->InitOverlayGui();


    // create the floor for the physical world
    this->CreateFloor(0.0f, -5.0f, 0.0f);

    // create a single default light
    nTransformNode *lightNode = (nTransformNode *)kernelServer->LoadAs("lights:point_lights/simple_light.n2", "/objects/default_light");
    lightNode->RenderContextCreated(&this->lightRenderContext);
    lightRenderContext.SetRootNode(lightNode);

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

    // First, disable any possibility of future rendering
    this->refGfxServer->CloseDisplay();
    // Then kill the GUI
    this->refGuiServer->Close();

    // Then, release the list of objects
    nNode *node = objectList.GetHead();

    while(node)
    {
        node->Remove();

        SimpleObject *obj = (SimpleObject *)node->GetPtr();
        obj->Release();

        delete(node);

        node = objectList.GetHead();
    }

    // Destroy the collision joint group
    nOpende::JointGroupDestroy(this->physColJointGroupId);

    // Destroy the contact joint array used by the collision callback
    delete(this->physContactArray);

    // Finally, move on to releasing the servers/etc
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
    nVariable::Handle timeHandle = this->refVarServer->GetVariableHandleByName("time");
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

        this->currFPS = 1.0f / frameTime;

        // Update the GUI's concept of time
        this->refGuiServer->SetTime(time);
        // Also update the GUI FPS element
        char fpsString[256];
        sprintf(fpsString, "FPS: %.1f", this->currFPS);
        this->guiFPSLabel->SetText(fpsString);

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
        n_sleep(0.005);
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
        nPathString filename;
        
        filename = "physdemo";

        filename.Append(nString(this->screenshotID++));
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

        this->CreateBox(x, -4.6f, z);
        this->CreateBox(x, -3.55f, z);
        this->CreateBox(x, -2.5f, z);
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
                this->CreateBox(x, -4.6f, index);
                this->CreateBox(x, -3.55f, index);
                this->CreateBox(x, -2.5f, index);
            }
        }
        if (z > -10 && z < 10)
        {
            for (float index = -10; index < 10.0f; index += 1.8f)
            {
                this->CreateBox(index, -4.6f, z);
                this->CreateBox(index, -3.55f, z);
                this->CreateBox(index, -2.5f, z);
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

        obj->refPhysBody->AddForce(forceVec * 10000000.0f);
    }
    // Create a floor-clearing explosion
    if (inputServer->GetButton("kaboom"))
    {
        this->CreateExplosion((rand()%1000)/200.0f - 2.5f, -4.0f, (rand()%1000)/200.0f - 2.5f, 30000.0);
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
    // First get the new universal ID for the object
    int uID = refUIDServer->GetNewID();
    // ... and turn it into a string, to work as an object name
    char *strUID = new char[10];
    itoa(uID, strUID, 10);

    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", strUID);
    nNode *objListNode = new nNode;
    objListNode->SetPtr(newObj);
    kernelServer->PopCwd();
    delete(strUID);

    // Make sure the obj knows its new uID
    newObj->uID = uID;

    // ... and add the newly-created container for the object to the object list
    objectList.AddTail(objListNode);

    // All other NOH classes used by this object will be defined under it in the NOH hierarchy, for organizational purposes
    kernelServer->PushCwd(newObj);

    // Create the box shapenode
    nShapeNode *objShapeNode = (nShapeNode *)kernelServer->LoadAs("shapes:big_flat_plane.n2", "shapenode");
    newObj->refRootShapeNode = objShapeNode;

    // Now set the data for the newly created shapenode
    newObj->refRootShapeNode->SetPosition(vector3(x, y, z));

    // ... and update the render context
    newObj->timeHandle = this->refVarServer->GetVariableHandleByName("time");
    newObj->renderContext.AddVariable(nVariable(newObj->timeHandle, 0.0f));

    // ... and let the shapenode and the rendercontext know about eachother.
    newObj->refRootShapeNode->RenderContextCreated(&newObj->renderContext);
    newObj->renderContext.SetRootNode(objShapeNode);

    n_assert(newObj == kernelServer->GetCwd());

    // Now create the physical representation
    // (the floor has no body, only a geom, which is connected to body 0, "the world")

    nOpendeBoxGeom *physGeom = (nOpendeBoxGeom *)kernelServer->New("nopendeboxgeom", "physgeom");
    physGeom->Create("/phys/world/space");
    physGeom->SetPosition(vector3(x, y, z));
    physGeom->SetLengths(20.0f, 1.0f, 20.0f);
    physGeom->SetBody((dBodyID)0);
    newObj->refPhysGeom = physGeom;    

    // Finished defining objects, pop the Cwd
    kernelServer->PopCwd();

    return newObj;
}

//------------------------------------------------------------------------------
/**
    Create a box
*/
SimpleObject *
PhysDemoApp::CreateBox(float x, float y, float z)
{
    // First get the new universal ID for the object
    int uID = refUIDServer->GetNewID();
    // ... and turn it into a string, to work as an object name
    char *strUID = new char[10];
    itoa(uID, strUID, 10);

    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", strUID);
    nNode *objListNode = new nNode;
    objListNode->SetPtr(newObj);
    kernelServer->PopCwd();
    delete(strUID);

    // Make sure the obj knows its new uID
    newObj->uID = uID;

    // ... and add the newly-created container for the object to the object list
    objectList.AddTail(objListNode);

    // All other NOH classes used by this object will be defined under it in the NOH hierarchy, for organizational purposes
    kernelServer->PushCwd(newObj);

    // Create the box shapenode
    nShapeNode *objShapeNode = (nShapeNode *)kernelServer->LoadAs("shapes:box/box.n2", "shapenode");
    newObj->refRootShapeNode = objShapeNode;

    // Now set the data for the newly created shapenode
    newObj->refRootShapeNode->SetPosition(vector3(x, y, z));

    // ... and update the render context
    newObj->timeHandle = this->refVarServer->GetVariableHandleByName("time");
    newObj->renderContext.AddVariable(nVariable(newObj->timeHandle, 0.0f));

    // ... and let the shapenode and the rendercontext know about eachother.
    newObj->refRootShapeNode->RenderContextCreated(&newObj->renderContext);
    newObj->renderContext.SetRootNode(objShapeNode);

    n_assert(newObj == kernelServer->GetCwd());

    // Now create the physical representation
    newObj->refPhysBody = (nOpendeBody *)kernelServer->New("nopendebody", "physbody");
    newObj->refPhysBody->Create("/phys/world");
    newObj->refPhysBody->SetPosition(vector3(x, y, z));
    newObj->refPhysBody->SetBoxMass(10.0f, 1.0f, 1.0f, 1.0f);

    nOpendeBoxGeom *physGeom = (nOpendeBoxGeom *)kernelServer->New("nopendeboxgeom", "physgeom");
    physGeom->Create("/phys/world/space");
    physGeom->SetLengths(1.0f, 1.0f, 1.0f);
    physGeom->SetBody(newObj->refPhysBody->id);
    physGeom->SetData((void *)newObj);
    physGeom->SetCategoryBits(PHYS_COLBITS_OBJECT);
    newObj->refPhysGeom = physGeom;

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
    // First get the new universal ID for the object
    int uID = refUIDServer->GetNewID();
    // ... and turn it into a string, to work as an object name
    char *strUID = new char[10];
    itoa(uID, strUID, 10);

    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", strUID);
    nNode *objListNode = new nNode;
    objListNode->SetPtr(newObj);
    kernelServer->PopCwd();
    delete(strUID);

    // Make sure the obj knows its new uID
    newObj->uID = uID;

    // ... and add the newly-created container for the object to the object list
    objectList.AddTail(objListNode);

    // All other NOH classes used by this object will be defined under it in the NOH hierarchy, for organizational purposes
    kernelServer->PushCwd(newObj);

    // Create the box shapenode
    nShapeNode *objShapeNode = (nShapeNode *)kernelServer->LoadAs("shapes:sphere/sphere.n2", "shapenode");
    newObj->refRootShapeNode = objShapeNode;

    // Now set the data for the newly created shapenode
    newObj->refRootShapeNode->SetPosition(vector3(x, y, z));

    // ... and update the render context
    newObj->timeHandle = this->refVarServer->GetVariableHandleByName("time");
    newObj->renderContext.AddVariable(nVariable(newObj->timeHandle, 0.0f));

    // ... and let the shapenode and the rendercontext know about eachother.
    newObj->refRootShapeNode->RenderContextCreated(&newObj->renderContext);
    newObj->renderContext.SetRootNode(objShapeNode);

    n_assert(newObj == kernelServer->GetCwd());

    // Now create the physical representation
    newObj->refPhysBody = (nOpendeBody *)kernelServer->New("nopendebody", "physbody");
    newObj->refPhysBody->Create("/phys/world");
    newObj->refPhysBody->SetPosition(vector3(x, y, z));
    newObj->refPhysBody->SetSphereMass(10.0f, 0.5f);

    nOpendeSphereGeom *physGeom = (nOpendeSphereGeom *)kernelServer->New("nopendespheregeom", "physgeom");
    physGeom->Create("/phys/world/space");
    physGeom->SetRadius(0.5f);
    physGeom->SetBody(newObj->refPhysBody->id);
    physGeom->SetData((void *)newObj);
    physGeom->SetCategoryBits(PHYS_COLBITS_OBJECT);
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
    // First get the new universal ID for the object
    int uID = refUIDServer->GetNewID();
    // ... and turn it into a string, to work as an object name
    char *strUID = new char[10];
    itoa(uID, strUID, 10);


    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", strUID);
    nNode *objListNode = new nNode;
    objListNode->SetPtr(newObj);
    kernelServer->PopCwd();
    delete(strUID);

    // Make sure the obj knows its new uID
    newObj->uID = uID;

    // ... and add the newly-created container for the object to the object list
    objectList.AddTail(objListNode);

    // All other NOH classes used by this object will be defined under it in the NOH hierarchy, for organizational purposes
    kernelServer->PushCwd(newObj);

    // Create the box shapenode
    nShapeNode *objShapeNode = (nShapeNode *)kernelServer->LoadAs("shapes:sphere/sphere.n2", "shapenode");
    objShapeNode->SetScale(vector3(2.0f, 2.0f, 2.0f));
    newObj->refRootShapeNode = objShapeNode;

    // Now set the data for the newly created shapenode
    newObj->refRootShapeNode->SetPosition(vector3(x, y, z));

    // ... and update the render context
    newObj->timeHandle = this->refVarServer->GetVariableHandleByName("time");
    newObj->renderContext.AddVariable(nVariable(newObj->timeHandle, 0.0f));

    // ... and let the shapenode and the rendercontext know about eachother.
    newObj->refRootShapeNode->RenderContextCreated(&newObj->renderContext);
    newObj->renderContext.SetRootNode(objShapeNode);

    n_assert(newObj == kernelServer->GetCwd());

    // Now create the physical representation
    newObj->refPhysBody = (nOpendeBody *)kernelServer->New("nopendebody", "physbody");
    newObj->refPhysBody->Create("/phys/world");
    newObj->refPhysBody->SetPosition(vector3(x, y, z));
    newObj->refPhysBody->SetSphereMass(100.0f, 1.0f);

    nOpendeSphereGeom *physGeom = (nOpendeSphereGeom *)kernelServer->New("nopendespheregeom", "physgeom");
    physGeom->Create("/phys/world/space");
    physGeom->SetRadius(1.0f);
    physGeom->SetBody(newObj->refPhysBody->id);
    physGeom->SetData((void *)newObj);
    physGeom->SetCategoryBits(PHYS_COLBITS_OBJECT);
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
    // First get the new universal ID for the object
    int uID = refUIDServer->GetNewID();
    // ... and turn it into a string, to work as an object name
    char *strUID = new char[10];
    itoa(uID, strUID, 10);

    // Establish the containers for the new object.
    kernelServer->PushCwd(kernelServer->Lookup("/objects"));
    SimpleObject *newObj = (SimpleObject *)kernelServer->New("simpleobject", strUID);
    nNode *objListNode = new nNode;
    objListNode->SetPtr(newObj);
    kernelServer->PopCwd();

    // Make sure the obj knows its new uID
    newObj->uID = uID;

    // ... and add the newly-created container for the object to the object list
    objectList.AddTail(objListNode);

    // All other NOH classes used by this object will be defined under it in the NOH hierarchy, for organizational purposes
    kernelServer->PushCwd(newObj);

    // Create the box shapenode
    nShapeNode *objShapeNode = (nShapeNode *)kernelServer->LoadAs("shapes:sphere/sphere.n2", "shapenode");
    newObj->refRootShapeNode = objShapeNode;

    // Now set the data for the newly created shapenode
    newObj->refRootShapeNode->SetPosition(vector3(x, y, z));

    // ... and update the render context
    newObj->timeHandle = this->refVarServer->GetVariableHandleByName("time");
    newObj->renderContext.AddVariable(nVariable(newObj->timeHandle, 0.0f));

    // ... and let the shapenode and the rendercontext know about eachother.
    newObj->refRootShapeNode->RenderContextCreated(&newObj->renderContext);
    newObj->renderContext.SetRootNode(objShapeNode);

    n_assert(newObj == kernelServer->GetCwd());

    // Now create the physical representation
    newObj->refPhysBody = (nOpendeBody *)kernelServer->New("nopendebody", "physbody");
    newObj->refPhysBody->Create("/phys/world");
    newObj->refPhysBody->SetPosition(vector3(x, y, z));
    newObj->refPhysBody->SetSphereMass(10000.0f, 0.5f);

    nOpendeSphereGeom *physGeom = (nOpendeSphereGeom *)kernelServer->New("nopendespheregeom", "physgeom");
    physGeom->Create("/phys/world/space");
    physGeom->SetRadius(0.5f);
    physGeom->SetBody(newObj->refPhysBody->id);
    physGeom->SetData((void *)newObj);
    physGeom->SetCategoryBits(PHYS_COLBITS_OBJECT);
    newObj->refPhysGeom = physGeom;

    // Finished defining objects, pop the Cwd
    kernelServer->PopCwd();

    // Let's give the bullet some floaty text
    this->kernelServer->PushCwd(this->refGuiServer->GetRootWindowPointer());

    newObj->refFloatyText = (nGuiTextLabel *) kernelServer->New("nguitextlabel", strUID);
    n_assert(newObj->refFloatyText.isvalid());
    newObj->refFloatyText->SetText("bullet");
    newObj->refFloatyText->SetAlignment(nGuiTextLabel::Alignment::Center);
    newObj->refFloatyText->SetColor(vector4(1.0f, 0.0f, 0.0f, 1.0f));
    newObj->refFloatyText->SetFont("physDefaultFont");

    newObj->textWidth = 0.15;
    newObj->textHeight = 0.05;

    // Now we're REALLY finished with strUID, so delete it
    delete(strUID);

    this->kernelServer->PopCwd();

    return newObj;
}

//------------------------------------------------------------------------------
/**
    Create an explosion
*/
void
PhysDemoApp::CreateExplosion(float x, float y, float z, float force)
{
    // Grab the head node to the object list.
    nNode *node = this->objectList.GetHead();

    // Loop through the entire object list, applying the explosion to all created objects (in
    // a real engine, you would do an octree sphere check and only apply the effect to those
    // objects within the 
    while(node)
    {
        SimpleObject *obj = (SimpleObject *)node->GetPtr();
        
        if (obj->refPhysBody.isvalid())
        {
            vector3 explosionVector = obj->refPhysBody->GetPosition() - vector3(x, y, z);
            explosionVector.norm();
            explosionVector = explosionVector * force;

            obj->refPhysBody->AddForceAtPos(explosionVector, vector3(x, y, z));
        }

        node = node->GetSucc();
    }
}

//------------------------------------------------------------------------------
/**
    Destroy a shape
*/
void
PhysDemoApp::DestroyObject(int objID)
{
    // Grab the head node to the object list.
    nNode *node = this->objectList.GetHead();

    // Loop through the entire list, checking for the object (I know, I know, this isn't horribly
    // efficient - so don't do this in a final game)
    while(node)
    {
        SimpleObject *obj = (SimpleObject *)node->GetPtr();

        // If the object IDs line up, we have our object!
        if (obj->uID == objID)
        {
            // ... so ditch the object
            obj->Release();
            node->Remove();
            delete(node);

            // and release the UID for future use
            refUIDServer->ReleaseID(objID);

            return;
        }

        node = node->GetSucc();
    }
}

//------------------------------------------------------------------------------
/**
    Collision resolution callback
*/
void PhysDemoApp::PhysCollisionCallback(void *data, dGeomID o1, dGeomID o2)
{
    PhysDemoApp *app = (PhysDemoApp *)data;
    int numContacts = nOpende::Collide(o1, o2, 3, &app->physContactArray[0].geom, sizeof(dContact));

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
        this->refPhysWorld->Step(PHYSICS_STEPSIZE);
        
        physTime -= PHYSICS_STEPSIZE;
    }

    // And now step through each object, updating the nshapenode position to match the physical
    // position - in this case, the physical position is authortative.
    nNode *node = objectList.GetHead();
    while(node)
    {
        SimpleObject *obj = (SimpleObject *)node->GetPtr();

        if (obj->refPhysBody.isvalid())
        {
            // If the object has fallen below -10.0f y coord, kill it
            if (obj->refPhysBody->GetPosition().y < -10.0f)
            {
                node = node->GetSucc();

                this->DestroyObject(obj->uID);
            }
            // Otherwise, update the position
            else
            {
                nShapeNode *shapeNode = obj->refRootShapeNode.get();
                shapeNode->SetPosition(obj->refPhysBody->GetPosition());
                shapeNode->SetQuat(obj->refPhysBody->GetQuaternion());

                // if the floaty text is initialized, update its position
                if (obj->refFloatyText.isvalid())
                {
                    matrix44 projMat = this->refGfxServer->GetTransform(nGfxServer2::ViewProjection);

                    vector3 textCoords = obj->refRootShapeNode->GetPosition();
                    vector4 projCoords;

                    // First, move the original coords into a vector4
                    projCoords.x = textCoords.x;
                    projCoords.y = textCoords.y;
                    projCoords.z = textCoords.z;
                    projCoords.w = 1.0f;

                    // Now, do textCoords * projectMat (I know, it looks backwards, no idea why the operation works this way)
                    projCoords = projMat * projCoords;

                    // Now divide by w to complete the projection.
                    projCoords.x /= projCoords.w;
                    projCoords.y /= projCoords.w;
                    projCoords.z /= projCoords.w;

                    // ... and scale the projected coords to be in proper screen space.
                    projCoords.x = projCoords.x / 2.0f + 0.5f;
                    projCoords.y = -projCoords.y / 2.0f + 0.5f;

                    // And finally, set up the rect to reflect the projected coordinates.
                    rectangle screenCoords;
                    screenCoords.v0.set(projCoords.x - obj->textWidth / 2.0f, projCoords.y - obj->textHeight / 2.0f - 0.05f);
                    screenCoords.v1.set(projCoords.x + obj->textWidth / 2.0f, projCoords.y + obj->textHeight / 2.0f - 0.05f);
                    obj->refFloatyText->SetRect(screenCoords);
                }

                node = node->GetSucc();
            }
        }
        else node = node->GetSucc();
    }
}

//------------------------------------------------------------------------------
/**
    Renders all objects in the world
*/
void PhysDemoApp::RenderWorld(nTime time, uint frameId)
{
    nNode *node = objectList.GetHead();

    // Begin rendering
    this->refSceneServer->BeginScene(viewMatrix);

    // Render a single default light (PhysDemo only needs the one)
    nTransformNode *lightNode = (nTransformNode *)kernelServer->Lookup("/objects/default_light");
    if (lightNode)
        this->refSceneServer->Attach(&lightRenderContext);

    while(node)
    {
        SimpleObject *obj = (SimpleObject *)node->GetPtr();

        // update render context variables
        obj->renderContext.GetVariable(obj->timeHandle)->SetFloat((float)time);
        obj->renderContext.SetFrameId(frameId);

        // render the object        
        this->refSceneServer->Attach(&obj->renderContext);

        node = node->GetSucc();
    }

    // Complete the rendering of the scene
    this->refSceneServer->EndScene();
    this->refSceneServer->RenderScene();             // renders the 3d scene
    this->refGuiServer->Render();                    // render the GUI elements
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
    this->guiFPSLabel->SetText("30.0");
    this->guiFPSLabel->SetAlignment(nGuiTextLabel::Alignment::Left);
    this->guiFPSLabel->SetColor(vector4(1.0f, 0.0f, 0.0f, 1.0f));
    this->guiFPSLabel->SetFont("physDefaultFont");

    kernelServer->PopCwd();

    // set the new user root window
    this->refGuiServer->SetRootWindowPointer(userRootWindow);
}