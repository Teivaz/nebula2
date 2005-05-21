#ifndef N_VIEWER_H
#define N_VIEWER_H
//------------------------------------------------------------------------------
/**
    @class nViewerApp
    @ingroup Tools

    A simple viewer app class. This is for use for small sample apps
    and for nviewer. It should not be used as the basis of your own
    application that uses Nebula 2.  For that, see the @ref Application.

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
#include "shadow/nshadowserver.h"
#include "network/nhttpserver.h"
#include "misc/nprefserver.h"
#include "tools/nmayacamcontrol.h"

//------------------------------------------------------------------------------
class nViewerApp
{
public:

    /// constructor
    nViewerApp();
    /// destructor
    virtual ~nViewerApp();
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
    /// enable/disable the logo overlay
    void SetOverlayEnabled(bool b);
    /// Make application load file from ram instead of disk, if `v' is true.
    void SetUseRam(bool v);
    /// Load file from ram instead of disk?
    bool UseRam() const;
    
    // get the Camera-Control instance
    nMayaCamControl& GetCamControl();

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
    /// define the input mapping
    void DefineInputMapping();
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
    nMayaCamControl camControl;

    nString sceneFilename;
    nString projDir;
    nString sceneServerClass;
    nString scriptServerClass;
    nString startupScript;
    nString stageScript;
    nString gfxServerClass;
    bool isOpen;
    bool isOverlayEnabled;
    bool useRam;
    nDisplayMode2 displayMode;
    nCamera2 camera;
    nGfxServer2::FeatureSet featureSetOverride;
    nRenderContext renderContext;
    matrix44 viewMatrix;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetFeatureSetOverride(nGfxServer2::FeatureSet f)
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
    this->sceneServerClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetSceneServerClass() const
{
    return this->sceneServerClass.IsEmpty() ? 0 : this->sceneServerClass.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetScriptServerClass(const char* type)
{
    this->scriptServerClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetScriptServerClass() const
{
    return this->scriptServerClass.IsEmpty() ? 0 : this->scriptServerClass.Get();
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
nViewerApp::SetGfxServerClass(const char* cl)
{
    n_assert(cl);
    this->gfxServerClass = cl;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nViewerApp::GetGfxServerClass() const
{
    return this->gfxServerClass.Get();
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
/**
*/
inline
void
nViewerApp::SetUseRam(bool v)
{
    this->useRam = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nViewerApp::UseRam() const
{
    return this->useRam;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMayaCamControl&
nViewerApp::GetCamControl()
{
    return this->camControl;
}
//------------------------------------------------------------------------------
#endif

