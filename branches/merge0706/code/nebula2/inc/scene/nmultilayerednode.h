#ifndef N_MULTILAYEREDNODE_H
#define N_MULTILAYEREDNODE_H
//------------------------------------------------------------------------------
/**
    @class nMultiLayeredNode
    @ingroup Scene

    (C) 2005 RadonLabs GmbH
*/
#include "scene/nshapenode.h"
//#include "scene/nscenenode.h"

class nRenderContext;
//------------------------------------------------------------------------------
//class nMultiLayeredNode : public nSceneNode
class nMultiLayeredNode : public nShapeNode
{
public:
    /// constructor
    nMultiLayeredNode();
    /// destructor
    virtual ~nMultiLayeredNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// render geometry
    virtual bool ApplyShader(nSceneServer* sceneServer);

    /// set uv stretch factor 
    void SetUVStretch(int i, float val);
    /// set layer specular intensity
    void SetSpecIntensity(int i, float val);

protected:
    static const int MaxLayers = 8;
    float uvStretch[MaxLayers];
    float specIntensity[MaxLayers];
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMultiLayeredNode::SetUVStretch(int i, float value)
{
    n_assert((i >= 0) && (i < MaxLayers));
    this->uvStretch[i] = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMultiLayeredNode::SetSpecIntensity(int i, float value)
{
    n_assert((i >= 0) && (i < MaxLayers));
    this->specIntensity[i] = value;
}

//------------------------------------------------------------------------------
#endif
