

//float4x4 coordTransform;
float4x4 ModelViewProjection;
//float4 diffColor;
float4 MatDiffuse;

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position  : POSITION;
    float4 diffuse   : COLOR0;
};


VS_OUTPUT vsMain(const VS_INPUT vsIn)
{
    VS_OUTPUT vsOut;
    //vsOut.position = mul(float4(vsIn.position.xy, 0.0f, 1.0), coordTransform);
    vsOut.position = mul(float4(vsIn.position.xy, 0.0f, 1.0), ModelViewProjection);
    //vsOut.diffuse = diffColor;
    vsOut.diffuse = MatDiffuse;
    return vsOut;
}


technique vs11_ffpp
{
    pass p0
    {
        VertexShader = compile vs_1_1 vsMain();
        PixelShader  = 0;

        Lighting     = False;
        ZEnable      = True;
        ZWriteEnable = False;
        CullMode     = None;
		
        AlphaBlendEnable = True;
        AlphaTestEnable  = False;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
 
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Diffuse;

        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Diffuse;

        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
    }
}
