#ifndef N_STDSCENESERVER_H
#define N_STDSCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nStdSceneServer
    @ingroup NebulaSceneSystemServers

    Default scene server. A simple example of how to write a scene server class.

    (C) 2003 RadonLabs GmbH
*/
#include "scene/nsceneserver.h"
#include "kernel/nautoref.h"
#include "variable/nvariable.h"

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
    /// present the scene
    virtual void PresentScene();

    static nKernelServer* kernelServer;

protected:
    /// split scene nodes into light and shape nodes
    virtual void SplitNodes();
    /// sort shape nodes for optimal rendering
    virtual void SortNodes();
    /// render light/shape interaction for all lit shapes
    virtual void RenderLightShapes(uint shaderFourCC, ushort* shapeArray, int numShapes );

    int numLights;
    int numShapes;

    ushort lightArray[nSceneServer::MaxGroups];    // indices of light nodes in scene
    ushort shapeArray[nSceneServer::MaxGroups];    // indices of shape nodes in scene

    /// static qsort() compare function
    static int __cdecl Compare(const ushort* i1, const ushort* i2);

    // Used for sorting.
    static vector3 viewerPos;
private:
    /// initialize required resources
    bool LoadResources();
    /// unload resources
    void UnloadResources();
    /// check if resources are valid
    bool AreResourcesValid();

    enum
    {
        MaxLightsPerShape = 2
    };

    // Used for sorting.
    static nStdSceneServer* self;
};
//------------------------------------------------------------------------------
#endif


