

//float4x4 coordTransform;
//texture baseTexture;  
//float4 diffColor;
float4x4 ModelViewProjection;
texture DiffMap0;  
float4 MatDiffuse;


struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 diffuse  : COLOR0;
    float2 uv0 	    : TEXCOORD0;
};


sampler DiffuseMap = sampler_state
{
    //Texture = <baseTexture>;
    Texture   = <DiffMap0>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


VS_OUTPUT vsMain(const VS_INPUT vsIn)
{
    VS_OUTPUT vsOut;
    //vsOut.position = mul(float4(vsIn.position.xy, 0.0f, 1.0), coordTransform);
    vsOut.position = mul(float4(vsIn.position.xy, 0.0f, 1.0), ModelViewProjection);
    //vsOut.diffuse = diffColor;
    vsOut.diffuse = MatDiffuse;
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}


technique vs11_ffpp
{
    pass p0
    {
        VertexShader = compile vs_1_1 vsMain();
        PixelShader = 0;

        Lighting = False;
        ZEnable = False;
        ZWriteEnable = False;
        CullMode = None;
		
        Sampler[0] = <DiffuseMap>;

        AlphaBlendEnable = True;
        AlphaTestEnable = False;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
 
        ColorOp[0] = SelectArg1;
        ColorArg1[0] = Diffuse;

        AlphaOp[0] = SelectArg1; 
        AlphaArg1[0] = Texture;

        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
    }
}

