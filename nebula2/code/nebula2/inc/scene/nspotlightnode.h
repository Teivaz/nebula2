#ifndef N_SPOTLIGHTNODE_H
#define N_SPOTLIGHTNODE_H
//------------------------------------------------------------------------------
/**
    @class nSpotLightNode
    @ingroup NebulaSceneLightNodes
    @brief A light node with a projection matrix describing a pyramidal
    light cone.

    See also @ref N2ScriptInterface_nspotlightnode

    (C) 2003 RadonLabs GmbH
*/
#include "scene/nlightnode.h"
#include "gfx2/ncamera2.h"

//------------------------------------------------------------------------------
class nSpotLightNode : public nLightNode
{
public:
    /// constructor
    nSpotLightNode();
    /// destructor
    virtual ~nSpotLightNode();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// set the volume projection matrix in the current shader
    virtual bool RenderLight(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& lightTransform);
    /// set perspective parameters
    void SetPerspective(float aov, float aspect, float nearp, float farp);
    /// set orthogonal parameters
    void SetOrthogonal(float w, float h, float nearp, float farp);
    /// set the cone definition directly
    void SetCamera(const nCamera2& c);
    /// get the cone definition
    const nCamera2& GetCamera() const;

private:
    nCamera2 camera;
};
//------------------------------------------------------------------------------
#endif
