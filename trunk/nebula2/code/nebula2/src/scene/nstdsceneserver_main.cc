#define N_IMPLEMENTS nStdSceneServer
//------------------------------------------------------------------------------
//  nstdsceneserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nstdsceneserver.h"
#include "scene/nscenenode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ndisplaymode2.h"
#include "gfx2/nmesh2.h"
#include "variable/nvariableserver.h"

nNebulaClass(nStdSceneServer, "nsceneserver");

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::nStdSceneServer() :
    numLights(0),
    numShapes(0),
    numShapeLights(0)
{
    memset(this->lightArray, 0, sizeof(this->lightArray));
    memset(this->shapeArray, 0, sizeof(this->shapeArray));
    memset(this->shapeLightArray, 0, sizeof(this->shapeLightArray));
}

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::~nStdSceneServer()
{
    //empty
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
        if (group.sceneNode->HasLight())
        {
            this->lightArray[this->numLights++] = i;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render the shape objects in the scene, using the shader defined by
    the fourcc code. If no such shader exists, no rendering is done.
*/
void
nStdSceneServer::RenderShapes(uint shaderFourCC)
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // for each shape object...
    int i;
    for (i = 0; i < this->numShapes; i++)
    {
        Group& group = this->groupArray[this->shapeArray[i]];
        if (group.sceneNode->HasShader(shaderFourCC))
        {
            gfxServer->SetTransform(nGfxServer2::Model, group.modelTransform);
            group.sceneNode->RenderShader(shaderFourCC, this, group.renderContext);
            group.sceneNode->RenderGeometry(this, group.renderContext);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render all light/shape interactions matching a given fourcc shader code.

    @todo the intersection check happens once for the diffuse and
    specular pass, the information which lights intersect which
    shapes should be collected and stored once during SplitNodes()!
*/
void
nStdSceneServer::RenderLightShapes(uint shaderFourCC)
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // for each shape object...
    int curShapeIndex;
    for (curShapeIndex = 0; curShapeIndex < this->numShapes; curShapeIndex++)
    {
        Group& shapeGroup = this->groupArray[this->shapeArray[curShapeIndex]];
        if (shapeGroup.sceneNode->HasShader(shaderFourCC))
        {
            // set the modelview matrix for the shape
            gfxServer->SetTransform(nGfxServer2::Model, shapeGroup.modelTransform);

            // prepare shader and geometry
            shapeGroup.sceneNode->RenderShader(shaderFourCC, this, shapeGroup.renderContext);

            // render geometry for each light intersecting this shape...
            int shapeLightIndex;
            for (shapeLightIndex = 0; shapeLightIndex < this->numShapeLights; shapeLightIndex++)
            {
                Group& lightGroup = this->groupArray[this->shapeLightArray[shapeLightIndex]];

                // update light parameters
                lightGroup.sceneNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);

                // render!
                shapeGroup.sceneNode->RenderGeometry(this, shapeGroup.renderContext);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render the scene. Rendering happens in multiple passes:

     -# clear the screen
     -# render scene into the depth buffer only
     -# render diffuse lighting 
     -# render pixel colors and modulate into frame buffer
     -# render specular light and add onto frame buffer
     -# apply various post-special-effects
*/
void
nStdSceneServer::RenderScene()
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // split shape and light nodes
    this->SplitNodes();

    // start rendering to the frame buffer
    if (gfxServer->BeginScene()) 
    {
        gfxServer->Clear(nGfxServer2::AllBuffers, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f, 0);

        // clear, render depth, diffuse, color and specular into the back buffer
        this->RenderShapes(FOURCC('dept'));
        this->RenderLightShapes(FOURCC('diff'));
        this->RenderShapes(FOURCC('colr'));
        this->RenderLightShapes(FOURCC('spec'));

        // finish the frame
        gfxServer->DrawTextBuffer();
        gfxServer->EndScene();
        gfxServer->PresentScene();
    }
}
