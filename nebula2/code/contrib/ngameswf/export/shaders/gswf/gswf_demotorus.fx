
texture DiffMap0;        
float4x4 ModelViewProjection;

struct VS_INPUT
{
    float3 position 	: POSITION;
    float2 uv0      	: TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position  	: POSITION;
    float2 uv0 		: TEXCOORD0;
};

sampler DiffuseMap = sampler_state
{
    Texture   = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

VS_OUTPUT vsMain(const VS_INPUT vsIn)
{
    VS_OUTPUT vsOut;
    vsOut.position = mul(float4(vsIn.position, 1.0), ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

technique vs11_ffpp
{
    pass p0
    {
	ZEnable      = True;
	ZWriteEnable = True;
			
	AlphaBlendEnable = False;
	AlphaTestEnable  = False;

        Sampler[0] = <DiffuseMap>;

        VertexShader = compile vs_1_1 vsMain();
        PixelShader  = 0;
        
	//CullMode = CW;
        CullMode = None;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;

	ColorOp[1] = Disable;
        AlphaOp[0] = Disable;
    }
}

