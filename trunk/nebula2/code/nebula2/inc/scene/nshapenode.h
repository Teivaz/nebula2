#ifndef N_SHAPENODE_H
#define N_SHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nShapeNode
    @ingroup NebulaSceneSystemNodes

    A shape node is the simplest actually visible object in the scene node
    class hierarchy. It is derived from nMaterialNode, and thus inherits
    transform and shader information. It adds a simple mesh which it 
    can render.

    See also @ref N2ScriptInterface_nshapenode
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_MATERIALNODE_H
#include "scene/nmaterialnode.h"
#endif

#ifndef N_MESH2_H
#include "gfx2/nmesh2.h"
#endif

#undef N_DEFINES
#define N_DEFINES nShapeNode
#include "kernel/ndefdllclass.h"

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
    /// return true if resources for this object are valid
    virtual bool AreResourcesValid() const;

    /// indicate to scene server that we offer geometry for rendering
    virtual bool HasGeometry() const;
    /// render geometry
    virtual void RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext);

    /// set the mesh resource name
    void SetMesh(const char* name);
    /// get the mesh resource name
    const char* GetMesh() const;
    /// set the mesh group index
    void SetGroupIndex(int i);
    /// get the mesh group index
    int GetGroupIndex() const;

    /// turn wireframe rendering on/off
    void SetRenderWireframe(bool b);
    /// get wireframe rendering flag
    bool GetRenderWireframe() const;

    static nKernelServer* kernelServer;

protected:
    /// load mesh resource
    bool LoadMesh();
    /// unload mesh resource
    void UnloadMesh();
    /// extract the mesh filename from the resource name
    const char* ExtractFileName(const char* resourceName, char* buf, int bufSize);
    /// extract the group name from the resource name
    const char* ExtractGroupName(const char* resourceName);

    nRef<nMesh2> refMesh;
    nString meshName;
    int groupIndex;
    nMesh2::Usage meshUsage;
    bool renderWireframe;
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
/**
*/
inline
void
nShapeNode::SetRenderWireframe(bool b)
{
    this->renderWireframe = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShapeNode::GetRenderWireframe() const
{
    return this->renderWireframe;
}

//------------------------------------------------------------------------------
#endif

