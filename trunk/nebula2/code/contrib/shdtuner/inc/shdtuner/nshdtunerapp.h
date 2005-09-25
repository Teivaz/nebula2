//----------------------------------------------------------------------------
/**
    @class nShdTunerApp
    @ingroup shdtuner
    @brief a brief description of the class

    (c) 2005    John Smith
*/
//----------------------------------------------------------------------------
#ifndef N_SHDTUNERAPP_H
#define N_SHDTUNERAPP_H

#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "kernel/nscriptserver.h"
#include "resource/nresourceserver.h"
#include "variable/nvariableserver.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nsceneserver.h"
#include "anim2/nanimationserver.h"
#include "particle/nparticleserver.h"
#include "shadow2/nshadowserver2.h"
#include "gui/nguiserver.h"
#include "misc/nconserver.h"
#include "misc/nprefserver.h"
#include "input/ninputserver.h"
#include "tools/nnodelist.h"
#include "tools/nmayacamcontrol.h"

//----------------------------------------------------------------------------
class nShdTunerApp : public nRoot
{
public:
    /// constructor
    nShdTunerApp();
    /// destructor
    virtual ~nShdTunerApp();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

public:
    /// initialize
    virtual void Initialize();
    /// release
    virtual bool Release();
    /// open application
    bool Open();
    /// run application
    void Run();
    /// close application
    void Close();

public:
    /// reset scene
    void ResetScene();
    /// load object
    uint LoadObject(const char* fileName);
    /// mouse pick object
    uint PickObject(const vector2& pos) const;
    /// get picked object
    uint GetPickedObject() const;
    /// get object transform node by id
    nTransformNode* GetObjectNode(uint objectId) const;
    /// get object position by id
    vector3 GetObjectPosition(uint objectId) const;
    /// get object size by id
    vector3 GetObjectSize(uint objectId) const;

private:
    void CreateServers();
    void OpenServers();
    void CreateInputSystems();
    void CloseServers();
    void GetPickRay(const vector2& pos, line3& line) const;
    void DrawObjectBoundingBox(uint objectId, const vector4& color) const;
    void GetObjectBoundingBox(uint objectId, bbox3& bb) const;
    void GetBoundingBox(nTransformNode* node, bbox3& bb, matrix44& trans) const;

private:
    nRef<nScriptServer> refScriptSvr;
    nRef<nResourceServer> refResourceSvr;
    nRef<nVariableServer> refVariableSvr;
    nRef<nGfxServer2> refGfxSvr;
    nRef<nSceneServer> refSceneSvr;
    nRef<nAnimationServer> refAnimationSvr;
    nRef<nParticleServer> refParticleSvr;
    nRef<nShadowServer2> refShadowSvr;
    nRef<nGuiServer> refGuiSvr;
    nRef<nConServer> refConSvr;
    nRef<nPrefServer> refPrefSvr;
    nRef<nInputServer> refInputSvr;
    nNodeList objectList;
    nMayaCamControl cameraCtrl;
    uint iPickedObject;
    nString scriptResult;
};

//----------------------------------------------------------------------------
#endif // N_SHDTUNERAPP_H
