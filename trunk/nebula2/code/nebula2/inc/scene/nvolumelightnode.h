#ifndef N_VOLUMELIGHTNODE_H
#define N_VOLUMELIGHTNODE_H
//------------------------------------------------------------------------------
/**
    @class nVolumeLightNode
    @ingroup NebulaSceneSystemNodes

    A nVolumeLightNode defines a bounding box volume, influencing the
    lighting of shape nodes that intersect. Lighting happens inside
    RenderLightVolume(), which will modify the light parameters 
    in the current shader. Note that the scaling component of the 
    superclass nTransformNode defines the size of the bounding box volume.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_ABSTRACTSHADERNODE_H
#include "scene/nabstractshadernode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nMaterialNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nVolumeLightNode : public nAbstractShaderNode
{
public:
    /// constructor
    nVolumeLightNode();
    /// destructor
    virtual ~nVolumeLightNode();
    /// load resources
    virtual bool LoadResources();
    /// return true if resources for this object are valid
    virtual bool AreResourcesValid() const;

    /// return true if node provides a light volume
    virtual bool HasLightVolume() const;
    /// render light volume
    virtual void RenderLightVolume(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& lightModelView);

    static nKernelServer* kernelServer;

private:
    nVariable::Handle modelLightPosVarHandle;   // handle of "modelLightPos" variable
};
//------------------------------------------------------------------------------
#endif
