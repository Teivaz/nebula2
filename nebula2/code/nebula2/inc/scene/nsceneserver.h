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

    /// returns true if scene graph uses a specific shader type (override in subclasses!)
    virtual bool IsShaderUsed(uint fourcc) const;
    /// begin the scene
    virtual void BeginScene(const matrix44& viewer);
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
        MaxGroups = 2024,
        MaxHierarchyDepth = 64,
    };

    nAutoRef<nGfxServer2> refGfxServer;
    bool inBeginScene;
    uint stackDepth;
    Group* groupStack[MaxHierarchyDepth];
    uint numGroups;
    Group groupArray[MaxGroups];
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetModelTransform(const matrix44& m)
{
    n_assert((this->numGroups > 0) && (this->numGroups < MaxGroups));
    this->groupArray[this->numGroups - 1].modelTransform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nSceneServer::GetModelTransform() const
{
    n_assert((this->numGroups > 0) && (this->numGroups < MaxGroups));
    return this->groupArray[this->numGroups - 1].modelTransform;
}

//------------------------------------------------------------------------------
#endif
