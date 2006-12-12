#ifndef N_SHADOWSKINSHAPENODE_H
#define N_SHADOWSKINSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nShadowSkinShapeNode
    @ingroup Scene

    @brief A shadow skin node is 'visible' object that provide shadow to
    the scene.

    It can be animated like a skinshape node, and provides shadow
    for skinned meshes.

    (C) 2005 RadonLabs GmbH
*/
#include "scene/ntransformnode.h"
#include "shadow2/nskinnedshadowcaster2.h"

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
    virtual bool SaveCmds(nPersistServer* ps);
    /// load resources for this object
    virtual bool LoadResources();
    /// unload resources for this object
    virtual void UnloadResources();
    /// perform per-instance-rendering of shadow
    virtual bool RenderShadow(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& modelMatrix);
    /// return true if node provides shadow
    virtual bool HasShadow() const;
    /// set the mesh resource name
    void SetMesh(const nString& n);
    /// get the mesh resource name
    const nString& GetMesh() const;
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
    static const float maxDistance;
    nString meshName;
    int groupIndex;
    nRef<nSkinnedShadowCaster2> refShadowCaster;
    nDynAutoRef<nSkinAnimator> refSkinAnimator;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowSkinShapeNode::SetMesh(const nString& n)
{
    this->meshName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nShadowSkinShapeNode::GetMesh() const
{
    return this->meshName;
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
/**
    Set relative path to the skin animator object.
*/
inline
void
nShadowSkinShapeNode::SetSkinAnimator(const char* path)
{
    n_assert(path);
    this->refSkinAnimator = path;
}

//------------------------------------------------------------------------------
/**
    Get relative path to the skin animator object
*/
inline
const char*
nShadowSkinShapeNode::GetSkinAnimator() const
{
    return this->refSkinAnimator.getname();
}

//------------------------------------------------------------------------------
/**
    Update the pointer to an uptodate nCharSkeleton object. This pointer
    is provided by the nSkinAnimator object and is routed to the
    nCharSkinRenderer so that the mesh can be properly deformed.
*/
inline
void
nShadowSkinShapeNode::SetCharSkeleton(const nCharSkeleton* charSkeleton)
{
    n_assert(charSkeleton);
    this->refShadowCaster->SetCharSkeleton(charSkeleton);
}

//------------------------------------------------------------------------------
#endif
