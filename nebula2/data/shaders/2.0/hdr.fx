//------------------------------------------------------------------------------
//  hdr.fx
//
//  Contains various techniques for HDR rendering effects.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

static const int   MaxSamples = 16;

texture DiffMap0;
texture DiffMap1;
texture DiffMap2;
float3 SampleOffsetsWeights[MaxSamples];
shared float4 DisplayResolution;
float Intensity0;
float Intensity1;
float Intensity2;
float4 MatDiffuse;
float4 MatAmbient;

//------------------------------------------------------------------------------
//  Declarations.
//------------------------------------------------------------------------------
struct vsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct vsOutput
{
    float4 position  : POSITION;
    float2 uv0 : TEXCOORD0;
};

sampler SourceSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

sampler BloomSampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

sampler Lum1x1Sampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

sampler BloomSampler2 = sampler_state
{
    Texture = <DiffMap2>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

//------------------------------------------------------------------------------
//  Helper Functions.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
*/    
float
GaussianDistribution(in const float x, in const float y, in const float rho)
{
    const float pi = 3.1415927f;
    float g = 1.0f / sqrt(2.0f * pi * rho * rho);
    g *= exp(-(x * x + y * y) / (2 * rho * rho));
    return g;
}

//------------------------------------------------------------------------------
/**
    UpdateSamplesDownscale4x4
    
    Create filter kernel for a 4x4 downscale operation.
    This is normally executed pre-shader.    
*/
void
UpdateSamplesDownscale4x4(in int texWidth, in int texHeight, out float3 sampleOffsetsWeights[MaxSamples])
{
    float tu = 1.0f / texWidth;
    float tv = 1.0f / texHeight;
    
    int index = 0;
    int y;
    for (y = 0; y < 4; y++)
    {
        int x;
        for (x = 0; x < 4; x++)
        {
            sampleOffsetsWeights[index++] = float3((x - 1.5f) * tu, (y - 1.5f) * tv, 0.0f);
        }
    }
}

//------------------------------------------------------------------------------
/**
    UpdateSamplesLuminance
    
    Create a filter kernel for a 3x3 luminance filter.
    This is normally executed pre-shader.
*/
void
UpdateSamplesLuminance(in int texWidth, in int texHeight, out float3 sampleOffsetsWeights[MaxSamples])
{
    float tu = 1.0f / (3.0f * texWidth);
    float tv = 1.0f / (3.0f * texHeight);
    
    int index = 0;
    int x;
    for (x = -1; x <= 1; x++)
    {
        int y;
        for (y = -1; y <= 1; y++)
        {
            sampleOffsetsWeights[index++] = float3(x * tu, y * tv, 0.0f);
        }
    }
    for (index; index < MaxSamples; index++)
    {
        sampleOffsetsWeights[index] = float3(0.0f, 0.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------
/**
    UpdateSamplesGaussBlur5x5
    
    Update the sample offsets and weights for a horizontal or vertical blur.
    This is normally executed in the pre-shader.
*/
void
UpdateSamplesGaussBlur5x5(in int texWidth, in int texHeight, float multiplier, out float3 sampleOffsetsWeights[MaxSamples])
{
    float tu = 1.0f / (float) texWidth;
    float tv = 1.0f / (float) texHeight;
    
    float totalWeight = 0.0f;
    int index = 0;
    int x;
    for (x = -2; x <= 2; x++)
    {
        int y;
        for (y = -2; y <= 2; y++)
        {
            // exclude pixels with block distance > 2, to reduce 5x5 filter
            // to 13 sample points instead of 25 (we only do 16 lookups)
            if ((abs(x) + abs(y)) <= 2)
            {
                // get unscaled Gaussian intensity
                sampleOffsetsWeights[index].xy = float2(x * tu, y * tv);
                float curWeight = GaussianDistribution(x, y, 1.0f);
                sampleOffsetsWeights[index].z = curWeight;
                totalWeight += curWeight;

                index++;
            }
        }
    }
    
    // normalize weights
    int i;
    float invTotalWeightMultiplier = (1.0f / totalWeight) * multiplier;
    for (i = 0; i < index; i++)
    {
        sampleOffsetsWeights[i].z *= invTotalWeightMultiplier;
    }             

    // make sure the extra samples dont influence the result
    for (; index < MaxSamples; index++)
    {
        sampleOffsetsWeights[index] = float3(0.0f, 0.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------
/**
    UpdateSamplesBloom
    
    Get sample offsets and weights for a horizontal or vertical bloom filter.
    This is normally executed in the pre-shader.
*/
void
UpdateSamplesBloom(in bool horizontal, in int texSize, in float deviation, in float multiplier, out float3 sampleOffsetsWeights[MaxSamples])
{
    float tu = 1.0f / (float) texSize;
    
    // fill center texel
    float weight = multiplier * GaussianDistribution(0.0f, 0.0f, deviation);
    sampleOffsetsWeights[0]  = float3(0.0f, 0.0f, weight);
    sampleOffsetsWeights[15] = float3(0.0f, 0.0f, 0.0f);

    // fill first half
    int i;
    for (i = 1; i < 8; i++)
    {
        if (horizontal)
        {
            sampleOffsetsWeights[i].xy = float2(i * tu, 0.0f);
        }
        else
        {
            sampleOffsetsWeights[i].xy = float2(0.0f, i * tu);
        }
        weight = multiplier * GaussianDistribution((float)i, 0, deviation);
        sampleOffsetsWeights[i].z = weight;
    }

    // mirror second half
    for (i = 8; i < 15; i++)
    {
        sampleOffsetsWeights[i] = sampleOffsetsWeights[i - 7] * float3(-1.0f, -1.0f, 1.0f);
    }
}

//------------------------------------------------------------------------------
//  Vertex shader for rendering a fullscreen quad with correction
//  for D3D's texel center.
//------------------------------------------------------------------------------
vsOutput vsQuad(const vsInput vsIn)
{
    vsOutput vsOut;
    vsOut.position = vsIn.position;
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
//  A 4x4 filtered downscale pixel shader.
//------------------------------------------------------------------------------
float4 psDownscale4x4(const vsOutput psIn) : COLOR
{
    UpdateSamplesDownscale4x4(DisplayResolution.x, DisplayResolution.y, SampleOffsetsWeights);
    float4 sample = 0.0f;
    int i;
    for (i = 0; i < 16; i++)
    {
        sample += tex2D(SourceSampler, psIn.uv0 + SampleOffsetsWeights[i].xy);
    }
    return sample / 16;
}

//------------------------------------------------------------------------------
//  Sample luminance of source image returning log() of the averages.
//------------------------------------------------------------------------------
float4 psSampleLumInitial(const vsOutput psIn) : COLOR
{
    UpdateSamplesLuminance(DisplayResolution.x, DisplayResolution.y, SampleOffsetsWeights);
    
    const float3 lumVector = float3(0.2125f, 0.7154f, 0.0721f);
    float logLumSum = 0.0f;
    int i;
    for (i = 0; i < 9; i++)
    {
        float3 sample = tex2D(SourceSampler, psIn.uv0 + SampleOffsetsWeights[i].xy);
        logLumSum += log(dot(sample, lumVector) + 0.0001f);
    }
    logLumSum /= 9;
    return float4(logLumSum, logLumSum, logLumSum, 1.0f);
}

//------------------------------------------------------------------------------
//  Iterative step for luminance measurement.
//------------------------------------------------------------------------------
float4 psSampleLumIterative(const vsOutput psIn) : COLOR
{
    UpdateSamplesDownscale4x4(DisplayResolution.x, DisplayResolution.y, SampleOffsetsWeights);

    float sample = 0.0f;
    int i;
    for (i = 0; i < 16; i++)
    {
        sample += tex2D(SourceSampler, psIn.uv0 + SampleOffsetsWeights[i].xy);
    }
    sample /= 16;
    return float4(sample, sample, sample, 1.0f);
}

//------------------------------------------------------------------------------
//  Final step for luminance measurement.
//------------------------------------------------------------------------------
float4 psSampleLumFinal(const vsOutput psIn) : COLOR
{
    float sample = 0.0f;
    int i;
    for (i = 0; i < 16; i++)
    {
        sample += tex2D(SourceSampler, psIn.uv0 + SampleOffsetsWeights[i].xy);
    }
    sample = exp(sample / 16);
    return float4(sample, sample, sample, 1.0f);    
}

//------------------------------------------------------------------------------
//  Implements a bright pass filter. Uses the measured 1x1 luminance texture
//  as indication what areas of the picture are classified as "bright".
//------------------------------------------------------------------------------
float4 psBrightPassFilter(const vsOutput psIn) : COLOR
{
    float4 sample = tex2D(SourceSampler, psIn.uv0);
    float brightPassThreshold = Intensity1;
    float brightPassOffset    = Intensity2;
    
    // subtract out dark pixels
    sample.rgb -= brightPassThreshold;
    
    // clamp to 0
    sample.rgb = max(sample.rgb, 0.0f);
    
    // map value to 0..1
    sample.rgb /= brightPassOffset + sample.rgb;

    return sample;
}

//------------------------------------------------------------------------------
//  Implements a 5x5 Gaussian blur filter.
//------------------------------------------------------------------------------
float4 psGaussBlur5x5(const vsOutput psIn) : COLOR
{
    // preshader
    UpdateSamplesGaussBlur5x5(DisplayResolution.x, DisplayResolution.y, 1.0f, SampleOffsetsWeights);
    
    // shader
    float4 sample = 0.0f;
    int i;
    for (i = 0; i < 12; i++)
    {
        sample += SampleOffsetsWeights[i].z * tex2D(SourceSampler, psIn.uv0 + SampleOffsetsWeights[i].xy);
    }
    return sample;
}

//------------------------------------------------------------------------------
//  Brightpass pixel shader.
//------------------------------------------------------------------------------
float4 psBrightPass(const vsOutput psIn) : COLOR
{
/*
    UpdateSamplesGaussBlur5x5(DisplayResolution.x, DisplayResolution.y, 1.0f, SampleOffsetsWeights);
    int i;
    float4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
    for (i = 0; i < MaxSamples; i++)
    {
        color += SampleOffsetsWeights[i].z * tex2D(SourceSampler, psIn.uv0 + SampleOffsetsWeights[i].xy);
    }
    return color;
*/
    return 0.25 * tex2D(SourceSampler, psIn.uv0);
}

//------------------------------------------------------------------------------
//  Pixel shader which performs a horizontal bloom effect.
//------------------------------------------------------------------------------
float4 psBloomHori(const vsOutput psIn) : COLOR
{
    // preshader...
    UpdateSamplesBloom(true, DisplayResolution.x, 3.0f, 2.0f, SampleOffsetsWeights);

    // shader...
    int i;
    float4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
    for (i = 0; i < MaxSamples; i++)
    {
        color += SampleOffsetsWeights[i].z * tex2D(SourceSampler, psIn.uv0 + SampleOffsetsWeights[i].xy);
    }
    return color;
}

//------------------------------------------------------------------------------
//  Pixel shader which performs a vertical bloom effect.
//------------------------------------------------------------------------------
float4 psBloomVert(const vsOutput psIn) : COLOR
{
    // preshader...
    UpdateSamplesBloom(false, DisplayResolution.y, 3.0f, 2.0f, SampleOffsetsWeights);

    // shader...
    int i;
    float4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
    for (i = 0; i < MaxSamples; i++)
    {
        color += SampleOffsetsWeights[i].z * tex2D(SourceSampler, psIn.uv0 + SampleOffsetsWeights[i].xy);
    }
    return color;
}

//------------------------------------------------------------------------------
//  Pixel shader which combines the source buffers into the final
//  result for the HDR adaptive eye model render path.
//------------------------------------------------------------------------------
float4 psFinalScene(const vsOutput psIn) : COLOR
{
    float4 sample = tex2D(SourceSampler, psIn.uv0);
    float4 bloom  = tex2D(BloomSampler2, psIn.uv0);
    //float adaptedLum = tex2D(Lum1x1Sampler, float2(0.5f, 0.5f));
    // float middleGray = Intensity0;
    float bloomScale = Intensity1;
    
    // FIXME: handle blue shift
    
    // add bloom effect
    sample += bloomScale * bloom;
    
    return sample;
}

//------------------------------------------------------------------------------
//  Pixel shader for final scene composition.
//------------------------------------------------------------------------------
float4 psHdrCompose(const vsOutput psIn) : COLOR
{
    float4 sample = tex2D(SourceSampler, psIn.uv0);
    float4 bloom  = tex2D(BloomSampler2, psIn.uv0);
    float saturation = Intensity0;
    float bloomScale = Intensity1;
    float4 lumiVec = MatAmbient;
    float4 balance = MatDiffuse;
    sample += bloomScale * bloom;
    float luminance = dot(sample.xyz, lumiVec.xyz);
    float4 color = balance * lerp(float4(luminance, luminance, luminance, luminance), sample, saturation);
    return color;
}

//------------------------------------------------------------------------------
//  A simple copy pixel shader
//------------------------------------------------------------------------------
float4 psCopy(const vsOutput psIn) : COLOR
{
    return tex2D(SourceSampler, psIn.uv0);
}

//------------------------------------------------------------------------------
//  Pixel shader which combines a source texture with a bloom texture.
//------------------------------------------------------------------------------
float4 psCombine(const vsOutput psIn) : COLOR
{
    return tex2D(SourceSampler, psIn.uv0) + Intensity0 * tex2D(BloomSampler, psIn.uv0);
}

//------------------------------------------------------------------------------
//  Technique to perform a 4x4 filtered downscale
//------------------------------------------------------------------------------
technique tDownscale4x4
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psDownscale4x4();
    }
}

//------------------------------------------------------------------------------
//  Technique for initial luminance measurement pass.
//------------------------------------------------------------------------------
technique tSampleLumInitial
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psSampleLumInitial();
    }
}

//------------------------------------------------------------------------------
//  Technique for initial luminance measurement pass.
//------------------------------------------------------------------------------
technique tSampleLumIterative
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psSampleLumIterative();
    }
}

//------------------------------------------------------------------------------
//  Technique for final luminance measurement pass.
//------------------------------------------------------------------------------
technique tSampleLumFinale
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psSampleLumFinal();
    }
}

