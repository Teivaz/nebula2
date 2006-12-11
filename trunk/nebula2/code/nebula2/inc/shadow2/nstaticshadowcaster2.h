#ifndef N_STATICSHADOWCASTER2_H
#define N_STATICSHADOWCASTER2_H
//------------------------------------------------------------------------------
/**
    @class nStaticShadowCaster2
    @ingroup Shadow2

    A shadow caster for static geometry.

    (C) 2005 Radon Labs GmbH
*/
#include "shadow2/nshadowcaster2.h"

//------------------------------------------------------------------------------
class nStaticShadowCaster2 : public nShadowCaster2
{
public:
    /// constructor
    nStaticShadowCaster2();
    /// destructor
    virtual ~nStaticShadowCaster2();
    /// setup the shadow volume for rendering
    virtual void SetupShadowVolume(const nLight& light, const matrix44& invModelLightMatrix);
    /// render the shadow volume
    virtual void RenderShadowVolume();

protected:
    /// override in subclass to perform actual resource loading
    virtual bool LoadResource();
    /// override in subclass to perform actual resource unloading
    virtual void UnloadResource();

    nRef<nMesh2> refMesh;   // contains the shadow mesh data
};
//------------------------------------------------------------------------------
#endif
