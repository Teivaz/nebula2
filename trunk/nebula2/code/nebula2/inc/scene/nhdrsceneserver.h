#ifndef N_HDRSCENESERVER_H
#define N_HDRSCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nHdrSceneServer
    @ingroup SceneServers

    @brief A scene server which implements an HDR lighting model.
    
     -# scene is rendered into a fullsize R16G16B16A16 offscreen buffer
     -# overbright areas from scene are rendered into offscreen "bloom" buffer.
     -# bloom buffer is blurred
     -# final frame is composed
    
    (C) 2004 RadonLabs GmbH
*/
#include "scene/nsceneserver.h"
#include "gfx2/nmesh2.h"

//------------------------------------------------------------------------------
class nHdrSceneServer : public nSceneServer
{
public:
    /// constructor
    nHdrSceneServer();
    /// destructor
    virtual ~nHdrSceneServer();
    /// begin the scene
    virtual bool BeginScene(const matrix44& viewer);
    /// render the scene
    virtual void RenderScene();
    /// present the scene
    virtual void PresentScene();
    /// set tint color
    void SetHueColor(const vector4& c);
    /// get tint color
    const vector4& GetHueColor() const;

private:
    /// initialize required resources
    bool LoadResources();
    /// unload resources
    void UnloadResources();
    /// check if resources are valid
    bool AreResourcesValid();
    /// render shape objects in scene
    void RenderShapes(uint shaderFourCC);
    /// render light/shape interaction for all lit shapes
    void RenderLightShapes(uint shaderFourCC);
    /// render the bloom effect
    void RenderPostEffect();
    /// compute a 2d gaussian distribution value
    float GaussianDistribution(float x, float y, float rho) const;
    /// get sample offsets for a horizontal or vertical blur
    void UpdateSampleOffsetsBloom(bool horizontal, int texSize, float deviation, float multiplier);
    /// get sample offsets for a 5x5 Gaussian blur
    void UpdateSampleOffsetsGaussBlur5x5(int texWidth, int texHeight, float multiplier);

    enum
    {
        ExtractShader = 0,
        FilterShader = 1,
        ComposeShader = 2,
        NumShaders = 3,

        NumSamples = 15,
    };
         
    nRef<nTexture2> hdrSceneBuffer;     // fullsize offscreen buffer
    nRef<nTexture2> hdrPostBuffer0;     // ping-buffer for bloom effect
    nRef<nTexture2> hdrPostBuffer1;     // pong-buffer for bloom effect
    nRef<nMesh2> quadMesh;              // mesh describing a full screen quad
    nRef<nShader2> shaders[3];
    vector4 hueColor;
    vector4 sampleOffsets[NumSamples];
    vector4 sampleWeights[NumSamples];
    int postBufferWidth;
    int postBufferHeight;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nHdrSceneServer::SetHueColor(const vector4& c)
{
    this->hueColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nHdrSceneServer::GetHueColor() const
{
    return this->hueColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nHdrSceneServer::GaussianDistribution(float x, float y, float rho) const
{
    float g = 1.0f / sqrtf(2.0f * N_PI * rho * rho);
    g *= expf(-(x * x + y * y) / (2 * rho * rho));
    return g;
}

//------------------------------------------------------------------------------
#endif    


