#ifndef N_TEXTSHAPENODE_H
#define N_TEXTSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nTextShapeNode
    @ingroup Scene

    A simple shape node for texts
    
    (C) 2006 RadonLabs GmbH
*/
#include "scene/nshapenode.h"
#include "gfx2/nfont2.h"

//------------------------------------------------------------------------------
class nTextShapeNode : public nShapeNode
{
public:
    /// constructor
    nTextShapeNode();
    /// destructor
    virtual ~nTextShapeNode();

    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// perform pre-instancing rending of geometry
    virtual bool ApplyGeometry(nSceneServer* sceneServer);
    /// render the geometry
    virtual bool RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext);

protected:
    /// load font resource
    bool LoadFont();
    /// unload font resource
    void UnloadFont();
    /// update the screen space rectangle
    void UpdateScreenSpaceRect();

    nRef<nFont2>    refFont;

    uint        fontFlags;
    rectangle   screenSpaceRect;
    nClass*     transformNodeClass;
};

#endif