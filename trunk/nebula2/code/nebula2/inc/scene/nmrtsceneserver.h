#ifndef N_MRTSCENESERVER_H
#define N_MRTSCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nMTRSceneServer
    @ingroup SceneServers

    @brief "Multiple Render Target" scene server, spreads rendering across
    various specialized offscreen buffers, which are combined to the final
    result by specific "compositing" pixel shaders.

    (C) 2003 RadonLabs GmbH
*/
#include "scene/nsceneserver.h"
#include "kernel/nautoref.h"
#include "variable/nvariable.h"
#include "gfx2/nshaderparams.h"
#include "misc/nwatched.h"
#include "shadow/nshadowserver.h"

class nTexture2;
class nShader2;
class nMesh2;

//------------------------------------------------------------------------------
class nMRTSceneServer : public nSceneServer
{
public:
    /// constructor
    nMRTSceneServer();
    /// destructor
    virtual ~nMRTSceneServer();
    /// open the scene server
    virtual bool Open();
    /// begin the scene
    virtual bool BeginScene(const matrix44& viewer);
    /// render the scene
    virtual void RenderScene();
    /// present the scene
    virtual void PresentScene();

private:
    /// do the render path rendering
    void DoRenderPath();
    /// render shadow
    void RenderShadow();
    
    nAutoRef<nShadowServer> refShadowServer;
    nWatched dbgNumInstanceGroups;
    nWatched dbgNumInstances;
    bool gfxServerInBeginScene; // HACK
};
//------------------------------------------------------------------------------
#endif


