#ifndef N_CRENDERER_H
#define N_CRENDERER_H
//------------------------------------------------------------------------------
/**
    @class CCRenderer
    @ingroup BombsquadBruceContribModule
    @brief The renderer

    CCRenderer sends the visible objects in the world,
    as extracted from the global octree, plus any objects
    that the world knows always to be visible (such as 
    the player's avatar) to the scene server for display.

    (C)	2003	Rafael Van Daele-Hunt
*/
#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include "scene/nrendercontext.h"
#include "BombsquadBruce/CCRef.h"
class nSceneServer;
class nConServer;
class CCCamera;
class CCRoot;
class CCGuiMgr;
class nGuiServer;
class CCCullingMgr;

//------------------------------------------------------------------------------
class CCRenderer : public nRoot
{  
public:
    typedef CCRef<CCRoot> RenderObject;

    CCRenderer();
    virtual ~CCRenderer();
    virtual bool SaveCmds(nPersistServer* persistServer);
    void Render(const matrix44& viewTransform, CCCamera& cullCamera, const nArray< CCRef<CCRoot> >& alwaysVisibleObjects, bool debugVisualization );

    // Script commands
    void SetScene(const char* scene_path); 		/// Set m_StaticScene
private:
    uint m_FrameId;
    nAutoRef<nSceneServer> m_SceneServer;
    nAutoRef<nConServer> m_ConsoleServer;
    nRenderContext m_StaticSceneContext; /// lights, sun, skybox -- everything that is always rendered, rather than passed in by the world
    nAutoRef<CCCullingMgr> m_rCullingMgr;
};
//------------------------------------------------------------------------------

#endif

