#ifndef N_SCENESERVER_H
#define N_SCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nSceneServer
    @ingroup SceneServers

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
#include "scene/nrenderpath.h"

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
    /// return pointer to instance
    static nSceneServer* Instance();

    /// set the file name for the renderpath.xml file.
    void SetRenderPathFilename(const nString& name);
    /// open the scene server
    bool Open();
    /// close the scene server;
    void Close();
    /// return true if scene server open
    bool IsOpen() const;
    /// set background color
    void SetBgColor(const vector4& c);
    /// get background color
    const vector4& GetBgColor() const;
    /// returns true if scene graph uses a specific shader type (override in subclasses!)
    virtual bool IsShaderUsed(nFourCC fourcc) const;
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
    virtual void SetModelTransform(const matrix44& m);
    /// get current model matrix
    virtual const matrix44& GetModelTransform() const;
    /// begin a group node
    virtual void BeginGroup(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// finish current group node
    virtual void EndGroup();

private:
    static nSceneServer* Singleton;

protected:
    /// split scene nodes into light and shape nodes
    virtual void SplitNodes(nFourCC shaderFourCC);
    /// make sure scene node resources are valid
    void ValidateNodeResources();
    /// sort shape nodes for optimal rendering
    void SortNodes();
    /// the actual sorting call is virtual, since Compare can't be
    virtual void DoSort( ushort* indexPtr, int numIndices );
    /// static qsort() compare function
    static int __cdecl Compare(const ushort* i1, const ushort* i2);
    /// get shader object from shape bucket (may return 0)
    nShader2* GetBucketShader(int bucketIndex, nFourCC fourcc);

    class Group
    {
    public:
        int parentIndex;                // index of parent in group array, or -1
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

    bool isOpen;
    bool inBeginScene;
    uint stackDepth;
    nRenderPath renderPath;
    nFixedArray<int> groupStack;
    nArray<Group> groupArray;
    nArray<ushort> lightArray;
    nArray<ushort> shadowArray;
    nBucket<ushort,32> shapeBucket;                     // contains indices of shape nodes, bucketsorted by shader
    vector4 bgColor;
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
bool
nSceneServer::IsOpen() const
{
    return this->isOpen;
}

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
#endif
