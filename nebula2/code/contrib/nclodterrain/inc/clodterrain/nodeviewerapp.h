#ifndef N_LUAVIEWER_H
#define N_LUAVIEWER_H
//------------------------------------------------------------------------------
/**
    A simple viewer app class.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gfx2/ndisplaymode2.h"
#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nsceneserver.h"
#include "scene/nrendercontext.h"
#include "kernel/nscriptserver.h"
#include "input/ninputserver.h"
#include "misc/nconserver.h"
#include "resource/nresourceserver.h"
#include "variable/nvariableserver.h"
#include "anim2/nanimationserver.h"
#include "particle/nparticleserver.h"
#include "mathlib/polar.h"
#include "kernel/nremoteserver.h"

#include "opende/nopendeworld.h"
#include "opende/nopendespace.h"
#include "opende/nopendegeom.h"
#include "opende/nopendebody.h"
#include "opende/nopendeserver.h"

//------------------------------------------------------------------------------
class nODEViewerApp
{
public:
    /// control mode
    enum ControlMode
    {
        Maya,
        Fly,
    };

    /// constructor
    nODEViewerApp(nKernelServer* ks);
    /// destructor
    virtual ~nODEViewerApp();

    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// set camera parameters
    void SetCamera(const nCamera2& camera);
    /// get camera parameters
    const nCamera2& GetCamera() const;
    /// set control mode
    void SetControlMode(ControlMode c);
    /// get control mode
    ControlMode GetControlMode() const;
    /// set scene file name
    void SetSceneFile(const char* name);
    /// get scene file name
    const char* GetSceneFile() const;
    /// set optional project dir
    void SetProjDir(const char* name);
    /// get project dir
    const char* GetProjDir() const;
    /// set optional startup script
    void SetStartupScript(const char* name);
    /// get optional startup script
    const char* GetStartupScript() const;
    /// open the viewer
    bool Open();
    /// close the viewer
    void Close();
    /// run the viewer, returns when app should exit
    void Run();
    /// return true if currently open
    bool IsOpen() const;

    /// setup dynamics objects
    void InitDynamics();

    /// reset object positions
    void ResetTestObjects();

private:
    /// define the input mapping
    void DefineInputMapping();
    /// handle input in Maya control mode
    void HandleInputMaya(float frameTime);
    /// handle input in Fly control mode
    void HandleInputFly(float frameTime);
    /// transfer global variables from variable server to render context
    void TransferGlobalVariables();

    nKernelServer* kernelServer;
    nRef<nScriptServer> refScriptServer;
    nRef<nGfxServer2> refGfxServer;
    nRef<nInputServer> refInputServer;
    nRef<nConServer> refConServer;
    nRef<nResourceServer> refResourceServer;
    nRef<nSceneServer> refSceneServer;
    nRef<nVariableServer> refVarServer;
    nRef<nAnimationServer> refAnimServer;
    nRef<nParticleServer> refParticleServer;

    nRef<nTransformNode> refRootNode;

    nString sceneFilename;
    nString projDir;
    nString startupScript;
    bool isOpen;
    nDisplayMode2 displayMode;
    nCamera2 camera;
    ControlMode controlMode;

    polar2 defViewerAngles;
    vector3 defViewerPos;
    vector3 defViewerZoom;

    float viewerVelocity;
    polar2 viewerAngles;
    vector3 viewerPos;
    vector3 viewerZoom;

    nRenderContext renderContext;
    matrix44 viewMatrix;

    // dynamics stuff-hold a world and some bodies
    enum { NUMTHINGS = 5 };
    nRef<nOpendeWorld> refDyWorld;
    nRef<nOpendeSpace> refDySpace;
    nRef<nOpendeSpace> refTerrainSpace;
    nRef<nOpendeServer> refDyServer;
    nRef<nOpendeGeom> geoms[NUMTHINGS];
    nRef<nOpendeBody> bodies[NUMTHINGS];
    nRef<nOpendeGeom> planegeom;

    // body representing ode geoms/bodies
    nRef<nTransformNode> refSimplebody;
    nRenderContext bodyContext;
    dJointGroupID contactgroup;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nODEViewerApp::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nODEViewerApp::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
nODEViewerApp::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nODEViewerApp::SetCamera(const nCamera2& cam)
{
    this->camera = cam;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nCamera2&
nODEViewerApp::GetCamera() const
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nODEViewerApp::SetControlMode(ControlMode mode)
{
    this->controlMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
nODEViewerApp::ControlMode
nODEViewerApp::GetControlMode() const
{
    return this->controlMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nODEViewerApp::SetSceneFile(const char* filename)
{
    this->sceneFilename = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nODEViewerApp::GetSceneFile() const
{
    return this->sceneFilename.IsEmpty() ? 0 : this->sceneFilename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nODEViewerApp::SetProjDir(const char* name)
{
    this->projDir = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nODEViewerApp::GetProjDir() const
{
    return this->projDir.IsEmpty() ? 0 : this->projDir.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nODEViewerApp::SetStartupScript(const char* script)
{
    this->startupScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nODEViewerApp::GetStartupScript() const
{
    return this->startupScript.IsEmpty() ? 0 : this->startupScript.Get();
}

//------------------------------------------------------------------------------
#endif    
