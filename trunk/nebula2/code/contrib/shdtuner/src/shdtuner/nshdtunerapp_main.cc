//----------------------------------------------------------------------------
//  (c) 2005    John Smith
//----------------------------------------------------------------------------
#include "shdtuner/nshdtunerapp.h"
#include "kernel/nkernelserver.h"

nNebulaScriptClass(nShdTunerApp, "nroot")

//----------------------------------------------------------------------------
/**
*/
nShdTunerApp::nShdTunerApp() {
    iPickedObject = 0;
}

//----------------------------------------------------------------------------
/**
*/
nShdTunerApp::~nShdTunerApp() {
}

//----------------------------------------------------------------------------
/**
*/
void nShdTunerApp::Initialize() {
    nRoot::Initialize();
    CreateServers();
}

//----------------------------------------------------------------------------
/**
    @return             success or failure
*/
bool nShdTunerApp::Release() {
    refInputSvr->Release();
    refPrefSvr->Release();
    refConSvr->Release();
    refGuiSvr->Release();
    refShadowSvr->Release();
    refParticleSvr->Release();
    refAnimationSvr->Release();
    refSceneSvr->Release();
    refGfxSvr->Release();
    refVariableSvr->Release();
    refResourceSvr->Release();
    refScriptSvr->Release();
    return nRoot::Release();
}

//----------------------------------------------------------------------------
/**
    @return             success or failure
*/
bool nShdTunerApp::Open() {
    n_verify(refScriptSvr->RunScript("home:data/scripts/shdtuner/shdtuner.py", scriptResult));
    OpenServers();
    CreateInputSystems();
    objectList.SetStageScript("gfxlib:stdlight.n2");
    objectList.SetLightStageEnabled(true);
    objectList.Open();
    return true;
}

//----------------------------------------------------------------------------
/**
*/
void nShdTunerApp::Run() {
    nTimeServer* timeSvr = kernelServer->GetTimeServer();
    n_assert(timeSvr != NULL);
    uint frameId = 0;
    bool isRunning = true;
    while (isRunning && refGfxSvr->Trigger()) {
        nTime time = timeSvr->GetTime();
        refGuiSvr->SetTime(time);
        isRunning = refScriptSvr->Trigger();
        refParticleSvr->Trigger();
        refInputSvr->Trigger(time);
        if (!refGuiSvr->IsMouseOverGui()) {
            nInputEvent* evt = refInputSvr->FirstEvent();
            int x = evt->GetAbsXPos();
            int y = evt->GetAbsYPos();
            if (x > 0 || y > 0) {
                if (refInputSvr->GetButton("look") || refInputSvr->GetButton("pan") || refInputSvr->GetButton("zoom")) {
                    iPickedObject = PickObject(refInputSvr->GetMousePos());
                    nString func;
                    func.Format("OnMouseClick(%d, %d, %d)", evt->GetButton(), x, y);
                    n_verify(refScriptSvr->Run(func.Get(), scriptResult));
                }
                cameraCtrl.SetLookButton(refInputSvr->GetButton("look"));
                cameraCtrl.SetPanButton(refInputSvr->GetButton("pan"));
                cameraCtrl.SetZoomButton(refInputSvr->GetButton("zoom"));
                cameraCtrl.SetSliderLeft(refInputSvr->GetSlider("left"));
                cameraCtrl.SetSliderRight(refInputSvr->GetSlider("right"));
                cameraCtrl.SetSliderUp(refInputSvr->GetSlider("up"));
                cameraCtrl.SetSliderDown(refInputSvr->GetSlider("down"));
            }
            cameraCtrl.SetResetButton(refInputSvr->GetButton("reset"));
        }
        refGuiSvr->Trigger();
        objectList.Trigger(time, frameId++);
        cameraCtrl.Update();
        if (!refGfxSvr->InDialogBoxMode()) {
            refSceneSvr->BeginScene(cameraCtrl.GetViewMatrix());
            for (uint i = 0; i < objectList.GetCount(); i++) {
                refSceneSvr->Attach(objectList.GetRenderContextAt(i));
            }
            refSceneSvr->EndScene();
            refSceneSvr->RenderScene();
            refGfxSvr->SetTransform(nGfxServer2::Model, matrix44());
            refGfxSvr->BeginLines();
            if (iPickedObject != 0) {
                DrawObjectBoundingBox(iPickedObject, vector4(1.0f, 0.0f, 0.0f, 1.0f));
            }
            refGfxSvr->EndLines();
            refSceneSvr->PresentScene();
        }
        refInputSvr->FlushEvents();
        kernelServer->Trigger();
        n_sleep(0.0);
    }
}

