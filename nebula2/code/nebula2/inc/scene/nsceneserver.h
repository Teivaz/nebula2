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
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nSceneServer
#include "kernel/ndefdllclass.h"

class nRenderContext;
class nSceneNode;
class nGfxServer2;

//------------------------------------------------------------------------------
class nSceneServer : public nRoot
{
public:
    /// constructor
    nSceneServer();
    /// destructor
    virtual ~nSceneServer();

    /// begin the scene
    virtual void BeginScene(const matrix44& viewer);
    /// attach the toplevel object of a scene node hierarchy to the scene
    virtual void Attach(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// finish the scene
    virtual void EndScene();
    /// render the scene
    virtual void RenderScene();
    /// get the view matrix as defined in BeginScene()
    const matrix44& GetViewMatrix() const;
    /// get the inverse of the view matrix as defined in BeginScene()
    const matrix44& GetInvViewMatrix() const;
    /// set current modelview matrix
    void SetModelView(const matrix44& m);
    /// get current modelview matrix
    const matrix44& GetModelView() const;
    /// begin a group node
    void BeginGroup(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// finish current group node
    void EndGroup();

    static nKernelServer* kernelServer;

protected:

    class Group
    {
    public:
        Group* parent;
        nRenderContext* renderContext;
        nSceneNode* sceneNode;
        matrix44 modelView;
    };

    enum
    {
        MAX_GROUPS = 2024,
        MAX_HIERARCHY_DEPTH = 64,
    };

    nAutoRef<nGfxServer2> refGfxServer;
    bool inBeginScene;

    uint numGroups;
    Group groupArray[MAX_GROUPS];

    uint stackDepth;
    Group* groupStack[MAX_HIERARCHY_DEPTH];
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetModelView(const matrix44& m)
{
    n_assert((this->numGroups > 0) && (this->numGroups < MAX_GROUPS));
    this->groupArray[this->numGroups - 1].modelView = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nSceneServer::GetModelView() const
{
    n_assert((this->numGroups > 0) && (this->numGroups < MAX_GROUPS));
    return this->groupArray[this->numGroups - 1].modelView;
}

//------------------------------------------------------------------------------
#endif
