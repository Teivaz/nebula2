//------------------------------------------------------------------------------
//  splatterrain.fx
//
//  The terrain shader for splatted chunked LOD, using morphing on LOD chunks
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };
float4x4 TextureTransform1 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };
float Scale;
float4 MatDiffuse;
bool MatTranslucency = false;

int AlphaRef = 100;

texture DiffMap0;
texture DiffMap1;

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler WeightSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler DetailSampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
};


float4
transformStatic(const float3 pos, const float4x4 mvp)
{
    return mul(float4(pos, 1.0), mvp);
}

struct vsInput {
    float4 pos : POSITION;
    float2 uv0 : TEXCOORD0;  // actually u is the morph value, v is discarded (aagh!)
};

struct vsOutput {
    float4 pos : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
//    float4 ambient : COLOR0;
    float4 diffuse: COLOR0;
    float4 specular: COLOR1;
};

vsOutput vsMain(const vsInput vsIn)
{
    vsOutput vsOut;

    float4 mpos = vsIn.pos;// + float4(0,0,Scale * vsIn.uv0.x,0);

    vsOut.pos = transformStatic(mpos, ModelViewProjection);
//    vsOut.ambient = float4(1.0f,1.0f,1.0f,1.0f);
    vsOut.diffuse = float4(0.0f,0.0f,0.0f,1.0f);
    vsOut.specular = float4(0.0f,0.0f,0.0f,1.0f);

    float4 texcoord0 = mul(vsIn.pos, TextureTransform0);
    vsOut.uv0 = float2(texcoord0.x, 1.0-texcoord0.y);
    float4 texcoord1 = mul(vsIn.pos, TextureTransform1);
    vsOut.uv1 = float2(texcoord1.x, 1.0-texcoord1.y);

    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
//        WorldTransform[0]   = <Model>;
//        ViewTransform       = <View>;
//        ProjectionTransform = <Projection>;
//        TextureTransform[0] = <TextureTransform0>;

        ZWriteEnable     = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZEnable          = True;
        ZFunc            = LessEqual;

        AlphaBlendEnable = <MatTranslucency>;
        SrcBlend         = One;
        DestBlend        = One;
        NormalizeNormals = True;

        AlphaTestEnable  = False;
        AlphaFunc        = Greaterequal;
        AlphaRef         = <AlphaRef>;

        CullMode = <CullMode>;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = 0;

        SpecularEnable = false;

        Sampler[0] = <WeightSampler>;
        Sampler[1] = <DetailSampler>;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;

        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;

        ColorOp[2]   = Disable;
    }
}

