#ifndef N_SCENENODE_H
#define N_SCENENODE_H
//------------------------------------------------------------------------------
/**
    The nSceneNode is the base class of all objects which can be attached
    to a scene managed by the nSceneServer class. A scene node object
    may provide transform, geometry, shader and volume information.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nSceneNode
#include "kernel/ndefdllclass.h"

class nSceneServer;
class nRenderContext;
class nGfxServer2;
class nAnimator;

//-------------------------------------------------------------------------------
class nSceneNode : public nRoot
{
public:
    /// constructor
    nSceneNode();
    /// destructor
    virtual ~nSceneNode();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// load resources for this object
    virtual bool LoadResources();
    /// unload resources for this object
    virtual void UnloadResources();
    /// return true if resources for this object are valid
    virtual bool AreResourcesValid() const;
    /// called by app when new render context has been created for this object
    virtual void RenderContextCreated(nRenderContext* renderContext);
    /// called by app when render context is going to be released
    virtual void RenderContextDestroyed(nRenderContext* renderContext);
    /// called by nSceneServer when object is attached to scene
    virtual void Attach(nSceneServer* sceneServer, nRenderContext* renderContext);
    /// return true if node provides transformion
    virtual bool HasTransform() const;
    /// return true if node provides geometry
    virtual bool HasGeometry() const;
    /// return true if node provides shader
    virtual bool HasShader(uint fourcc) const;
    /// return true if node provides a light volume
    virtual bool HasLightVolume() const;
    /// render transformation
    virtual void RenderTransform(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& parentMatrix);
    /// render geometry
    virtual void RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext);
    /// render shader
    virtual void RenderShader(uint fourcc, nSceneServer* sceneServer, nRenderContext* renderContext);
    /// render light volume
    virtual void RenderLightVolume(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& lightModelView);
    /// get the node's bounding box
    const bbox3& GetBoundingBox() const;
    /// add an animator object
    void AddAnimator(const char* path);
    /// get number of animator objects
    int GetNumAnimators() const;
    /// get animator object at index
    const char* GetAnimatorAt(int index);
    /// invoke all shader animators
    void InvokeShaderAnimators(nRenderContext* renderContext);
    /// invoke all transform animators
    void InvokeTransformAnimators(nRenderContext* renderContext);

    static nKernelServer* kernelServer;

protected:
    /// set the bounding box
    void SetBoundingBox(const bbox3& b);

    bbox3 box;
    nArray< nDynAutoRef<nAnimator> > animatorArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneNode::SetBoundingBox(const bbox3& b)
{
    this->box = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
const bbox3&
nSceneNode::GetBoundingBox() const
{
    return this->box;
}

//------------------------------------------------------------------------------
#endif
