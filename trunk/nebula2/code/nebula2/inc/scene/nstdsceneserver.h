#ifndef N_STDSCENESERVER_H
#define N_STDSCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nStdSceneServer
    @ingroup NebulaSceneSystemServers

    Default scene server. A simple example of how to write a scene server class.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_SCENESERVER_H
#include "scene/nsceneserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_VARIABLE_H
#include "variable/nvariable.h"
#endif

#undef N_DEFINES
#define N_DEFINES nStdSceneServer
#include "kernel/ndefdllclass.h"

class nVariableServer;
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
    /// render the scene
    virtual void RenderScene();

    static nKernelServer* kernelServer;

private:
    enum
    {
        MAXLIGHTSPERSHAPE = 16,
    };

    /// render shape objects in scene
    void RenderShapes(uint shaderFourCC);
    /// render light/shape interaction for all lit shapes
    void RenderLightShapes(uint shaderFourCC);
    /// split scene nodes into light and shape nodes
    void SplitNodes();
    /// collect all lights influencing a given shape
    void CollectShapeLights(const Group& shapeGroup);

    int numLights;
    ushort lightArray[nSceneServer::MAX_GROUPS];    // indices of light nodes in scene

    int numShapes;
    ushort shapeArray[nSceneServer::MAX_GROUPS];    // indices of shape nodes in scene

    int numShapeLights;
    ushort shapeLightArray[MAXLIGHTSPERSHAPE];      // indices of light nodes intersecting a shape
};
//------------------------------------------------------------------------------
#endif


