#ifndef N_SDBVIEWER_H
#define N_SDBVIEWER_H
//------------------------------------------------------------------------------
/**
    A simple viewer app class to demo the spatial database system.
    
    (C) 2004 Gary Haussmann
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
#include "gui/nguiserver.h"
#include "shadow/nshadowserver.h"

#include "spatialdb/nspatialelements.h"
#include "spatialdb/nscriptablesector.h"
#include "spatialdb/nvisitorbase.h"

//------------------------------------------------------------------------------
class nSDBViewerApp
{
public:
    /// control mode
    enum ControlMode
    {
        Maya,
        Fly,
    };

    /// constructor
    nSDBViewerApp(nKernelServer* ks);
    /// destructor
    virtual ~nSDBViewerApp();

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
    /// set scene file name
    void SetSceneFile(const char* name);
    /// get scene file name
    const char* GetSceneFile() const;
    /// set the scene server (required)
    void SetSceneServerClass(const char* name);
    /// get the scene type
    const char* GetSceneServerClass() const;
    /// set the script type (ntclserver/nluaserver/etc - required)
    void SetScriptServerClass(const char* name);
    /// set optional project dir
    void SetProjDir(const char* name);
    /// get project dir
    const char* GetProjDir() const;
    /// get the script type
    const char* GetScriptServerClass() const;
    /// set optional startup script
    void SetStartupScript(const char* name);
    /// get optional startup script
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
    /// open the viewer
    bool Open();
    /// close the viewer
    void Close();
    /// run the viewer, returns when app should exit
    void Run();
    /// return true if currently open
    bool IsOpen() const;

    /// reset object positions
    void ResetTestObjects();

    /// delete all test objects
    void DeleteTestObjects();

    /// mark objects with the three cameras
    void UpdateObjectMarks(nVisibilityVisitor::VisibleElements &v);

private:
    /// handle general input
    void HandleInput(float frameTime);
    /// handle input in Maya control mode
    void HandleInputMaya(float frameTime);
    /// handle input in Fly control mode
    void HandleInputFly(float frameTime);
    /// handle movement of the 'play' objects using visibility
    void HandleInputPlay(float frameTime);
    /// handle state change--mainly switching play object
    void HandlePlaySwitch(float framtTime);
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
    nRef<nGuiServer> refGuiServer;
    nRef<nShadowServer> refShadowServer;

    nRef<nTransformNode> refRootNode;
    nRef<nTransformNode> refCameraMarkerNode;
    nRef<nTransformNode> refTestObjectNode;
    nRef<nTransformNode> refOccluderObjectNode;

    nRenderContext renderContext;

    nString sceneFilename;
    nString projDir;
    nString sceneserverClass;
    nString scriptserverClass;
    nString startupScript;
    nString stageScript;
    nString inputScript;
    bool isOpen;
    bool isOverlayEnabled;
    bool showVisualDebug;
    nDisplayMode2 displayMode;
    nCamera2 chasecamera, playcamera;
    ControlMode controlMode;
    nGfxServer2::FeatureSet featureSetOverride;

    polar2 defViewerAngles;
    vector3 defViewerPos;
    vector3 defViewerZoom;

    float viewerVelocity;
    int screenshotID;

	// describes which view clipper we are currently using
	enum { Frustum, OccludingFrustum, Sphere, OccludingSphere, SpatialSphere } CurrentClipState;

    // spatial db reference-just a sector, really
    nAutoRef<nScriptableSector> m_rootsector;

    struct CameraDescription {
        polar2 viewerAngles;
        vector3 viewerPos;
        vector3 viewerZoom;
        matrix44 viewMatrix;
        nRenderContext camerarc;

        void GenerateTransform(matrix44 &xform)
        {
            xform.ident();
            xform.translate(viewerZoom);
            xform.rotate_x(viewerAngles.theta);
            xform.rotate_y(viewerAngles.rho);
            xform.translate(viewerPos);
            matrix44 xformcopy(xform);
            //xformcopy.invert_simple();
            camerarc.SetTransform(xformcopy);
            viewMatrix = xform;
        }
    };

    // these cameras are used to mark the objects
    enum { CAMERACOUNT = 3 };
    CameraDescription markcameras[CAMERACOUNT];
    // which camera is used as the render view? which camera is the user controlling? which camera is used for visibility?
    int m_viewcamera, m_activecamera, m_viscamera;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetFeatureSetOverride(nGfxServer2::FeatureSet f)
{
    this->featureSetOverride = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::FeatureSet
nSDBViewerApp::GetFeatureSetOverride() const
{
    return this->featureSetOverride;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSDBViewerApp::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
nSDBViewerApp::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetCamera(const nCamera2& cam)
{
    this->chasecamera = cam;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nCamera2&
nSDBViewerApp::GetCamera() const
{
    return this->chasecamera;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetControlMode(ControlMode mode)
{
    this->controlMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
nSDBViewerApp::ControlMode
nSDBViewerApp::GetControlMode() const
{
    return this->controlMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetSceneFile(const char* filename)
{
    this->sceneFilename = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSDBViewerApp::GetSceneFile() const
{
    return this->sceneFilename.IsEmpty() ? 0 : this->sceneFilename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetProjDir(const char* name)
{
    this->projDir = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSDBViewerApp::GetProjDir() const
{
    return this->projDir.IsEmpty() ? 0 : this->projDir.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetSceneServerClass(const char* type)
{
    this->sceneserverClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSDBViewerApp::GetSceneServerClass() const
{
    return this->sceneserverClass.IsEmpty() ? 0 : this->sceneserverClass.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetScriptServerClass(const char* type)
{
    this->scriptserverClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSDBViewerApp::GetScriptServerClass() const
{
    return this->scriptserverClass.IsEmpty() ? 0 : this->scriptserverClass.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetStartupScript(const char* script)
{
    this->startupScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSDBViewerApp::GetStartupScript() const
{
    return this->startupScript.IsEmpty() ? 0 : this->startupScript.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetStageScript(const char* script)
{
    this->stageScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSDBViewerApp::GetStageScript() const
{
    return this->stageScript.IsEmpty() ? 0 : this->stageScript.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetInputScript(const char* script)
{
    this->inputScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSDBViewerApp::GetInputScript() const
{
    return this->inputScript.IsEmpty() ? 0 : this->inputScript.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSDBViewerApp::SetOverlayEnabled(bool b)
{
    this->isOverlayEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSDBViewerApp::GetOverlayEnabled() const
{
    return this->isOverlayEnabled;
}

//------------------------------------------------------------------------------
#endif    
