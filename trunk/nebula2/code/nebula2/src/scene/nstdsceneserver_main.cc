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

// global data for qsort() compare function
nStdSceneServer* nStdSceneServer::self = 0;
vector3 nStdSceneServer::viewerPos;

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::nStdSceneServer() :
    numLights(0),
    numShapes(0)
{
    memset(this->lightArray, 0, sizeof(this->lightArray));
    memset(this->shapeArray, 0, sizeof(this->shapeArray));
    self = this;
}

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::~nStdSceneServer()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
nStdSceneServer::AreResourcesValid()
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nStdSceneServer::UnloadResources()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nStdSceneServer::LoadResources()
{
    return true;
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
    The scene node sorting compare function. The goal is to sort the attached
    shape nodes for optimal rendering performance.

    FIXME: handling of transparent nodes, compare shaders
*/
int
__cdecl
nStdSceneServer::Compare(const ushort* i1, const ushort* i2)
{
    nStdSceneServer* sceneServer = nStdSceneServer::self;
    const nSceneServer::Group& g1 = sceneServer->groupArray[*i1];
    const nSceneServer::Group& g2 = sceneServer->groupArray[*i2];
    int cmp;

    // render priority
    cmp = g1.sceneNode->GetRenderPri() - g2.sceneNode->GetRenderPri();
    if (cmp != 0)
    {
        return cmp;
    }

    // by identical scene node
    cmp = int(g1.sceneNode) - int(g2.sceneNode);
    if (cmp != 0)
    {
        return cmp;
    }

    // distance to viewer (closest first)
    static vector3 dist1;
    static vector3 dist2;
    dist1.set(viewerPos.x - g1.modelTransform.M41,
              viewerPos.y - g1.modelTransform.M42,
              viewerPos.z - g1.modelTransform.M43);
    dist2.set(viewerPos.x - g2.modelTransform.M41,
              viewerPos.y - g2.modelTransform.M42,
              viewerPos.z - g2.modelTransform.M43);
    float diff = dist1.lensquared() - dist2.lensquared();
    if (diff < 0.001f)      return -1;
    else if (diff > 0.001f) return +1;

    // nodes are identical
    return 0;
}

//------------------------------------------------------------------------------
/**
    Sort the indices in the shape array for optimal rendering.
*/
void
nStdSceneServer::SortNodes()
{
    // initialize the static viewer pos vector                                      
    viewerPos = this->refGfxServer->GetTransform(nGfxServer2::InvView).pos_component();

    // call the sorter hook                                                         
    qsort(this->shapeArray, this->numShapes, sizeof(ushort), (int(__cdecl *)(const void *, const void *)) Compare);
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
            nShader2* curShader = gfxServer->GetShader();
            this->UpdateShader(curShader, group.renderContext);
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
            // prepare shader
            if (shapeGroup.sceneNode->RenderShader(shaderFourCC, this, shapeGroup.renderContext))
            {
                nShader2* curShader = gfxServer->GetShader();
                this->UpdateShader(curShader, shapeGroup.renderContext);

                // render the "most important light" into the shader (just the first light)
                if (this->numLights > 0)
                {
                    Group& lightGroup = this->groupArray[this->lightArray[0]];
                    lightGroup.sceneNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);
                }

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

    // make sure all required resources are ok
    if (!this->AreResourcesValid())
    {
        bool success = this->LoadResources();
        n_assert(success);
    }

    // split nodes into shapes and lights
    this->SplitNodes();

    // sort shape nodes for optimal rendering
    this->SortNodes();

    if (gfxServer->BeginScene())
    {
        // clear, render depth, diffuse, color and specular into the back buffer
        gfxServer->Clear(nGfxServer2::AllBuffers, 0.3f, 0.3f, 0.3f, 1.0f, 1.0f, 0);
        this->RenderLightShapes(FOURCC('colr'));
    }
}

//------------------------------------------------------------------------------
/**
    This finalizes rendering and presents the scene to the user.
*/
void
nStdSceneServer::PresentScene()
{
    nGfxServer2* gfxServer = this->refGfxServer.get();
    gfxServer->DrawTextBuffer();
    gfxServer->EndScene();
    gfxServer->PresentScene();
}
