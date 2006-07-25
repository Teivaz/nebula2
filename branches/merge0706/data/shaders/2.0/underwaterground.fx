//------------------------------------------------------------------------------
//  underwaterground.fx
//
//  Special layered shader.
//
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#define VS_SETSCREENPOS(X) vsOut.screenPos = vsComputeScreenCoord(X);
#define DECLARE_SCREENPOS(X) float4 screenPos : X;
#define PS_ARG_SCREENPOS
#define PS_SCREENPOS psComputeScreenCoord(psIn.screenPos)


shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float Time;

float3 ModelLightPos;
shared float3   LightPos;
shared int      LightType;
shared float    LightRange;
shared float4   LightDiffuse;
shared float4   LightSpecular;
shared float4   LightAmbient;
shared float4   ShadowIndex;

float4 Color0;
float4 MatDiffuse;
float4 MatEmissive = float4(0.0f, 0.0f, 0.0f, 0.0f);
float  MatEmissiveIntensity = 0.0f;
float4 MatSpecular;
float4 MatAmbient;
float  MatSpecularPower;

bool  AlphaBlendEnable = true;
float Intensity0;
int CullMode = 2;

float4 TexGenS;         // caustics scale
float4 Velocity;        // caustics speed

texture DiffMap0;
texture DiffMap1;
texture DiffMap2;
texture DiffMap3;
texture NoiseMap0;
texture NoiseMap1;
texture NoiseMap2;
shared texture AmbientMap1;

typedef half4 color4;


struct vsInputStaticDepth
{
    float4 position : POSITION;
};

struct vsOutputStaticDepth
{
    float4 position : POSITION;
    float  depth : TEXCOORD0;
};


struct vsInputStatic2Color
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 binormal : BINORMAL;
};

struct vsOutputStatic2Color
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float2 uv1           : TEXCOORD1;
    float2 uv2           : TEXCOORD2;
    float3 lightVec      : TEXCOORD3;
    float3 modelLightVec : TEXCOORD4;
    float3 halfVec       : TEXCOORD5;
    float3 eyePos        : TEXCOORD6;    
    DECLARE_SCREENPOS(TEXCOORD7)
};


//------------------------------------------------------------------------------
/**
    sampler definitions
*/
sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler Diff1Sampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler Diff2Sampler = sampler_state
{
    Texture = <DiffMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler Diff3Sampler = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};


