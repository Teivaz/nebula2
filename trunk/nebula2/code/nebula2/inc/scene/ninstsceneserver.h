#ifndef N_INSTSCENESERVER_H
#define N_INSTSCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nInstSceneServer
    @ingroup SceneServers

    @brief An experimental scene server which uses instance streams for
    rendering identical objects.

    The instance server's central data structure is this:

    ShaderBucket
        |
        +- Shader:  - SceneNode - SceneNode - SceneNode -
        |                 |           |           |
        |             RenderCxt   RenderCxt   RenderCxt
        |             RenderCxt   RenderCxt   RenderCxt
        |             RenderCxt   RenderCxt   RenderCxt
        |                ...         ...         ...
        |
        +- Shader:  - SceneNode - SceneNode - SceneNode -
        |                 |           |           |
        |             RenderCxt   RenderCxt   RenderCxt
        |             RenderCxt   RenderCxt   RenderCxt
        |                ...         ...         ...
        ...

    This allows a tree traversal for optimal rendering 
    (with minimal shader state switches).

    (C) 2004 RadonLabs GmbH
*/
#include "scene/nsceneserver.h"

//------------------------------------------------------------------------------
class nInstSceneServer : public nSceneServer
{
public:
    /// constructor
    nInstSceneServer();
    /// destructor
    virtual ~nInstSceneServer();
    /// begin the scene
    virtual bool BeginScene(const matrix44& viewer);
    /// attach the toplevel object of a scene node hierarchy to the scene
    virtual void Attach(nRenderContext* renderContext);
    /// finish the scene
    virtual void EndScene();
    /// render the scene
    virtual void RenderScene();
    /// present the scene
    virtual void PresentScene();
    /// begin a group node
    virtual void BeginGroup(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// finish current group node
    virtual void EndGroup();
    /// set current model matrix
    virtual void SetModelTransform(const matrix44& m);
    /// get current model matrix
    virtual const matrix44& GetModelTransform() const;

private:
    /// add instance data from render context to scene node
    void AddInstanceData(nRenderContext* renderContext);

    nBucket<nSceneNode*,32> nodeBucket;
};

//------------------------------------------------------------------------------
#endif

