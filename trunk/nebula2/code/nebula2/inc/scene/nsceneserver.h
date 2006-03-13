#ifndef N_SCENESERVER_H
#define N_SCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nSceneServer
    @ingroup Scene

    @brief The scene server builds a scene from nSceneNode objects and then
    renders it.
    
    The scene is rebuilt every frame, so some sort of culling should happen
    externally before building the scene. 

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "mathlib/matrix.h"
#include "kernel/nautoref.h"
#include "gfx2/nshaderparams.h"
#include "util/nbucket.h"
#include "renderpath/nrenderpath2.h"
#include "misc/nwatched.h"
#include "gfx2/nmesh2.h"
#include "kernel/nprofiler.h"

class nRenderContext;
class nSceneNode;
class nGfxServer2;
class nShader2;
class nTexture2;
class nRpPhase;
class nOcclusionQuery;

//------------------------------------------------------------------------------
class nSceneServer : public nRoot
{
public:
    /// constructor
    nSceneServer();
    /// destructor
    virtual ~nSceneServer();
    /// return pointer to instance
    static nSceneServer* Instance();
    /// set filename of render path definition XML file
    void SetRenderPathFilename(const nString& renderPathFilename);
    /// get the render path filename
    const nString& GetRenderPathFilename() const;
    /// open the scene server, call after nGfxServer2::OpenDisplay()
    virtual bool Open();
    /// close the scene server;
    virtual void Close();
    /// return true if scene server open
    bool IsOpen() const;
    /// begin the scene
    virtual bool BeginScene(const matrix44& viewer);
    /// attach the toplevel object of a scene node hierarchy to the scene
    virtual void Attach(nRenderContext* renderContext);
    /// finish the scene
    virtual void EndScene();
    /// render the scene through the default render path section
    virtual void RenderScene();
    /// present the scene
    virtual void PresentScene();
    /// begin a group node
    virtual void BeginGroup(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// finish current group node
    virtual void EndGroup();
    /// set current model matrix
    void SetModelTransform(const matrix44& m);
    /// get current model matrix
    const matrix44& GetModelTransform() const;
    /// access to current render path object
    const nRenderPath2* GetRenderPath() const;
    /// enable/disable debug visualization
    void SetRenderDebug(bool b);
    /// get debug visualization flag
    bool GetRenderDebug() const;
    /// set obey light link mode
    void SetObeyLightLinks(bool b);
    /// get obey light link mode
    bool GetObeyLightLinks() const;
    /// enable/disable occlusion query
    void SetOcclusionQuery(bool b);
    /// get occlusion query status
    bool GetOcclusionQuery() const;
    /// enable/disable clip plane fencing for point lights
    void SetClipPlaneFencing(bool b);
    /// get clip plane fencing mode
    bool GetClipPlaneFencing() const;

private:

    static nSceneServer* Singleton;

    class Group
    {
    public:
        int parentIndex;                // index of parent in group array, or -1
        nRenderContext* renderContext;
        nSceneNode* sceneNode;
        matrix44 modelTransform;
        int lightPass;
    };

    class LightInfo
    {
    public:
        ushort groupIndex;                  // group index of the light source itself
        rectangle scissorRect;              // scissor rect of the light
        vector4 shadowLightMask;            // the shadow light index
        nArray<plane> clipPlanes;           // clip planes for the light
    };

    /// split scene nodes into light and shape nodes
    void SplitNodes();
    /// make sure scene node resources are valid
    void ValidateNodeResources();
    /// sort shape nodes for optimal rendering
    void SortNodes();
    /// static qsort() compare function for scene nodes
    static int __cdecl CompareNodes(const ushort* i1, const ushort* i2);
    /// static qsort() compare function for shadow light sources
    static int __cdecl CompareShadowLights(const LightInfo* i1, const LightInfo* i2);
    /// do the render path rendering
    void DoRenderPath(nRpSection& rpSection);
    /// render shadow
    void RenderShadow(nRpPass& curPass);
    /// render a complete phase for light mode "Off"
    void RenderPhaseLightModeOff(nRpPhase& curPhase);
    /// render a complete phase for light mode "FFP"
    void RenderPhaseLightModeFFP(nRpPhase& curPhase);
    /// render a complete phase for light mode "Shader"
    void RenderPhaseLightModeShader(nRpPhase& curPhase);
    /// render single shape with light mode "Off"
    void RenderShapeLightModeOff(const Group& shapeGroup);
    /// render single shape with light mode "FFP"
    void RenderShapeLightModeFFP(const Group& shapeGroup);
    /// render single shape with light mode "Shader"
    void RenderShapeLightModeShader(Group& shapeGroup, const nRpSequence& seq);
    /// update scissor rects for one light source
    void ComputeLightScissor(LightInfo& lightInfo);
    /// update the clip planes for a single light source
    void ComputeLightClipPlanes(LightInfo& lightInfo);
    /// update scissor rects and clip planes once for all lights
    void ComputeLightScissorsAndClipPlanes();
    /// apply the scissor rectangle for a light group
    void ApplyLightScissors(const LightInfo& lightInfo);
    /// the clip planes for a light group
    void ApplyLightClipPlanes(const LightInfo& lightInfo);
    /// reset light scissors and clip plane
    void ResetLightScissorsAndClipPlanes();
    /// render debug visualization of light scissors
    void DebugRenderLightScissors();
    /// return true if a shape's light links contain the given light
    bool IsShapeLitByLight(const Group& shapeGroup, const Group& lightGroup);
    /// render the cameras
    void RenderCameraScene();
    /// copy the stencil buffer state to a texture
    void CopyStencilBufferToTexture(nRpPass& rpPass, const vector4& shadowLightMask);
    /// issue a single general occlusion query
    void IssueOcclusionQuery(Group& group, const vector3& viewerPos);
    /// do a general occlusion query on all root nodes
    void DoOcclusionQuery();
    /// find the N most important shadow casting light sources
    void GatherShadowLights();

    enum
    {
        MaxHierarchyDepth = 64,
        NumBuckets = 64,
        MaxShadowLights = 4,
    };

    static nSceneServer* self;
    static vector3 viewerPos;
    static int sortingOrder;

    nClass* lightNodeClass;
    bool isOpen;
    bool inBeginScene;
    bool obeyLightLinks;
    bool clipPlaneFencing;
    bool gfxServerInBeginScene; // HACK
    bool ffpLightingApplied;
    bool renderDebug;
    bool occlusionQueryEnabled;
    nString renderPathFilename;
    uint stackDepth;
    nRenderPath2 renderPath;

    nFixedArray<int> groupStack;

    nArray<Group> groupArray;
    nArray<LightInfo> lightArray;               // all light sources
    nArray<LightInfo> shadowLightArray;         // shadow casting light sources
    nArray<ushort> rootArray;                   // root nodes
    nArray<ushort> shadowArray;
    nArray<ushort> cameraArray;
    nBucket<ushort,NumBuckets> shapeBucket;     // contains indices of shape nodes, bucketsorted by shader
    
    nWatched dbgNumInstanceGroups;
    nWatched dbgNumInstances;
    nWatched dbgNumOccluded;
    nWatched dbgNumNotOccluded;
    nWatched dbgOccludeViewerInBox;

    nOcclusionQuery* occlusionQuery;

#if __NEBULA_STATS__
    nProfiler profFrame;
    nProfiler profAttach;
    nProfiler profValidateResources;
    nProfiler profSplitNodes;
    nProfiler profComputeScissors;
    nProfiler profSortNodes;
    nProfiler profRenderShadow;
    nProfiler profOcclusion;
    nProfiler profEndScene_TextBuffer;
    nProfiler profEndScene_EndScene;
    nProfiler profEndScene_PresentScene;
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nSceneServer*
nSceneServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetClipPlaneFencing(bool b)
{
    this->clipPlaneFencing = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSceneServer::GetClipPlaneFencing() const
{
    return this->clipPlaneFencing;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nRenderPath2*
nSceneServer::GetRenderPath() const
{
    return &this->renderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetRenderPathFilename(const nString& n)
{
    this->renderPath.SetFilename(n);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nSceneServer::GetRenderPathFilename() const
{
    return this->renderPath.GetFilename();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSceneServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Turn obey light links on/off. When turned off, every shape will
    be lit by every light in the scene. If turned on, the Nebula2 application
    is responsible for establishing bidirectional light links between the
    render context objects.
*/
inline
void
nSceneServer::SetObeyLightLinks(bool b)
{
    this->obeyLightLinks = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSceneServer::GetObeyLightLinks() const
{
    return this->obeyLightLinks;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetModelTransform(const matrix44& m)
{
    this->groupArray.Back().modelTransform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nSceneServer::GetModelTransform() const
{
    return this->groupArray.Back().modelTransform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetRenderDebug(bool b)
{
    this->renderDebug = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSceneServer::GetRenderDebug() const
{
    return this->renderDebug;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetOcclusionQuery(bool b)
{
    this->occlusionQueryEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSceneServer::GetOcclusionQuery() const
{
    return this->occlusionQueryEnabled;
}

//------------------------------------------------------------------------------
#endif