sampler Layer0Sampler = sampler_state
{
    Texture = <NoiseMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler Layer1Sampler = sampler_state
{
    Texture = <NoiseMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler Layer2Sampler = sampler_state
{
    Texture = <NoiseMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};


sampler ShadowSampler = sampler_state
{
    Texture = <AmbientMap1>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

// detail texture scale for the layered shader, zoom by 40 as default
static const float4x4 DetailTexture = {40.0f,  0.0f,  0.0f,  0.0f,
                                        0.0f, 40.0f,  0.0f,  0.0f, 
                                        0.0f,  0.0f, 40.0f,  0.0f,
                                        0.0f,  0.0f,  0.0f, 40.0f };                                


color4 EncodeHDR(in color4 rgba)
{
    return rgba * color4(0.3334, 0.3334, 0.3334, 1.0);
}


//------------------------------------------------------------------------------
/**
    Vertex shader part to compute texture coordinate shadow texture lookups.
*/
float4 vsComputeScreenCoord(float4 pos)
{
    return pos;
}

//------------------------------------------------------------------------------
/**
    Pixel shader part to compute texture coordinate shadow texture lookups.
*/
float2 psComputeScreenCoord(float4 pos)
{
    float2 screenCoord = pos.xy / pos.ww;
    return float2(screenCoord * float2(0.5, -0.5)) + float2(0.5f, 0.5f);
}


//------------------------------------------------------------------------------
/**
    Vertex shader part for per-pixel-lighting. Computes the light
    and half vector in tangent space which are then passed to the
    interpolators.
*/
void vsLight(in float3 position, 
             in float3 normal, 
             in float3 tangent,
             in float3 binormal,
             in float3 modelEyePos,
             in float3 modelLightPos,
             out float3 tangentLightVec,
             out float3 modelLightVec,
             out float3 halfVec,
             out float3 tangentEyePos)
{
    float3 eVec = normalize(modelEyePos - position);
    if (LightType == 0)
    {
        // point light
        modelLightVec = modelLightPos - position;
    }
    else
    {
        // directional light
        modelLightVec = modelLightPos;
    }
    float3 hVec = normalize(normalize(modelLightVec) + eVec);
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);
    tangentLightVec = mul(tangentMatrix, modelLightVec);
    halfVec = mul(tangentMatrix, hVec);
    tangentEyePos = normalize(mul(tangentMatrix, eVec));
}

//------------------------------------------------------------------------------
/**
    Compute per-pixel lighting.
    
    NOTE: lightVec.w contains the distance to the light source
*/
color4 psLight(in color4 mapColor, in float3 tangentSurfaceNormal, in float3 lightVec, in float3 modelLightVec, in float3 halfVec, in half shadowValue)
{
    color4 emm  = MatEmissive * MatEmissiveIntensity;    
    color4 emColor   = mapColor * emm * tangentSurfaceNormal.z;
    color4 ambColor  = mapColor * LightAmbient * tangentSurfaceNormal.z;
    color4 color = color4(ambColor.rgb + emColor.rgb, mapColor.a * MatDiffuse.a);
    
    if (shadowValue > 0.1f)
    {
        // light intensities
        half diffIntensity = saturate(dot(tangentSurfaceNormal, normalize(lightVec)));
        half specIntensity = pow(saturate(dot(tangentSurfaceNormal, normalize(halfVec))), MatSpecularPower);

        // attenuation
        half att;
        if (LightType == 0)
        {
            // point light source
            att = shadowValue * (1.0f - saturate(length(modelLightVec) / LightRange));
        }
        else
        {
            att = shadowValue;
        }

        color4 diff = LightDiffuse * MatDiffuse;
        color4 spec = LightSpecular * MatSpecular;
        color4 diffColor = mapColor * diff;
        color4 specColor = specIntensity * spec;
        
        color.rgb += att * diffIntensity * (diffColor.rgb + specColor.rgb);
    }
    return color;
}

//------------------------------------------------------------------------------
/**
    Compute shadow value. Returns a shadow intensity between 1 (not in shadow)
    and 0 (fully in shadow)
*/
half psShadow(in float2 screenCoord)
{
    // determine if pixel is in shadow
    color4 shadow = tex2D(ShadowSampler, screenCoord);
    half shadowValue = 1.0 - saturate(length(ShadowIndex * shadow));
    return shadowValue;    
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate depth values for static geometry.
*/
vsOutputStaticDepth vsStaticDepth(const vsInputStaticDepth vsIn)
{
    vsOutputStaticDepth vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.depth = vsOut.position.z;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
*/
color4 psStaticDepth(const vsOutputStaticDepth psIn) : COLOR
{
    return float4(psIn.depth, 0.0f, 0.0f, 1.0f);
}


/**
    Vertex shader for 4-layered shader.
*/
vsOutputStatic2Color vsUnderwaterGroundColor(const vsInputStatic2Color vsIn)
{
    vsOutputStatic2Color vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0      = mul(vsIn.uv0, DetailTexture);
    vsOut.uv1      = vsIn.uv0;

    const float periode = 5.0;
    float modTime = fmod(Time, 100.0f);    
    float scale = TexGenS.xy + 0.05f * (sin( fmod(Time, periode) * 6.28318531 / periode ) + 0.5);
    
    vsOut.uv2.xy = vsIn.uv0 * scale + modTime * Velocity.xy;

    vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for 4-layered shader.
*/
color4 psUnderwaterGroundColor(const vsOutputStatic2Color psIn, uniform bool hdr, uniform bool shadow PS_ARG_SCREENPOS) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05f, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    
    // read tiled material colors
    color4 material0 = tex2D(DiffSampler,  psIn.uv0);
    color4 material1 = tex2D(Diff1Sampler, psIn.uv0);
    color4 material2 = tex2D(Diff2Sampler, psIn.uv0);
    color4 material3 = tex2D(Diff3Sampler, psIn.uv2);

    // read weight colors
    half weight0 = tex2D(Layer0Sampler, psIn.uv1).r;
    half weight1 = tex2D(Layer1Sampler, psIn.uv1).r;

    color4 color;
    color = lerp(material0, material1, weight0);
    color = lerp(color, material2, weight1);
    color = color + material3;   
    color = psLight(color, float3(0.0f, 0.0f, 1.0f), psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    
    if (hdr)
    {
        return EncodeHDR(color);
    }
    else
    {
        return color;
    }
#endif    
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "layered"
*/
technique tUnderwaterGroundDepth
{
    pass p0
    {
        CullMode     = <CullMode>;
        VertexShader = compile VS_PROFILE vsStaticDepth();
        PixelShader  = compile PS_PROFILE psStaticDepth();
    }
}

technique tUnderwaterGroundColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsUnderwaterGroundColor();
        PixelShader  = compile PS_PROFILE psUnderwaterGroundColor(false, false);
    }
}

technique tUnderwaterGroundColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsUnderwaterGroundColor();
        PixelShader  = compile PS_PROFILE psUnderwaterGroundColor(true, false);
    }
}

technique tUnderwaterGroundColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsUnderwaterGroundColor();
        PixelShader  = compile PS_PROFILE psUnderwaterGroundColor(false, true);
    }
}

technique tUnderwaterGroundColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsUnderwaterGroundColor();
        PixelShader  = compile PS_PROFILE psUnderwaterGroundColor(true, true);
    }
}