#ifndef N_SHADOWSKINSHAPENODE_H
#define N_SHADOWSKINSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nShadowSkinShapeNode
    @ingroup Scene

    A shadow skin node is 'visible' object that provide shadow to the scene.
    It can be animated like a skinshape node, and provides shadow
    for skinned meshes.

    (C) 2004 RadonLabs GmbH
*/

#include "scene/ntransformnode.h"
#include "shadow/nshadowserver.h"
#include "shadow/nskinshadowcaster.h"

#include "character/ncharjointpalette.h"

class nCharSkeleton;
class nSkinAnimator;

//------------------------------------------------------------------------------
class nShadowSkinShapeNode : public nTransformNode
{
public:
    /// constructor
    nShadowSkinShapeNode();
    /// destructor
    virtual ~nShadowSkinShapeNode();
    
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources for this object
    virtual bool LoadResources();
    /// unload resources for this object
    virtual void UnloadResources();

    /// perform pre-instancing rendering of shadow
    virtual bool ApplyShadow(nSceneServer* sceneServer);
    /// perform per-instance-rendering of shadow
    virtual bool RenderShadow(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& model);
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

    /// set the skin animator
    void SetSkinAnimator(const char* path);
    /// get the skin animator
    const char* GetSkinAnimator() const;
    /// set pointer to an uptodate character skeleton object (called exclusively by nSkinAnimator)
    void SetCharSkeleton(const nCharSkeleton* charSkeleton);

protected:
    /// setup the shadow caster
    bool LoadShadowCaster();
    /// cleanup the shadow caster
    void UnloadShadowCaster();

    nString meshName;
    int groupIndex;

    nRef<nSkinShadowCaster> refShadowCaster;

private:
    static const float maxDistance;
    nDynAutoRef<nSkinAnimator> refSkinAnimator;
    const nCharSkeleton* extCharSkeleton;
};

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nShadowSkinShapeNode::GetMesh() const
{
    return this->meshName.IsEmpty() ? 0 : this->meshName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowSkinShapeNode::SetGroupIndex(int i)
{
    this->groupIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShadowSkinShapeNode::GetGroupIndex() const
{
    return this->groupIndex;
}
//------------------------------------------------------------------------------
#endif