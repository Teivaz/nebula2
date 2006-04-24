#ifndef N_PHYSDEMOAPP_H
#define N_PHYSDEMOAPP_H
//------------------------------------------------------------------------------
/**
    @class PhysDemoApp
    @ingroup PhysDemoContribModule
    @brief A simple viewer app class (modified for PhysDemo use).
    
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
#include "gui/nguiserver.h"
#include "gui/nguiwindow.h"
#include "gui/nguilabel.h"
#include "gui/nguitextlabel.h"
#include "shadow2/nshadowserver2.h"

#include "physdemo/simpleobject.h"

#include "opende/nopendeworld.h"
#include "opende/nopendehashspace.h"
#include "opende/nopendebody.h"
#include "opende/nopendeboxgeom.h"
#include "opende/nopendespheregeom.h"
#include "opende/nopendeplanegeom.h"

#define PHYSICS_STEPSIZE 0.01f
#define PHYS_MAX_CONTACTS 4
#define PHYS_QUICKSTEP_ITERS 10
#define PHYS_LINEAR_VEL_THRESHOLD 0.008f
#define PHYS_ANGULAR_VEL_THRESHOLD 0.008f
#define PHYS_VEL_THRESHOLD_TIMEOUT 10

//------------------------------------------------------------------------------
class PhysDemoApp
{
public:
    /// constructor
    PhysDemoApp(nKernelServer* ks);
    /// destructor
    virtual ~PhysDemoApp();
    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// set optional feature set override
    void SetFeatureSetOverride(nGfxServer2::FeatureSet f);
    /// get optional feature set override
    nGfxServer2::FeatureSet GetFeatureSetOverride() const;
    /// set font size
    void SetFontSize(int newFontSize);
    /// get font size
    int GetFontSize() const;
    /// set camera parameters
    void SetCamera(const nCamera2& camera);
    /// get camera parameters
    const nCamera2& GetCamera() const;
    /// set the input binding script (required)
    void SetInputScript(const nString& name);
    /// get the input binding script
    const char* GetInputScript() const;
    /// open the viewer
    bool Open();
    /// close the viewer
    void Close();
    /// run the viewer, returns when app should exit
    void Run();
    /// return true if currently open
    bool IsOpen() const;


    /// entity management

    /// Creates the base "floor" object
    SimpleObject *CreateFloor(float x, float y, float z);
    /// Creates a box
    SimpleObject *CreateBox(float x, float y, float z, bool createDisabled = false);
    /// Creates a sphere
    SimpleObject *CreateSphere(float x, float y, float z);
    /// Creates a big sphere
    SimpleObject *CreateBigSphere(float x, float y, float z);
    /// Creates a bullet (an INCREDIBLY massive sphere)
    SimpleObject *CreateBullet(float x, float y, float z);
    /// Creates an explosion
    void CreateExplosion(float x, float y, float z, float force, bool enableObjects = false);
    /// Destroys any phys object
    void DestroyObject(int objID);

    /// The callback used by UpdatePhysWorld to evaluate collisions
    static void PhysCollisionCallback(void *data, dGeomID o1, dGeomID o2);
    /// update the physical world (and syncs mesh positions with the phys objects)
    void UpdatePhysWorld(float &physTime);
    /// renders all objects in the world
    void RenderWorld(nTime time, uint frameId);

private:
    /// handle general input
    void HandleInput(float frameTime);
    /// handle movement input
    void HandleMovementInput(float frameTime);
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
    nRef<nShadowServer2> refShadowServer;

    nString sceneFilename;
    nString projDir;
    nString sceneserverClass;
    nString scriptserverClass;
    nString startupScript;
    nString stageScript;
    nString inputScript;
    bool isOpen;
    nDisplayMode2 displayMode;
    nCamera2 camera;

    nGfxServer2::FeatureSet featureSetOverride;

    polar2 defViewerAngles;
    vector3 defViewerPos;
    vector3 defViewerZoom;

    float viewerVelocity;
    polar2 viewerAngles;
    vector3 viewerPos;
    vector3 viewerZoom;

    matrix44 viewMatrix;
    int screenshotID;
    float currFPS;
    float fpsCutoff;
    int fontSize;

    /// GUI label used for displaying the FPS (pointer retained so that the text can be updated easily)
    nGuiTextLabel *guiFPSLabel;

    nGuiTextLabel *guiCFMLabel, *guiERPLabel, *guiFPSCutoffLabel;

    /// entity management
    nList objectList;

    nRef<nOpendeWorld> refPhysWorld;
    nRef<nOpendeHashSpace> refPhysColSpace;

    dJointGroupID physColJointGroupId;
    dContact *physContactArray;

    /// a single default light render context
    nRenderContext lightRenderContext;
    
    /// used to make object names unique
    int objectID;
    
    /// variable handles for the render context
    nVariable::Handle timeHandle;
    nVariable::Handle windHandle;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
PhysDemoApp::SetFeatureSetOverride(nGfxServer2::FeatureSet f)
{
    this->featureSetOverride = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::FeatureSet
PhysDemoApp::GetFeatureSetOverride() const
{
    return this->featureSetOverride;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PhysDemoApp::SetFontSize(int newFontSize)
{
    this->fontSize = newFontSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
PhysDemoApp::GetFontSize() const
{
    return this->fontSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
PhysDemoApp::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PhysDemoApp::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
PhysDemoApp::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PhysDemoApp::SetCamera(const nCamera2& cam)
{
    this->camera = cam;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nCamera2&
PhysDemoApp::GetCamera() const
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PhysDemoApp::SetInputScript(const nString& script)
{
    this->inputScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
PhysDemoApp::GetInputScript() const
{
    return this->inputScript.IsEmpty() ? 0 : this->inputScript.Get();
}

//------------------------------------------------------------------------------
#endif    
