#ifndef N_SHAPENODE_H
#define N_SHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nShapeNode
    @ingroup NebulaSceneSystemNodes

    @brief A shape node is the simplest actually visible object in the
    scene node class hierarchy.
    
    It is derived from nMaterialNode, and thus inherits transform and
    shader information. It adds a simple mesh which it can render.

    See also @ref N2ScriptInterface_nshapenode
    
    (C) 2002 RadonLabs GmbH
*/
#include "scene/nmaterialnode.h"
#include "gfx2/nmesh2.h"
#include "kernel/ndynautoref.h"

//------------------------------------------------------------------------------
class nShapeNode : public nMaterialNode
{
public:
    /// constructor
    nShapeNode();
    /// destructor
    virtual ~nShapeNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// indicate to scene server that we offer geometry for rendering
    virtual bool HasGeometry() const;
    /// perform pre-instancing rending of geometry
    virtual bool ApplyGeometry(nSceneServer* sceneServer);
    /// perform per-instance-rendering of geometry
    virtual bool RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext);
    /// get the mesh usage flags required by this shape node
    int GetMeshUsage() const;
    /// override the default mesh usage for this shape node
    void SetMeshUsage(int);

    /// set the mesh resource name
    void SetMesh(const char* name);
    /// get the mesh resource name
    const char* GetMesh() const;
    /// set the mesh group index
    void SetGroupIndex(int i);
    /// get the mesh group index
    int GetGroupIndex() const;
    /// set the NOH path to the mesh's resource loader
    void SetMeshResourceLoader(const char* resourceLoaderPath);
    /// get the mesh's resource loader
    const char* GetMeshResourceLoader();

protected:
    /// load mesh resource
    bool LoadMesh();
    /// unload mesh resource
    void UnloadMesh();

    nRef<nMesh2> refMesh;
    nString meshName;
    int groupIndex;
    nDynAutoRef<nResourceLoader> refMeshResourceLoader;
    int meshUsage;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nShapeNode::SetGroupIndex(int i)
{
    this->groupIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShapeNode::GetGroupIndex() const
{
    return this->groupIndex;
}

//------------------------------------------------------------------------------
#endif

