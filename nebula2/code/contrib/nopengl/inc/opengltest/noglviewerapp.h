#ifndef N_OGLVIEWERAPP_H
#define N_OGLVIEWERAPP_H
//------------------------------------------------------------------------------
/**
    @class nOGLViewerApp
    @ingroup Tools

    A simple viewer app class.
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
#include "shadow/nshadowserver.h"
#include "network/nhttpserver.h"
#include "misc/nprefserver.h"

//------------------------------------------------------------------------------
class nOGLViewerApp
{
public:
    /// control mode
    enum ControlMode
    {
        Maya,
        Fly,
    };

    /// constructor
    nOGLViewerApp();
    /// destructor
    virtual ~nOGLViewerApp();
    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// set optional feature set override
    void SetFeatureSetOverride(nGfxServer2::FeatureSet f);
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
    /// set the gfx server class to use
    void SetGfxServerClass(const char* cl);
    /// get the gfx server class
    const char* GetGfxServerClass() const;
    /// set scene server class to use
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
    virtual bool Open();
    /// close the viewer
    virtual void Close();
    /// run the viewer, returns when app should exit
    virtual void Run();
    /// called before nSceneServer::BeginScene()
    virtual void OnFrameBefore();
    /// called after nSceneServer::RenderScene()
    virtual void OnFrameRendered();
    /// return true if currently open
    bool IsOpen() const;

protected:
    /// handle general input
    void HandleInput(float frameTime);
    /// define the input mapping
    void DefineInputMapping();
    /// handle input in Maya control mode
    void HandleInputMaya(float frameTime);
    /// handle input in Fly control mode
    void HandleInputFly(float frameTime);
    /// transfer global variables from variable server to render context
    void TransferGlobalVariables();
    /// initialize the overlay GUI
    void InitOverlayGui();
    /// validate the root node (and render context), if necessary
    void ValidateRootNode();

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
    nRef<nShadowServer> refShadowServer;
    nRef<nHttpServer> refHttpServer;
    nRef<nPrefServer> refPrefServer;

    nRef<nTransformNode> refRootNode;

    nString sceneFilename;
    nString projDir;
    nString sceneServerClass;
    nString scriptServerClass;
    nString startupScript;
    nString stageScript;
    nString inputScript;
    nString gfxServerClass;
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
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetFeatureSetOverride(nGfxServer2::FeatureSet f)
{
    this->featureSetOverride = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::FeatureSet
nOGLViewerApp::GetFeatureSetOverride() const
{
    return this->featureSetOverride;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nOGLViewerApp::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
nOGLViewerApp::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetCamera(const nCamera2& cam)
{
    this->camera = cam;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nCamera2&
nOGLViewerApp::GetCamera() const
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetControlMode(ControlMode mode)
{
    this->controlMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
nOGLViewerApp::ControlMode
nOGLViewerApp::GetControlMode() const
{
    return this->controlMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetSceneFile(const char* filename)
{
    this->sceneFilename = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nOGLViewerApp::GetSceneFile() const
{
    return this->sceneFilename.IsEmpty() ? 0 : this->sceneFilename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetProjDir(const char* name)
{
    this->projDir = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nOGLViewerApp::GetProjDir() const
{
    return this->projDir.IsEmpty() ? 0 : this->projDir.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetSceneServerClass(const char* type)
{
    this->sceneServerClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nOGLViewerApp::GetSceneServerClass() const
{
    return this->sceneServerClass.IsEmpty() ? 0 : this->sceneServerClass.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetScriptServerClass(const char* type)
{
    this->scriptServerClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nOGLViewerApp::GetScriptServerClass() const
{
    return this->scriptServerClass.IsEmpty() ? 0 : this->scriptServerClass.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetStartupScript(const char* script)
{
    this->startupScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nOGLViewerApp::GetStartupScript() const
{
    return this->startupScript.IsEmpty() ? 0 : this->startupScript.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetStageScript(const char* script)
{
    this->stageScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nOGLViewerApp::GetStageScript() const
{
    return this->stageScript.IsEmpty() ? 0 : this->stageScript.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetInputScript(const char* script)
{
    this->inputScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nOGLViewerApp::GetInputScript() const
{
    return this->inputScript.IsEmpty() ? 0 : this->inputScript.Get();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetGfxServerClass(const char* cl)
{
    n_assert(cl);
    this->gfxServerClass = cl;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nOGLViewerApp::GetGfxServerClass() const
{
    return this->gfxServerClass.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOGLViewerApp::SetOverlayEnabled(bool b)
{
    this->isOverlayEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nOGLViewerApp::GetOverlayEnabled() const
{
    return this->isOverlayEnabled;
}

//------------------------------------------------------------------------------
#endif    

