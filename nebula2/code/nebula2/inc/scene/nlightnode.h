#ifndef N_LIGHTNODE_H
#define N_LIGHTNODE_H
//------------------------------------------------------------------------------
/**
    @class nLightNode

    Base class of scene node which provide lighting information.

    (C) 2003 RadonLabs GmbH
*/
#include "scene/nabstractshadernode.h"

//------------------------------------------------------------------------------
class nLightNode : public nAbstractShaderNode
{
public:
    /// constructor
    nLightNode();
    /// destructor
    virtual ~nLightNode();
    /// return true if node provides lighting information
    virtual bool HasLight() const;
    /// set the user defined shader parameters
    virtual bool RenderLight(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& lightTransform);

    static nKernelServer* kernelServer;
};
//------------------------------------------------------------------------------
#endif



