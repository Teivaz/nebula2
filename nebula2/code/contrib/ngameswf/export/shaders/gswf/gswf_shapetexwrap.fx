
//float4x4 coordTransform;
float4x4 ModelViewProjection;

//texture baseTexture;  
texture DiffMap0;
//float4 texGenS;
float4 TexGenS;
//float4 texGenT;
float4 TexGenT;
//float4 diffColor;
float4 MatDiffuse;


struct VS_INPUT
{
    float3 position : POSITION;
};


struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 diffuse  : COLOR0;
    float2 texture0 : TEXCOORD0;
};


sampler DiffuseMap = sampler_state
{
    //Texture = <baseTexture>;
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
    //vsOut.position = mul(float4(vsIn.position.xy, 0.0, 1.0), coordTransform);
    vsOut.position = mul(float4(vsIn.position.xy, 0.0, 1.0), ModelViewProjection);
    float4 texPos = float4(vsIn.position.xy, 0.0, 1.0); 
    vsOut.texture0.x = mul(texPos, TexGenS);
    vsOut.texture0.y = mul(texPos, TexGenT);
    //vsOut.diffuse = diffColor;
    vsOut.diffuse = MatDiffuse;
    return vsOut;
}


technique vs11_ffpp
{
    pass p0
    {
        Lighting     = False;
        ZEnable      = True;
        ZWriteEnable = False;
        CullMode     = None;
			
        AlphaBlendEnable = True;
        AlphaTestEnable  = False;

        SrcBlend  = SrcAlpha;
        DestBlend = InvSrcAlpha;

        Sampler[0] = <DiffuseMap>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        colorArg2[0] = Diffuse;

        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;

        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
		
        VertexShader = compile vs_1_1 vsMain();
        PixelShader  = 0;
    }
}
