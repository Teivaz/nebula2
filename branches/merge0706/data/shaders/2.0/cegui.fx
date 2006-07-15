//------------------------------------------------------------------------------
//  cegui.fx
//
//  A 2d rectangle shader for GUI rendering.
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;   // the modelview*projection matrix
texture DiffMap0;

//------------------------------------------------------------------------------
struct vsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 color    : COLOR;
};

struct vsOutput
{
    float4 position  : POSITION;
    float2 uv0 : TEXCOORD0;
    float4 color    : COLOR;
};

struct psOutput
{
    float4 color : COLOR0;
};

//------------------------------------------------------------------------------
sampler ColorMap = sampler_state
{
    Texture = <DiffMap0>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

//------------------------------------------------------------------------------
vsOutput vsMain(const vsInput vsIn)
{
    vsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.color = vsIn.color;
    return vsOut;
}

//------------------------------------------------------------------------------
psOutput psMain(const vsOutput psIn)
{
    psOutput psOut;
    psOut.color = tex2D(ColorMap, psIn.uv0) * psIn.color;
    return psOut;
}

//------------------------------------------------------------------------------
technique tColor
{
    pass p0
    {
        ZWriteEnable     = True;
        ZEnable          = True;
        ZFunc            = LessEqual;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = False;
        StencilEnable    = False;

        CullMode = None;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain();
    }
}