//----------------------------------------------------------------------------
/**
*/
void nShdTunerApp::Close() {
    objectList.Close();
    CloseServers();
}

//----------------------------------------------------------------------------
/**
*/
void nShdTunerApp::ResetScene() {
    kernelServer->GetTimeServer()->ResetTime();
    objectList.Clear();
    objectList.AddDefaultEntry();
    iPickedObject = 0;
}

//----------------------------------------------------------------------------
/**
    @param fileName     object file name
    @return             object Id
*/
uint nShdTunerApp::LoadObject(const char* fileName) {
    kernelServer->GetTimeServer()->ResetTime();
    uint count = objectList.GetCount();
    matrix44 trans = objectList.GetRenderContextAt(count - 1)->GetTransform();
    if (count > 1) {
        trans.translate(vector3(10.0f, 0.0f, 0.0f));
    }
    objectList.LoadObject(fileName);
    objectList.GetRenderContextAt(count)->SetTransform(trans);
    return count;
}

//----------------------------------------------------------------------------
/**
    @param pos          mouse pick position
    @return             object Id
*/
uint nShdTunerApp::PickObject(const vector2& pos) const {
    line3 line;
    GetPickRay(pos, line);
    float dist = line.len();
    uint id = 0;
    for (uint i = 1; i < objectList.GetCount(); i++) {
        bbox3 bb;
        GetObjectBoundingBox(i, bb);
        vector3 ipos;
        if (bb.intersect(line, ipos)) {
            float idist = vector3::distance(line.start(), bb.center());
            if (idist < dist) {
                dist = idist;
                id = i;
            }
        }
    }
    return id;
}

//----------------------------------------------------------------------------
/**
    @return             picked object
*/
uint nShdTunerApp::GetPickedObject() const {
    return iPickedObject;
}

//----------------------------------------------------------------------------
/**
    @param objectId     object Id
    @return             object transform node
*/
nTransformNode* nShdTunerApp::GetObjectNode(uint objectId) const {
    if (objectId != 0) {
        return objectList.GetNodeAt(objectId);
    }
    return NULL;
}

//----------------------------------------------------------------------------
/**
    @param objectId     object Id
    @return             object position
*/
vector3 nShdTunerApp::GetObjectPosition(uint objectId) const {
    if (objectId != 0) {
        return objectList.GetRenderContextAt(objectId)->GetTransform().pos_component();
    }
    return vector3();
}

//----------------------------------------------------------------------------
/**
    @param objectId     object Id
    @return             object size
*/
vector3 nShdTunerApp::GetObjectSize(uint objectId) const {
    if (objectId != 0) {
        bbox3 bb;
        GetObjectBoundingBox(objectId, bb);
        return bb.size();
    }
    return vector3();
}