//------------------------------------------------------------------------------
//  Implement the adaptive eye model bright pass filter.
//------------------------------------------------------------------------------
technique tBrightPassFilter
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psBrightPassFilter();
    }
}

//------------------------------------------------------------------------------
//  Implements a Gaussian blur filter.
//------------------------------------------------------------------------------
technique tGaussBlur5x5
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psGaussBlur5x5();
    }
}

//------------------------------------------------------------------------------
//  Technique to perform the brightpass.
//------------------------------------------------------------------------------
technique tBrightPass
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psBrightPass();
    }
}        

//------------------------------------------------------------------------------
//  Technique to perform the horizontal bloom effect.
//------------------------------------------------------------------------------
technique tBloomHori
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psBloomHori();
    }
}        

//------------------------------------------------------------------------------
//  Technique to perform the horizontal bloom effect.
//------------------------------------------------------------------------------
technique tBloomVert
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psBloomVert();
    }
}        

//------------------------------------------------------------------------------
//  Final scene composition (pre-alpha) for adaptive eye model 
//  renderer.
//------------------------------------------------------------------------------
technique tFinalScene
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psFinalScene();
    }
}

//------------------------------------------------------------------------------
//  Technique to perform a horizontal blur
//------------------------------------------------------------------------------
technique tHoriBlur
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psBloomHori();
    }
}

//------------------------------------------------------------------------------
//  Technique to perform a vertical blur
//------------------------------------------------------------------------------
technique tVertBlur
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile vs_2_0 vsQuad();
        PixelShader      = compile ps_2_0 psBloomVert();
    }
}

//------------------------------------------------------------------------------
//  Technique to combine source texture with bloom texture.
//------------------------------------------------------------------------------
technique tCombine
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;

        VertexShader = compile vs_2_0 vsQuad();
        PixelShader = compile ps_2_0 psCombine();
    }
}

//------------------------------------------------------------------------------
//  Technique to copy a source texture to the render target.
//------------------------------------------------------------------------------
technique tCopy
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;

        VertexShader = compile vs_2_0 vsQuad();
        PixelShader = compile ps_2_0 psCopy();
    }
}

//------------------------------------------------------------------------------
//  Technique to for final scene composition.
//------------------------------------------------------------------------------
technique tCompose
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;

        VertexShader = compile vs_2_0 vsQuad();
        PixelShader = compile ps_2_0 psHdrCompose();
    }
}


