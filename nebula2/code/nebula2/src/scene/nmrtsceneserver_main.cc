#define N_IMPLEMENTS nMRTSceneServer
//------------------------------------------------------------------------------
//  nmrtsceneserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmrtsceneserver.h"
#include "scene/nscenenode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ndisplaymode2.h"
#include "gfx2/nmesh2.h"
#include "variable/nvariableserver.h"

nNebulaClass(nMRTSceneServer, "nsceneserver");

//------------------------------------------------------------------------------
/**
*/
nMRTSceneServer::nMRTSceneServer() :
    refVariableServer(kernelServer, "/sys/servers/variable"),
    offscreenBufferWidth(256),
    offscreenBufferHeight(256),
    oddFrame(false),
    numLights(0),
    numShapes(0),
    numShapeLights(0)
{
    memset(this->lightArray, 0, sizeof(this->lightArray));
    memset(this->shapeArray, 0, sizeof(this->shapeArray));
    memset(this->shapeLightArray, 0, sizeof(this->shapeLightArray));
    int i;
    for(i = 0; i < NUMVARIABLES; i++)
    {
        this->varHandle[i] = nVariable::INVALID_HANDLE;
    }
}

//------------------------------------------------------------------------------
/**
*/
nMRTSceneServer::~nMRTSceneServer()
{
    if (this->AreResourcesValid())
    {
        this->UnloadResources();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nMRTSceneServer::AreResourcesValid() const
{
    bool valid = true;
    int i;
    for (i = 0; i < NUMBUFFERS; i++)
    {
        valid &= this->refBuffer[i].isvalid();
    }
    for (i = 0; i < NUMSHADERS; i++)
    {
        valid &= this->refShader[i].isvalid();
    }
    for (i = 0; i < NUMVARIABLES; i++)
    {
        valid &= (this->varHandle[i] != nVariable::INVALID_HANDLE);
    }
    valid &= this->refMesh.isvalid();

    return valid;
}

//------------------------------------------------------------------------------
/**
*/
void
nMRTSceneServer::UnloadResources()
{
    int i;

    // release offscreen buffers
    for (i = 0; i < NUMBUFFERS; i++)
    {
        if (this->refBuffer[i].isvalid())
        {
            this->refBuffer[i]->Release();
            n_assert(!this->refBuffer[i].isvalid());
        }
    }

    // release offscreen shaders
    for (i = 0; i < NUMSHADERS; i++)
    {
        if (this->refShader[i].isvalid())
        {
            this->refShader[i]->Release();
            n_assert(!this->refShader[i].isvalid());
        }
    }

    // release screen rectangle mesh
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
        n_assert(!this->refMesh.isvalid());
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nMRTSceneServer::LoadResources()
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

/*
    int i;

    // initialize offscreen buffers
    static const char* bufferNames[] = 
    {
        "MRTEmissiveBuffer0",
        "MRTEmissiveBuffer1",
    };
    for (i = 0; i < NUMBUFFERS; i++)
    {
        // only the color buffer has a depth buffer attached
        bool hasColor = true;
        bool hasDepth = true;
        bool hasStencil = false;
        int width = this->offscreenBufferWidth;
        int height = this->offscreenBufferHeight;

        this->refBuffer[i] = gfxServer->NewRenderTarget(bufferNames[i],
                                                        width,
                                                        height,
                                                        nTexture2::A8R8G8B8,
                                                        hasColor, hasDepth, hasStencil);
        n_assert(this->refBuffer[i].isvalid());
    }

    // initialize shaders
    static const char* shaderNames[NUMSHADERS] = 
    {
        "shaders:mrtcompose.fx",
        "shaders:mrtconvolution.fx"
    };
    for (i = 0; i < NUMSHADERS; i++)
    {
        this->refShader[i] = gfxServer->NewShader(shaderNames[i]);
        this->refShader[i]->SetFilename(shaderNames[i]);
        bool success = this->refShader[i]->Load();
        n_assert(success);
    }

    // create a screen rectangle mesh
    this->refMesh = gfxServer->NewMesh("MRTMesh");
    this->refMesh->SetFilename("meshes:plane.n3d2");
    bool success = this->refMesh->Load();
    n_assert(success);

    // initialize variable handles
    static const char* varNames[NUMVARIABLES] =
    {
        "srcMap",
        "emissiveMap",
        "sample0Offset",
        "sample1Offset",
        "sample2Offset",
        "sample3Offset",
        "sample0Weight",
        "sample1Weight",
        "sample2Weight",
        "sample3Weight"
    };
    static nFourCC varFourCC[NUMVARIABLES] = 
    {
        FOURCC('srcm'),
        FOURCC('emim'),
        FOURCC('sm0o'),
        FOURCC('sm1o'),
        FOURCC('sm2o'),
        FOURCC('sm3o'),
        FOURCC('sm0w'),
        FOURCC('sm1w'),
        FOURCC('sm2w'),
        FOURCC('sm3w')
    };
    for (i = 0; i < NUMVARIABLES; i++)
    {
        this->varHandle[i] = this->refVariableServer->AddVariable(varNames[i], varFourCC[i]);
    }
*/
    return true;
}

//------------------------------------------------------------------------------
/**
    Split the collected scene nodes into light and shape nodes. Fills
    the lightArray[] and shapeArray[] members.
*/
void
nMRTSceneServer::SplitNodes()
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
nMRTSceneServer::CollectShapeLights(const Group& shapeGroup)
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
    Merge the offscreen buffers into the frame buffer. This simply renders
    a screen sized quad using the mrtcompose shader.
*/
void
nMRTSceneServer::MergeOffscreenBuffers(nTexture2* emissiveBuffer)
{
    n_assert(emissiveBuffer);

    nGfxServer2* gfxServer = this->refGfxServer.get();

    // set a quad spanning the entire screen as geometry
    gfxServer->SetMesh(0, this->refMesh.get());
    gfxServer->SetVertexRange(0, this->refMesh->GetNumVertices());
    gfxServer->SetIndexRange(0, this->refMesh->GetNumIndices());

    // initialize the compose shader
    nShader2* composeShader = this->refShader[COMPOSITESHADER].get();
    composeShader->SetTexture(this->varHandle[EMISSIVEMAP], emissiveBuffer);
    gfxServer->SetShader(composeShader);

    // and render!
    gfxServer->Draw();
}

//------------------------------------------------------------------------------
/**
    Render the shape objects in the scene, using the shader defined by
    the fourcc code. If no such shader exists, no rendering is done.
*/
void
nMRTSceneServer::RenderShapes(uint shaderFourCC)
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // for each shape object...
    int i;
    for (i = 0; i < this->numShapes; i++)
    {
        Group& group = this->groupArray[this->shapeArray[i]];
        if (group.sceneNode->HasShader(shaderFourCC))
        {
            gfxServer->SetTransform(nGfxServer2::MODELVIEW, group.modelView);
            group.sceneNode->RenderShader(shaderFourCC, this, group.renderContext);
            group.sceneNode->RenderGeometry(this, group.renderContext);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render all light/shape interactions matching a given fourcc shader code.

    FIXME: the intersection check happens once for the diffuse and
    specular pass, the information which lights intersect which
    shapes should be collected and stored once during SplitNodes()!
*/
void
nMRTSceneServer::RenderLightShapes(uint shaderFourCC)
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // for each shape object...
    int curShapeIndex;
    for (curShapeIndex = 0; curShapeIndex < this->numShapes; curShapeIndex++)
    {
        Group& shapeGroup = this->groupArray[this->shapeArray[curShapeIndex]];
        if (shapeGroup.sceneNode->HasShader(shaderFourCC))
        {
            // collect the lights influencing this shape
            this->CollectShapeLights(shapeGroup);
            if (this->numShapeLights > 0)
            {
                // set the modelview matrix for the shape
                gfxServer->SetTransform(nGfxServer2::MODELVIEW, shapeGroup.modelView);

                // prepare shader and geometry
                shapeGroup.sceneNode->RenderShader(shaderFourCC, this, shapeGroup.renderContext);
            
                // render geometry for each light intersecting this shape...
                int shapeLightIndex;
                for (shapeLightIndex = 0; shapeLightIndex < this->numShapeLights; shapeLightIndex++)
                {
                    Group& lightGroup = this->groupArray[this->shapeLightArray[shapeLightIndex]];

                    // update light parameters
                    lightGroup.sceneNode->RenderLightVolume(this, lightGroup.renderContext, lightGroup.modelView);

                    // render!
                    shapeGroup.sceneNode->RenderGeometry(this, shapeGroup.renderContext);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render the scene. Rendering happens in multiple passes:

    (1) clear the screen
    (2) render scene into the depth buffer only
    (3) render diffuse lighting 
    (4) render pixel colors and modulate into frame buffer
    (5) render specular light and add onto frame buffer
    (6) apply various post-special-effects
*/
void
nMRTSceneServer::RenderScene()
{
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // make sure all required resources are ok
    if (!this->AreResourcesValid())
    {
        bool success = this->LoadResources();
        n_assert(success);
    }

    // split shape and light nodes
    this->SplitNodes();

    // render the emissive pass
//    nTexture2* emissiveBuffer = this->RenderEmissivePass();

    // start rendering to the frame buffer
    gfxServer->BeginScene();
    gfxServer->Clear(nGfxServer2::ALL, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f, 0);

    // clear, render depth, diffuse, color and specular into the back buffer
    this->RenderShapes(FOURCC('dept'));
    this->RenderLightShapes(FOURCC('diff'));
    this->RenderShapes(FOURCC('colr'));
    this->RenderLightShapes(FOURCC('spec'));

    // merge the offscreen buffers into the frame buffer
//    this->MergeOffscreenBuffers(emissiveBuffer);

    // finish the frame
    gfxServer->DrawTextBuffer();
    gfxServer->EndScene();
    gfxServer->PresentScene();
}
