#ifndef N_TERRAINGRASSNODE_H
#define N_TERRAINGRASSNODE_H
//------------------------------------------------------------------------------
/**
    @class nTerrainGrassNode
    @ingroup NCTerrain2

    @brief A very specialized terrain grass scene node.

    Receives a list of triangles to render as grass from the application
    through a render context variable. The triangles will be subdivided and
    rendered as point sprites.
    
    (C) 2003 RadonLabs GmbH
*/
#include "scene/nmaterialnode.h"

class nMesh2;
class nTexture2;

//------------------------------------------------------------------------------
class nTerrainGrassNode : public nMaterialNode
{
public:
    /// constructor
    nTerrainGrassNode();
    /// destructor
    virtual ~nTerrainGrassNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// indicate to scene server that we offer geometry for rendering
    virtual bool HasGeometry() const;
    /// render geometry
    virtual bool RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext);

    /// rendering attrs structure (will be transfered from the app through a rendercontext variable)
    struct Attrs
    {
        nMesh2* mesh;
        nTexture2* texture;
        nFloat4 texGenS;
        nFloat4 texGenT;
    };

private:
    nVariable::Handle attrsVarHandle;        // must point to a valid nMesh2
};

//------------------------------------------------------------------------------
#endif    

