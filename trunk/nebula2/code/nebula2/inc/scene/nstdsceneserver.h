#ifndef N_STDSCENESERVER_H
#define N_STDSCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nStdSceneServer
    @ingroup SceneServers

    @brief Default scene server. A simple example of how to write a scene
    server class.

    (C) 2003 RadonLabs GmbH
*/
#include "scene/nsceneserver.h"
#include "kernel/nautoref.h"
#include "variable/nvariable.h"
#include "misc/nwatched.h"

class nTexture2;
class nShader2;
class nMesh2;

//------------------------------------------------------------------------------
class nStdSceneServer : public nSceneServer
{
public:
    /// constructor
    nStdSceneServer();
    /// destructor
    virtual ~nStdSceneServer();
    /// begin the scene
    virtual bool BeginScene(const matrix44& viewer);
    /// render the scene
    virtual void RenderScene();
    /// present the scene
    virtual void PresentScene();

protected:
    /// initialize required resources
    bool LoadResources();
    /// unload resources
    void UnloadResources();
    /// check if resources are valid
    bool AreResourcesValid();
    /// do the render path rendering
    void DoRenderPath();
    /// render shadow
    void RenderShadow();

    nWatched dbgNumInstanceGroups;
    nWatched dbgNumInstances;
};

//------------------------------------------------------------------------------
#endif
