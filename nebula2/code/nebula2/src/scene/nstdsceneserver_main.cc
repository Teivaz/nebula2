#define N_IMPLEMENTS nStdSceneServer
//------------------------------------------------------------------------------
//  nstdsceneserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nstdsceneserver.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nscenenode.h"

nNebulaClass(nStdSceneServer, "nsceneserver");

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::nStdSceneServer() :
    refGfxServer(kernelServer, "/sys/servers/gfx"),
    numLights(0),
    numShapes(0),
    numShapeLights(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::~nStdSceneServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Split the collected scene nodes into light and shape nodes. Fills
    the lightArray[] and shapeArray[] members.
*/
void
nStdSceneServer::SplitNodes()
{
    this->numLights = 0;
    this->numShapes = 0;

    ushort i;
    for (i = 0; i < this->numGroups; i++)
    {
        Group& group = this->groupArray[i];
        n_assert(group.sceneNode);

        if (group.sceneNode->HasGeometry())
        {
            this->shapeArray[this->numShapes++] = i;
        }
        if (group.sceneNode->HasLightVolume())
        {
            this->lightArray[this->numLights++] = i;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Collect all light nodes which intersect the given shape into the
    shapeLightArray member.
*/
void
nStdSceneServer::CollectShapeLights(const Group& shapeGroup)
{
    // get the shape's bounding box and transform it to 
    // view space
    // FIXME: this would only work for static shapes!
    // -> the bbox should be updated at transform time???
    bbox3 shapeBox = shapeGroup.sceneNode->GetBoundingBox();
    shapeBox.transform(shapeGroup.modelView);

    int curLight;
    bbox3 lightBox;
    this->numShapeLights = 0;
    for (curLight = 0; (curLight < this->numLights) && (this->numShapeLights < MAXLIGHTSPERSHAPE); curLight++)
    {
        ushort lightIndex = this->lightArray[curLight];
        Group& lightGroup = this->groupArray[lightIndex];

        // generate a bounding box in view space around the light's volume
        // (the volume is defined by the modelview matrix of the light)
        lightBox.set(lightGroup.modelView);

        // check for intersection
        if (lightBox.intersects(shapeBox))
        {
            this->shapeLightArray[this->numShapeLights++] = lightIndex;
        }
    }
}

//------------------------------------------------------------------------------
/**
    FIXME: 
    - implement simple viewport culling (?)
    - implement shape/lightsource intersection
    - implement sorting for minimal render state switches
*/
void
nStdSceneServer::RenderScene()
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // clear scene
    gfxServer->Clear(nGfxServer2::ALL);

    // split nodes into light and shape nodes
    this->SplitNodes();

    // render shape objects (unsorted!)
    int shapeIndex;
    for (shapeIndex = 0; shapeIndex < this->numShapes; shapeIndex++)
    {
        Group& shapeGroup = this->groupArray[this->shapeArray[shapeIndex]];

        // collect all light nodes which intersect this shape
        this->CollectShapeLights(shapeGroup);

        // set the modelview matrix for the shape
        gfxServer->SetTransform(nGfxServer2::MODELVIEW, shapeGroup.modelView);

        // prepare shader
        if (shapeGroup.sceneNode->HasShader())
        {
            shapeGroup.sceneNode->RenderShader(this, shapeGroup.renderContext);
        }

        // prepare geometry
        shapeGroup.sceneNode->RenderGeometry(this, shapeGroup.renderContext);

        // one render pass for each intersecting light
        if (this->numShapeLights > 0)
        {
            int shapeLightIndex;
            for (shapeLightIndex = 0; shapeLightIndex < this->numShapeLights; shapeLightIndex++)
            {
                Group& lightGroup = this->groupArray[this->shapeLightArray[shapeLightIndex]];
                
                // update light
                lightGroup.sceneNode->RenderLightVolume(this, lightGroup.renderContext, lightGroup.modelView);

                // render!
                gfxServer->Draw();
            }
        }
        else
        {
            // special case: no lights intersect this object...
            gfxServer->Draw();
        }
    }
}
    