#ifndef N_CRENDERER_H
#define N_CRENDERER_H
//------------------------------------------------------------------------------
/**
    @class BBRenderer
    @ingroup BombsquadBruceContribModule
    @brief The renderer

    BBRenderer sends the visible objects in the world,
    as extracted from the global octree, plus any objects
    that the world knows always to be visible (such as 
    the player's avatar) to the scene server for display.

    (C)	2003	Rafael Van Daele-Hunt
*/
#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include "scene/nrendercontext.h"
#include "BombsquadBruce/BBRef.h"
class nSceneServer;
class nConServer;
class BBCamera;
class BBRoot;
class BBGuiMgr;
class nGuiServer;
class BBCullingMgr;

//------------------------------------------------------------------------------
class BBRenderer : public nRoot
{  
public:
    typedef BBRef<BBRoot> RenderObject;

    BBRenderer();
    virtual ~BBRenderer();
    virtual bool SaveCmds(nPersistServer* persistServer);
    void Render(const matrix44& viewTransform, BBCamera& cullCamera, const nArray< BBRef<BBRoot> >& alwaysVisibleObjects, bool debugVisualization );

    // Script commands
    void SetScene(const char* scene_path); 		/// Set m_StaticScene
private:
    uint m_FrameId;
    nAutoRef<nSceneServer> m_SceneServer;
    nAutoRef<nConServer> m_ConsoleServer;
    nRenderContext m_StaticSceneContext; /// lights, sun, skybox -- everything that is always rendered, rather than passed in by the world
    nAutoRef<BBCullingMgr> m_rCullingMgr;
};
//------------------------------------------------------------------------------

#endif

