#ifndef N_VIEWER_H
#define N_VIEWER_H
//------------------------------------------------------------------------------
/**
    A simple viewer app class.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gfx2/ndisplaymode2.h"
#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"
#include "video/nvideoserver.h"
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
#include "gui/nguiserver.h"

//------------------------------------------------------------------------------
class nViewerApp
{
public:
    /// control mode
    enum ControlMode
    {
        Maya,
        Fly,
    };

    /// constructor
    nViewerApp(nKernelServer* ks);
    /// destructor
    virtual ~nViewerApp();
    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// set optional feature set override
    void SetFeaturSetOverride(nGfxServer2::FeatureSet f);
    /// get optional feature set override
    nGfxServer2::FeatureSet GetFeatureSetOverride() const;
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
    /// set the scene server (required)
    void SetSceneServerClass(const char* name);
    /// get the scene type
    const char* GetSceneServerClass() const;
    /// set the script type (ntclserver/nluaserver/etc - required)
    void SetScriptServerClass(const char* name);
    /// get the script type
    const char* GetScriptServerClass() const;
    /// set the startup script (required)
    void SetStartupScript(const char* name);
    /// get the startup script
    const char* GetStartupScript() const;
    /// set the light stage script (required - if a scene file is specified)
    void SetStageScript(const char* name);
    /// get the light stage script
    const char* GetStageScript() const;
    /// set the input binding script (required)
    void SetInputScript(const char* name);
    /// get the input binding script
    const char* GetInputScript() const;
    /// enable/disable the logo overlay
    void SetOverlayEnabled(bool b);
    /// get overlay enabled status
    bool GetOverlayEnabled() const;
    /// open the viewer
    bool Open();
    /// close the viewer
    void Close();
    /// run the viewer, returns when app should exit
    void Run();
    /// return true if currently open
    bool IsOpen() const;

private:
    /// handle general input
    void HandleInput(float frameTime);
    /// handle input in Maya control mode
    void HandleInputMaya(float frameTime);
    /// handle input in Fly control mode
    void HandleInputFly(float frameTime);
    /// transfer global variables from variable server to render context
    void TransferGlobalVariables();
    /// initialize the overlay GUI
    void InitOverlayGui();

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
    nRef<nVideoServer> refVideoServer;
    nRef<nGuiServer> refGuiServer;

    nRef<nTransformNode> refRootNode;

    nString sceneFilename;
    nString projDir;
    nString sceneserverClass;
    nString scriptserverClass;
    nString startupScript;
    nString stageScript;
    nString inputScript;
    bool isOpen;
    bool isOverlayEnabled;
    nDisplayMode2 displayMode;
    nCamera2 camera;
    ControlMode controlMode;
    nGfxServer2::FeatureSet featureSetOverride;

    polar2 defViewerAngles;
    vector3 defViewerPos;
    vector3 defViewerZoom;

    float viewerVelocity;
    polar2 viewerAngles;
    vector3 viewerPos;
    vector3 viewerZoom;

    nRenderContext renderContext;
    matrix44 viewMatrix;
    int screenshotID;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetFeaturSetOverride(nGfxServer2::FeatureSet f)
{
    this->featureSetOverride = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::FeatureSet
nViewerApp::GetFeatureSetOverride() const
{
    return this->featureSetOverride;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nViewerApp::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
nViewerApp::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetCamera(const nCamera2& cam)
{
    this->camera = cam;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nCamera2&
nViewerApp::GetCamera() const
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetControlMode(ControlMode mode)
{
    this->controlMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
nViewerApp::ControlMode
nViewerApp::GetControlMode() const
{
    return this->controlMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetSceneFile(const char* filename)
{
    this->sceneFilename = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetSceneFile() const
{
    return this->sceneFilename.IsEmpty() ? 0 : this->sceneFilename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetProjDir(const char* name)
{
    this->projDir = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetProjDir() const
{
    return this->projDir.IsEmpty() ? 0 : this->projDir.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetSceneServerClass(const char* type)
{
    this->sceneserverClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetSceneServerClass() const
{
    return this->sceneserverClass.IsEmpty() ? 0 : this->sceneserverClass.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetScriptServerClass(const char* type)
{
    this->scriptserverClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetScriptServerClass() const
{
    return this->scriptserverClass.IsEmpty() ? 0 : this->scriptserverClass.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetStartupScript(const char* script)
{
    this->startupScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetStartupScript() const
{
    return this->startupScript.IsEmpty() ? 0 : this->startupScript.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetStageScript(const char* script)
{
    this->stageScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetStageScript() const
{
    return this->stageScript.IsEmpty() ? 0 : this->stageScript.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetInputScript(const char* script)
{
    this->inputScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetInputScript() const
{
    return this->inputScript.IsEmpty() ? 0 : this->inputScript.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetOverlayEnabled(bool b)
{
    this->isOverlayEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nViewerApp::GetOverlayEnabled() const
{
    return this->isOverlayEnabled;
}

//------------------------------------------------------------------------------
#endif    