//----------------------------------------------------------------------------
/**
*/
void nShdTunerApp::CreateServers() {
    refScriptSvr = static_cast<nScriptServer*>(kernelServer->New("npythonserver", "/sys/servers/script"));
    n_assert(refScriptSvr.isvalid());
    refResourceSvr = static_cast<nResourceServer*>(kernelServer->New("nresourceserver", "/sys/servers/resource"));
    n_assert(refResourceSvr.isvalid());
    refVariableSvr = static_cast<nVariableServer*>(kernelServer->New("nvariableserver", "/sys/servers/variable"));
    n_assert(refVariableSvr.isvalid());
    refGfxSvr = static_cast<nGfxServer2*>(kernelServer->New("nd3d9server", "/sys/servers/gfx"));
    n_assert(refGfxSvr.isvalid());
    refSceneSvr = static_cast<nSceneServer*>(kernelServer->New("nsceneserver", "/sys/servers/scene"));
    n_assert(refSceneSvr.isvalid());
    refAnimationSvr = static_cast<nAnimationServer*>(kernelServer->New("nanimationserver", "/sys/servers/anim"));
    n_assert(refAnimationSvr.isvalid());
    refParticleSvr = static_cast<nParticleServer*>(kernelServer->New("nparticleserver", "/sys/servers/particle"));
    n_assert(refParticleSvr.isvalid());
    refShadowSvr = static_cast<nShadowServer2*>(kernelServer->New("nshadowserver2", "/sys/servers/shadow"));
    n_assert(refShadowSvr.isvalid());
    refGuiSvr = static_cast<nGuiServer*>(kernelServer->New("nguiserver", "/sys/servers/gui"));
    n_assert(refGuiSvr.isvalid());
    refConSvr = static_cast<nConServer*>(kernelServer->New("nconserver", "/sys/servers/console"));
    n_assert(refConSvr.isvalid());
    refPrefSvr = static_cast<nPrefServer*>(kernelServer->New("nwin32prefserver", "/sys/servers/pref"));
    n_assert(refPrefSvr.isvalid());
}

//----------------------------------------------------------------------------
/**
*/
void nShdTunerApp::OpenServers() {
    nFileServer2* fileSvr = kernelServer->GetFileServer();
    n_assert(fileSvr != NULL);
    fileSvr->SetAssign("data", "home:data");
    fileSvr->SetAssign("export", "home:export");
    fileSvr->SetAssign("scripts", "data:scripts");
	fileSvr->SetAssign("renderpath", "data:shaders");
    fileSvr->SetAssign("gfxlib", "export:gfxlib");
    fileSvr->SetAssign("anims", "export:anims");
    fileSvr->SetAssign("meshes", "export:meshes");
    fileSvr->SetAssign("textures", "export:textures");
    n_verify(kernelServer->New("nenv", "/sys/env/parent_hwnd") != NULL);
    refGfxSvr->SetDisplayMode(nDisplayMode2(NULL, nDisplayMode2::ChildWindow, 0, 0, 800, 600, false, false, NULL));
    refGfxSvr->SetSkipMsgLoop(true);
    n_verify(refScriptSvr->RunFunction("OnShowWindow", scriptResult));
    nGfxServer2::FeatureSet featureSet = refGfxSvr->GetFeatureSet();
    if (featureSet == nGfxServer2::DX9) {
        refSceneSvr->SetRenderPathFilename("data:shaders/dx9_renderpath.xml");
    } else if (featureSet == nGfxServer2::DX9FLT) {
        refSceneSvr->SetRenderPathFilename("data:shaders/dx9hdr_renderpath.xml");
    } else {
        refSceneSvr->SetRenderPathFilename("data:shaders/dx7_renderpath.xml");
    }
    refSceneSvr->SetOcclusionQuery(false);
    refSceneSvr->SetObeyLightLinks(false);
    n_verify(refSceneSvr->Open());
    refShadowSvr->SetUseZFail(true);
    refGuiSvr->SetRootPath("/gui");
    refGuiSvr->SetDisplaySize(vector2(800.0f, 600.0f));
    n_verify(refGuiSvr->Open());
    refConSvr->Open();
    refConSvr->Toggle();
    refPrefSvr->SetCompanyName("Nebula2 Public Community");
    refPrefSvr->SetApplicationName("Shader Tuner");
    n_verify(refPrefSvr->Open());
}

//----------------------------------------------------------------------------
/**
*/
void nShdTunerApp::CreateInputSystems() {
    refInputSvr = static_cast<nInputServer*>(kernelServer->New("ndi8server", "/sys/servers/input"));
    n_assert(refInputSvr.isvalid());
    refInputSvr->Open();
    refInputSvr->BeginMap();
    refInputSvr->Map("keyb0:space.down",            "reset");
    refInputSvr->Map("relmouse0:btn0.pressed",      "look");
    refInputSvr->Map("relmouse0:btn1.pressed",      "pan");
    refInputSvr->Map("relmouse0:btn2.pressed",      "zoom");
    refInputSvr->Map("relmouse0:-x",                "left");
    refInputSvr->Map("relmouse0:+x",                "right");
    refInputSvr->Map("relmouse0:-y",                "up");
    refInputSvr->Map("relmouse0:+y",                "down");
    refInputSvr->EndMap();
}

