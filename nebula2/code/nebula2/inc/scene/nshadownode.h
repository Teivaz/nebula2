#ifndef N_SHADOWNODE_H
#define N_SHADOWNODE_H
//------------------------------------------------------------------------------
/**
    @class nShadowNode
    @ingroup SceneNodes

    @brief A shadow node is 'visible' object that provide shadow to the scene.

    It is derived from nTransformNode so it inherits transform information. 
    It adds a simple shadows mesh geometry which can rendered through a
    nShadowCaster.

    (C) 2004 RadonLabs GmbH
*/

#include "scene/ntransformnode.h"
#include "shadow/nshadowserver.h"
#include "shadow/nstaticshadowcaster.h"

//------------------------------------------------------------------------------
class nShadowNode : public nTransformNode
{
public:
    /// constructor
    nShadowNode();
    /// destructor
    virtual ~nShadowNode();
    
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources for this object
    virtual bool LoadResources();
    /// unload resources for this object
    virtual void UnloadResources();

    /// perform pre-instancing rendering of shadow
    virtual bool ApplyShadow(nSceneServer* sceneServer);
    /// perform per-instance-rendering of shadow
    virtual bool RenderShadow(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& modelMatrix);
    /// return true if node provides shadow
    virtual bool HasShadow() const;

    /// set the mesh resource name
    void SetMesh(const char* name);
    /// get the mesh resource name
    const char* GetMesh() const;
    /// set the mesh group index
    void SetGroupIndex(int i);
    /// get the mesh group index
    int GetGroupIndex() const;

protected:
    /// setup the shadow caster
    bool LoadShadowCaster();
    /// cleanup the shadow caster
    void UnloadShadowCaster();

    // HACK!
    static const float maxSmallObjectDistance;

    nString meshName;
    int groupIndex;

    nRef<nStaticShadowCaster> refShadowCaster;
};

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nShadowNode::GetMesh() const
{
    return this->meshName.IsEmpty() ? 0 : this->meshName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowNode::SetGroupIndex(int i)
{
    this->groupIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShadowNode::GetGroupIndex() const
{
    return this->groupIndex;
}
//------------------------------------------------------------------------------
#endif

