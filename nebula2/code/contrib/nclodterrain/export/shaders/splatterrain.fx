//------------------------------------------------------------------------------
//  fixed/morphterrain.fx
//
//  The terrain shader for splatted chunked LOD, using morphing on LOD chunks
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
float4
transformStatic(const float3 pos, const float4x4 mvp)
{
	return mul(float4(pos, 1.0), mvp);
}


float4x4 ModelViewProjection;
float Height;
float4x4 TextureTransform0;
float4x4 TextureTransform1;
float4 MatDiffuse;
bool MatTranslucency = false;

int AlphaRef = 100;

texture DiffMap0;
texture DiffMap1;

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

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

struct vsInput {
    float4 pos : POSITION;
    float2 uv0 : TEXCOORD0;  // actually u is the morph value, v is discarded (aagh!)
};

struct vsOutput {
    float4 pos : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
//    float4 ambient : COLOR0;
    float4 diffuse: COLOR1;
};

vsOutput vsMain(const vsInput vsIn)
{
    vsOutput vsOut;

    float4 mpos = vsIn.pos + float4(0,0,Height * vsIn.uv0.x,0);

    vsOut.pos = transformStatic(mpos, ModelViewProjection);
//    vsOut.ambient = float4(1.0f,1.0f,1.0f,1.0f);
    vsOut.diffuse = float4(1.0f,1.0f,1.0f,1.0f);

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
        ZWriteEnable = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = <MatTranslucency>;
	SrcBlend         = One;
	DestBlend        = One;
        
        AlphaFunc        = Greaterequal;
        AlphaRef         = <AlphaRef>;
        AlphaTestEnable  = False;

        CullMode = <CullMode>;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = 0;

        Sampler[0]   = <WeightSampler>;
	Sampler[1]   = <DetailSampler>;

	ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;

//	ColorOp[1] = Disable;
	
	ColorOp[1]   = Modulate;
	ColorArg1[1] = Texture;
	ColorArg2[1] = Current;

	ColorOp[2]   = Disable;
    }
}

