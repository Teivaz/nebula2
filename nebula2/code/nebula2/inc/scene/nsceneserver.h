#ifndef N_SCENESERVER_H
#define N_SCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nSceneServer
    @ingroup NebulaSceneSystemServers

    The scene server builds a scene from nSceneNode objects and then
    renders it. The scene is rebuilt every frame, so some sort of
    culling should happen externally before building the scene. 

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "mathlib/matrix.h"
#include "kernel/nautoref.h"
#include "gfx2/nshaderparams.h"
#include "util/nbucket.h"

class nRenderContext;
class nSceneNode;
class nGfxServer2;
class nShader2;

//------------------------------------------------------------------------------
class nSceneServer : public nRoot
{
public:
    /// constructor
    nSceneServer();
    /// destructor
    virtual ~nSceneServer();

    /// set background color
    void SetBgColor(const vector4& c);
    /// get background color
    const vector4& GetBgColor() const;
    /// returns true if scene graph uses a specific shader type (override in subclasses!)
    virtual bool IsShaderUsed(uint fourcc) const;
    /// begin the scene
    virtual bool BeginScene(const matrix44& viewer);
    /// attach the toplevel object of a scene node hierarchy to the scene
    virtual void Attach(nRenderContext* renderContext);
    /// finish the scene
    virtual void EndScene();
    /// render the scene
    virtual void RenderScene();
    /// present the scene
    virtual void PresentScene();
    /// set current model matrix
    void SetModelTransform(const matrix44& m);
    /// get current model matrix
    const matrix44& GetModelTransform() const;
    /// begin a group node
    void BeginGroup(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// finish current group node
    void EndGroup();

protected:
    /// transfer standard parameters to shader (matrices, etc...)
    virtual void UpdateShader(nShader2* shd, nRenderContext* renderContext);
    /// split scene nodes into light and shape nodes
    void SplitNodes(uint shaderFourCC);
    /// make sure scene node resources are valid
    void ValidateNodeResources();
    /// sort shape nodes for optimal rendering
    void SortNodes();
    /// static qsort() compare function
    static int __cdecl Compare(const ushort* i1, const ushort* i2);
    /// get shader object from shape bucket (may return 0)
    nShader2* GetBucketShader(int bucketIndex, uint fourcc);

    class Group
    {
    public:
        Group* parent;
        nRenderContext* renderContext;
        nSceneNode* sceneNode;
        matrix44 modelTransform;
    };

    enum
    {
        MaxHierarchyDepth = 64,
    };

    static nSceneServer* self;
    static vector3 viewerPos;

    nAutoRef<nGfxServer2> refGfxServer;
    bool inBeginScene;
    uint stackDepth;
    Group* groupStack[MaxHierarchyDepth];
    nArray<Group> groupArray;
    nArray<ushort> lightArray;
    nBucket<ushort,32> shapeBucket;   ///< contains indices of shape nodes, bucketsorted by shader
    vector4 bgColor;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetBgColor(const vector4& c)
{
    this->bgColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nSceneServer::GetBgColor() const
{
    return this->bgColor;
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
#endif
