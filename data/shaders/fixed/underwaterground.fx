//------------------------------------------------------------------------------
//  underwaterground.fx
//
//  Special layered shader.
//
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float Time;

float4 Color0;
float4 MatDiffuse;
float4 MatEmissive = float4(0.0f, 0.0f, 0.0f, 0.0f);
float  MatEmissiveIntensity = 0.0f;

bool  AlphaBlendEnable = true;
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

typedef half4 color4;


static const float4x4 Ident = { 1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };

// detail texture scale for the layered shader, zoom by 40 as default
static const float4x4 DetailTexture = {40.0f,  0.0f,  0.0f,  0.0f,
                                        0.0f, 40.0f,  0.0f,  0.0f,
                                        0.0f,  0.0f, 40.0f,  0.0f,
                                        0.0f,  0.0f,  0.0f, 40.0f };


struct vsInputStatic2Color
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
};

struct vsOutputStatic2Color
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float2 uv1           : TEXCOORD1;
    float2 uv2           : TEXCOORD2;
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


/**
    Vertex shader for 4-layered shader.
*/
vsOutputStatic2Color vsUnderwaterGroundColor(const vsInputStatic2Color vsIn)
{
    vsOutputStatic2Color vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0      = mul(vsIn.uv0, DetailTexture);
    vsOut.uv1      = vsIn.uv0;

    const float periode = 5.0;
    float modTime = fmod(Time, 100.0f);
    float scale = TexGenS.xy + 0.05f * (sin( fmod(Time, periode) * 6.28318531 / periode ) + 0.5);

    vsOut.uv2.xy = vsIn.uv0 * scale + modTime * Velocity.xy;

    return vsOut;
}



technique tUnderwaterGround
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsUnderwaterGroundColor();

        CullMode          = <CullMode>;
        AlphaTestEnable   = False;
        MaterialDiffuse   = <MatDiffuse>;
        MaterialEmissive  = <MatEmissive>;
        MaterialAmbient   = {1.0, 1.0, 1.0, 1.0};

        TextureTransform[0] = <Ident>;
        TexCoordIndex[0]    = 0;
        TextureTransform[1] = <Ident>;
        TexCoordIndex[1]    = 1;
        TextureTransform[2] = <Ident>;
        TexCoordIndex[2]    = 2;

        Sampler[0]        = <DiffSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;

        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable;
    }
    pass p1
    {
        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = True;
        AlphaRef         = 1;

        Sampler[0]   = <Diff1Sampler>;
        Sampler[1]   = <Layer0Sampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg2;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;

        ColorOp[1]   = SelectArg1;
        ColorArg1[1] = Current;
        ColorArg2[1] = Current;
        AlphaOp[1]   = Modulate;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;

        ColorOp[2]   = Disable;
        AlphaOp[2]   = Disable;
    }
    pass p2
    {
        Sampler[0]   = <Diff2Sampler>;
        Sampler[1]   = <Layer1Sampler>;
        Sampler[2]   = <Diff3Sampler>;

        ColorOp[2]   = Add;
        ColorArg1[2] = Current;
        ColorArg2[2] = Texture;

        AlphaOp[2]   = Disable;
    }
}