#ifndef N_MRTSCENESERVER_H
#define N_MRTSCENESERVER_H
//------------------------------------------------------------------------------
/**
    "Multiple Render Target" scene server, spreads rendering across various
    specialized offscreen buffers, which are combined to the final result
    by specific "compositing" pixel shaders.

    *** EXPERIMENTAL ***
    
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
#define N_DEFINES nSceneServer
#include "kernel/ndefdllclass.h"

class nVariableServer;
class nTexture2;
class nShader2;
class nMesh2;

//------------------------------------------------------------------------------
class nMRTSceneServer : public nSceneServer
{
public:
    /// constructor
    nMRTSceneServer();
    /// destructor
    virtual ~nMRTSceneServer();
    /// render the scene
    virtual void RenderScene();

    static nKernelServer* kernelServer;

private:
    /// initialize required resources
    bool LoadResources();
    /// unload resources
    void UnloadResources();
    /// check if resources are valid
    bool AreResourcesValid() const;
    /// render shape objects in scene
    void RenderShapes(uint shaderFourCC);
    /// render light/shape interaction for all lit shapes
    void RenderLightShapes(uint shaderFourCC);
    /// render a texture using a blur filter
    void ApplyBlurFilter(nGfxServer2* gfxServer, nTexture2* srcBuffer);
    /// render the emissive pass
    nTexture2* RenderEmissivePass();
    /// merge offscreen buffers into the frame buffer
    void MergeOffscreenBuffers(nTexture2* emissiveBuffer);
    /// split scene nodes into light and shape nodes
    void SplitNodes();
    /// collect all lights influencing a given shape
    void CollectShapeLights(const Group& shapeGroup);

    nAutoRef<nVariableServer> refVariableServer;

    enum
    {
        MAXLIGHTSPERSHAPE = 16,
    };

    enum BufferTypes
    {
        EMISSIVEBUFFER0 = 0,    // even emissive buffer
        EMISSIVEBUFFER1,        // odd emissive buffer

        NUMBUFFERS,
    };

    enum ShaderTypes
    {
        COMPOSITESHADER = 0,    // composes offscreen buffers into back buffer
        CONVOLUTIONSHADER,      // convolution shader for the glow effect
        
        NUMSHADERS,
    };

    enum VariableTypes
    {
        SRCMAP = 0,
        EMISSIVEMAP,

        SAMPLE0OFFSET,
        SAMPLE1OFFSET,
        SAMPLE2OFFSET,
        SAMPLE3OFFSET,

        SAMPLE0WEIGHT,
        SAMPLE1WEIGHT,
        SAMPLE2WEIGHT,
        SAMPLE3WEIGHT,

        NUMVARIABLES,
    };

    nRef<nTexture2> refBuffer[NUMBUFFERS];          // the offscreen buffers
    nRef<nShader2>  refShader[NUMSHADERS];          // the frame-effect pixel shaders
    nRef<nMesh2>    refMesh;                        // a screen rect mesh
    nVariable::Handle varHandle[NUMVARIABLES];      // variable handles used by the scene server

    int offscreenBufferWidth;
    int offscreenBufferHeight;

    bool oddFrame;

    int numLights;
    ushort lightArray[nSceneServer::MAX_GROUPS];    // indices of light nodes in scene

    int numShapes;
    ushort shapeArray[nSceneServer::MAX_GROUPS];    // indices of shape nodes in scene

    int numShapeLights;
    ushort shapeLightArray[MAXLIGHTSPERSHAPE];       // indices of light nodes intersecting a shape
};
//------------------------------------------------------------------------------
#endif