//----------------------------------------------------------------------------
/**
*/
void nShdTunerApp::CloseServers() {
    refPrefSvr->Close();
    refConSvr->Close();
    refGuiSvr->Close();
    refSceneSvr->Close();
}

//----------------------------------------------------------------------------
/**
    @param pos          mouse pick position
    @param line         result line
*/
void nShdTunerApp::GetPickRay(const vector2& pos, line3& line) const {
    nCamera2 cam = refGfxSvr->GetCamera();
    float minX, maxX, minY, maxY, minZ, maxZ;
    cam.GetViewVolume(minX, maxX, minY, maxY, minZ, maxZ);
    vector3 start;
    start.x = (pos.x*2.0f - 1.0f)*maxX;
    start.y = (1.0f - pos.y*2.0f)*maxY;
    start.z = -minZ;
    vector3 end;
    float ratio = maxZ/minZ;
    end.x = start.x*ratio;
    end.y = start.y*ratio;
    end.z = -maxZ;
    matrix44 mat = refGfxSvr->GetTransform(nGfxServer2::InvView);
    line.set(mat*start, mat*end);
}

//----------------------------------------------------------------------------
/**
    @param objectId     object Id
    @param color        bounding box color
*/
void nShdTunerApp::DrawObjectBoundingBox(uint objectId, const vector4& color) const {
    bbox3 bb;
    GetObjectBoundingBox(objectId, bb);
    vector3 center = bb.center();
    vector3 ext = bb.extents();
    vector3 v[8];
    v[0] = center + vector3(ext.x, -ext.y, ext.z);
    v[1] = center + vector3(ext.x, -ext.y, -ext.z);
    v[2] = center + vector3(-ext.x, -ext.y, -ext.z);
    v[3] = center + vector3(-ext.x, -ext.y, ext.z);
    v[4] = center + vector3(ext.x, ext.y, ext.z);
    v[5] = center + vector3(ext.x, ext.y, -ext.z);
    v[6] = center + vector3(-ext.x, ext.y, -ext.z);
    v[7] = center + vector3(-ext.x, ext.y, ext.z);
    vector3 cube[16] = {
        v[1], v[0], v[4], v[5], v[1], v[2], v[6], v[5],
        v[3], v[2], v[6], v[7], v[3], v[0], v[4], v[7]
    };
    refGfxSvr->DrawLines3d(cube, 8, color);
    refGfxSvr->DrawLines3d(cube + 8, 8, color);
}

//----------------------------------------------------------------------------
/**
    @param objectId     object Id
    @param bb           bounding box
*/
void nShdTunerApp::GetObjectBoundingBox(uint objectId, bbox3& bb) const {
    bb.set(vector3(), vector3());
    if (objectId != 0) {
        matrix44 trans;
        bb.begin_extend();
        GetBoundingBox(objectList.GetNodeAt(objectId), bb, trans);
        bb.end_extend();
        bb.transform(objectList.GetRenderContextAt(objectId)->GetTransform());
    }
}

//----------------------------------------------------------------------------
/**
    @param node         transform node
    @param bb           bounding box
    @param trans        transform matrix
*/
void nShdTunerApp::GetBoundingBox(nTransformNode* node, bbox3& bb, matrix44& trans) const {
    bbox3 lbb = node->GetLocalBox();
    trans.translate(node->GetPosition());
    lbb.transform(trans);
    bb.extend(lbb);
    nRoot* child = node->GetHead();
    while (child != NULL) {
        if (node->IsA("ntransformnode")) {
            GetBoundingBox(static_cast<nTransformNode*>(child), bb, trans);
        }
        child = child->GetSucc();
    }
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
