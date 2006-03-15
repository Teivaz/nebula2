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
#include "particle/nparticleserver2.h"
#include "mathlib/polar.h"
#include "kernel/nremoteserver.h"
#include "gui/nguiserver.h"
#include "shadow2/nshadowserver2.h"
#include "network/nhttpserver.h"
#include "misc/nprefserver.h"
#include "tools/nmayacamcontrol.h"
#include "tools/nnodelist.h"
#include "audio3/naudioserver3.h"
#include "misc/ncaptureserver.h"

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
    void SetGfxServerClass(const nString& cl);
    /// get the gfx server class
    const nString& GetGfxServerClass() const;
    /// set scene server class to use
    /// set scene file name
    void SetSceneFile(const nString& name);
    /// get scene file name
    const nString& GetSceneFile() const;
    /// set optional project dir
    void SetProjDir(const nString& name);
    /// get project dir
    const nString& GetProjDir() const;
    /// set the scene server (required)
    void SetSceneServerClass(const nString& cl);
    /// get the scene type
    const nString& GetSceneServerClass() const;
    /// set the script type (ntclserver/nluaserver/etc - required)
    void SetScriptServerClass(const nString& cl);
    /// get the script type
    const nString& GetScriptServerClass() const;
    /// set the startup script (required)
    void SetStartupScript(const nString& name);
    /// get the startup script
    const nString& GetStartupScript() const;
    /// set optional render path (else autoselect)
    void SetRenderPath(const nString& p);
    /// get optional render path
    const nString& GetRenderPath() const;
    /// set the light stage script (required - if a scene file is specified)
    void SetStageScript(const nString& name);
    /// get the light stage script
    const nString& GetStageScript() const;
    /// enable/disable the logo overlay
    void SetOverlayEnabled(bool b);
    /// enable/disable the standard light stage
    void SetLightStageEnabled(bool b);
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
    nRef<nParticleServer2> refParticleServer2;
    nRef<nVideoServer> refVideoServer;
    nRef<nGuiServer> refGuiServer;
    nRef<nShadowServer2> refShadowServer;
    nRef<nHttpServer> refHttpServer;
    nRef<nPrefServer> refPrefServer;
    nRef<nAudioServer3> refAudioServer;
    nRef<nCaptureServer> refCaptureServer;

    nMayaCamControl camControl;

    nString sceneFilename;
    nString projDir;
    nString sceneServerClass;
    nString scriptServerClass;
    nString startupScript;
    nString stageScript;
    nString gfxServerClass;
    nString renderPath;
    bool isOpen;
    bool isOverlayEnabled;
    bool useRam;
    bool lightStageEnabled;
    nDisplayMode2 displayMode;
    nCamera2 camera;
    nGfxServer2::FeatureSet featureSetOverride;
    nRenderContext renderContext;
    matrix44 viewMatrix;
    nNodeList nodeList;
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
nViewerApp::SetSceneFile(const nString& filename)
{
    this->sceneFilename = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nViewerApp::GetSceneFile() const
{
    return this->sceneFilename;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetProjDir(const nString& name)
{
    this->projDir = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nViewerApp::GetProjDir() const
{
    return this->projDir;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetSceneServerClass(const nString& cl)
{
    this->sceneServerClass = cl;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nViewerApp::GetSceneServerClass() const
{
    return this->sceneServerClass;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetScriptServerClass(const nString& cl)
{
    this->scriptServerClass = cl;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nViewerApp::GetScriptServerClass() const
{
    return this->scriptServerClass;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetStartupScript(const nString& script)
{
    this->startupScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nViewerApp::GetStartupScript() const
{
    return this->startupScript;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetStageScript(const nString& script)
{
    this->stageScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nViewerApp::GetStageScript() const
{
    return this->stageScript;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetGfxServerClass(const nString& cl)
{
    this->gfxServerClass = cl;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nViewerApp::GetGfxServerClass() const
{
    return this->gfxServerClass;
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
/**
*/
inline
void
nViewerApp::SetLightStageEnabled(bool b)
{
    this->lightStageEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewerApp::SetRenderPath(const nString& p)
{
    this->renderPath = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nViewerApp::GetRenderPath() const
{
    return this->renderPath;
}

//------------------------------------------------------------------------------
#endif

